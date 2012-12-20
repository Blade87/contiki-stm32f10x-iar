#include <stm32f10x_map.h>
#include <stm32f10x_dma.h>
#include <gpio.h>
#include <nvic.h>
#include <stdint.h>
#include <stdio.h>
#include <debug-uart.h>
#include <sys/process.h>
#include <sys/procinit.h>
#include <etimer.h>
#include <sys/autostart.h>
#include <clock.h>

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "dev/slip.h"

#include "dev/uart1.h"
#include "dev/uart2.h"
#include "dev/slip-net.h"

#include <string.h>

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF(" %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x ", ((u8_t *)addr)[0], ((u8_t *)addr)[1], ((u8_t *)addr)[2], ((u8_t *)addr)[3], ((u8_t *)addr)[4], ((u8_t *)addr)[5], ((u8_t *)addr)[6], ((u8_t *)addr)[7], ((u8_t *)addr)[8], ((u8_t *)addr)[9], ((u8_t *)addr)[10], ((u8_t *)addr)[11], ((u8_t *)addr)[12], ((u8_t *)addr)[13], ((u8_t *)addr)[14], ((u8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF(" %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x ",lladdr.u8[0], lladdr.u8[1], lladdr.u8[2], lladdr.u8[3],lladdr.u8[4], lladdr.u8[5], lladdr.u8[6], lladdr.u8[7])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

unsigned int idle_count = 0;

#define XMACADDR "\x00\x80\xe1\x02\x00\x1d\x51\xba"

void set_rime_addr(void *addr)
{

  memcpy(&uip_lladdr.addr, addr, sizeof(uip_lladdr.addr));
  
#if UIP_CONF_IPV6
  rimeaddr_set_node_addr((rimeaddr_t *)&uip_lladdr.addr);
#else
  rimeaddr_set_node_addr((rimeaddr_t *)&uip_lladdr.addr[8-RIMEADDR_SIZE]);
#endif
  {
    int i;
  printf("Rime set with address ");
  for(i = 0; i < sizeof(rimeaddr_t) - 1; i++) {
    printf("%d.", rimeaddr_node_addr.u8[i]);
  }
  printf("%d\n", rimeaddr_node_addr.u8[i]);
  }  
}

char *get_arch_rime_addr(void);
int
main()
{
  //SystemInit();
  clock_init();
  //dbg_setup_uart();
  uart1_int(115200);
  PRINTF("\r\nStarting ");
  PRINTF(CONTIKI_VERSION_STRING);
  PRINTF(" on STM32F10x\r\n"); 
  
  uart2_int(115200);
   
  process_init();
  
  process_start(&etimer_process, NULL);
  
  ctimer_init();
  //rtimer_init();
  
  //set_rime_addr(XMACADDR);  
  set_rime_addr(get_arch_rime_addr());
  
#ifdef WITH_SLIP_NET
  slipnet_init(); 
#elif WITH_SERIAL_LINE_INPUT
  uart1_set_input(serial_line_input_byte);
  serial_line_init();
#endif
  
#if UIP_CONF_IPV6
  queuebuf_init();
  process_start(&tcpip_process, NULL);  
#endif /* UIP_CONF_IPV6 */
 #if  UIP_CONF_IPV6
{
    uip_ds6_addr_t *lladdr;
    int i;
    printf("Tentative link-local IPv6 address ");
    lladdr = uip_ds6_get_link_local(-1);
    
    for(i = 0; i < 7; ++i) {     
      printf("%02x%02x:", lladdr->ipaddr.u8[i * 2],
             lladdr->ipaddr.u8[i * 2 + 1]);
    }
    printf("%02x%02x\n", lladdr->ipaddr.u8[14], lladdr->ipaddr.u8[15]);
}  
#if !UIP_CONF_IPV6_RPL 
{
    uip_ipaddr_t ipaddr; 
    int i; 
    uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
    uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
    uip_ds6_addr_add(&ipaddr, 0, ADDR_TENTATIVE); 
    printf("Tentative global IPv6 address ");
    for(i = 0; i < 7; ++i) {
      printf("%02x%02x:",     
        ipaddr.u8[i * 2], ipaddr.u8[i * 2 + 1]);
    }
    printf("%02x%02x\n",   
           ipaddr.u8[7 * 2], ipaddr.u8[7 * 2 + 1]);
    
    //uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
    uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0x212, 0x4b00, 0x237, 0x7e30);
    uip_ds6_defrt_add(&ipaddr, 0);
    printf("default router ");
    for(i = 0; i < 7; ++i) {
      printf("%02x%02x:",     
        ipaddr.u8[i * 2], ipaddr.u8[i * 2 + 1]);
    }
    printf("%02x%02x\n",   
           ipaddr.u8[7 * 2], ipaddr.u8[7 * 2 + 1]);
}
#endif //UIP_CONF_IPV6_RPL
#endif //UIP_CONF_IPV6

#ifdef WITH_RPL_APP
{
  PROCESS_NAME(apprpl_process);
  process_start(&apprpl_process, NULL);
}
#endif

 autostart_start(autostart_processes);
  while(1) {
    do {
    } while(process_run() > 0);
    idle_count++;
    /* Idle! */
    /* Stop processor clock */
    /* asm("wfi"::); */ 
  }
  return 0;
}

#if UIP_CONF_LOGGING
void uip_log(char *msg)
{
  printf("%u : %s\n", clock_time(), msg);
}
#endif
/* assert_param() ************************/
void assert_param(int b)
{
}


