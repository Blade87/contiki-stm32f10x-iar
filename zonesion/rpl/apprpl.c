#include "contiki.h"
#include "net/uip.h"
#include "net/uip-udp-packet.h"
#include "net/uip-ds6.h"
#include "net/rpl/rpl.h"

#include <stdio.h> /* For printf() */
#include <string.h>


#define SEND_INTERVAL (4 * CLOCK_SECOND)

#define UIP_UDP_BUF  ((char *)(&uip_buf[UIP_LLH_LEN + UIP_IPUDPH_LEN]))
#define UIP_UDP_BUF_LEN (sizeof(uip_buf)-(UIP_LLH_LEN + UIP_IPUDPH_LEN))

extern uip_ds6_nbr_t uip_ds6_nbr_cache[];
extern uip_ds6_defrt_t uip_ds6_defrt_list[]; 
extern uip_ds6_route_t uip_ds6_routing_table[];

static struct uip_udp_conn *udp_conn;

static char* ipaddr_str(const uip_ipaddr_t *addr)
{
  static char buf[40];
  char b[4];
  uint16_t a;
  
  int i, f;
  buf[0] = 0;
  for(i = 0, f = 0; i < sizeof(uip_ipaddr_t); i += 2) {
    a = (addr->u8[i] << 8) + addr->u8[i + 1];
    if(a == 0 && f >= 0) {
      if(f++ == 0) strcat(buf, "::");
    } else {
      if(f > 0) {
        f = -1;
      } else if(i > 0) {
        strcat(buf, ":");
      }
      sprintf(b, "%x", a);
      strcat(buf, b);
    }
  }
  return buf;
}


static void ondr_report(void)
{
  int i, n=0;

  UIP_UDP_BUF[0] = 'O';
#if RPL_CONF_LEAF_ONLY  
  UIP_UDP_BUF[1] = 3;
#else
  UIP_UDP_BUF[1] = 2;
#endif
  uip_len = 2;
  
  UIP_UDP_BUF[uip_len] = 'N';
  n = 0;
  for(i = 0; i < UIP_DS6_NBR_NB; i++) {
    if(uip_ds6_nbr_cache[i].isused && uip_ds6_nbr_cache[i].state == NBR_REACHABLE) {
      memcpy(&UIP_UDP_BUF[uip_len+2+n*8], &uip_ds6_nbr_cache[i].ipaddr.u8[8], 8);
      n += 1;
    }
  }
  UIP_UDP_BUF[uip_len+1] = n;
  uip_len += 2 + n * 8;
  
  UIP_UDP_BUF[uip_len] = 'D';
  n = 0;
  for(i = 0; i < UIP_DS6_DEFRT_NB; i++) {
    if(uip_ds6_defrt_list[i].isused) {
      memcpy(&UIP_UDP_BUF[uip_len+2+n*8], &uip_ds6_defrt_list[i].ipaddr.u8[8], 8);
      n += 1;
    }
  }
  UIP_UDP_BUF[uip_len+1] = n;
  uip_len += 2 + n * 8;
  
    UIP_UDP_BUF[uip_len] = 'R';
  n = 0;
  for(i = 0; i < UIP_DS6_ROUTE_NB; i++) {
    if(uip_ds6_routing_table[i].isused) {
      memcpy(&UIP_UDP_BUF[uip_len+2+n*8], &uip_ds6_routing_table[i].ipaddr.u8[8], 8);
      n += 1;
    }
  }
  UIP_UDP_BUF[uip_len+1] = n;
  uip_len += 2 + n * 8;
  
}
static void route_report(void)
{
  int i, nb = 0;
  
  UIP_UDP_BUF[0] = 'O';
#if UIP_CONF_IPV6_RPL  
  UIP_UDP_BUF[1] = 2;
#else
  UIP_UDP_BUF[1] = 3;
#endif  
  UIP_UDP_BUF[2] = 'R';
  for(i = 0; i < UIP_DS6_NBR_NB; i++) {
    if(uip_ds6_routing_table[i].isused) {
      memcpy(&UIP_UDP_BUF[4+nb*8], &uip_ds6_routing_table[i].ipaddr.u8[8], 8);
      nb += 1;
    }
  }
  UIP_UDP_BUF[3] = nb;
  uip_len = nb * 8 + 4;
}

static void other_report(void) 
{
  UIP_UDP_BUF[0] = 'O';
#if UIP_CONF_IPV6_RPL  
  UIP_UDP_BUF[1] = 1;
#else
  UIP_UDP_BUF[1] = 2;
#endif
  
}

static void rplapp_handler(void)
{
  //static unsigned int seq = 0;  

  //sprintf(UIP_UDP_BUF, "%u:N[", ++seq);
  //if (++seq % 3 == 0x00) nbr_report();
  //else if (seq % 3 == 1) route_report();
  //else other_report();
  ondr_report();
  uip_udp_packet_send(udp_conn, UIP_UDP_BUF, uip_len);
}
/*---------------------------------------------------------------------------*/
PROCESS(apprpl_process, "apprpl process");
//AUTOSTART_PROCESSES(&hello_world_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(apprpl_process, ev, data)
{
  static struct etimer et;
  uip_ipaddr_t ipaddr;
  
  PROCESS_BEGIN();
  
  uip_ip6addr(&ipaddr, 0xaaaa,0,0,0,0,0,0,1);
  
  udp_conn = udp_new(&ipaddr, UIP_HTONS(3000), NULL);
  
  printf("UIP_UDP_BUF_LEN = %u\r\n", UIP_UDP_BUF_LEN);
  etimer_set(&et, SEND_INTERVAL);
  while(1) {
    PROCESS_YIELD();
    rplapp_handler();
    etimer_set(&et, SEND_INTERVAL);
  }
  
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/