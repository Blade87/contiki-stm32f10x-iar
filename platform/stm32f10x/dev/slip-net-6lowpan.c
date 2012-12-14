/*
 * Copyright (c) 2011, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "contiki.h"
#include "net/netstack.h"
#include "net/uip.h"
#include "net/packetbuf.h"
#include "dev/slip.h"
#include <stdio.h>

#define DEBUG 0
#if DEBUG
#define PRINTF(...)  printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#ifndef SLIPLOWPAN_CONF_NEIGHBOR_INFO
/* Default is to use neighbor info updates if using RPL */
#define SLIPLOWPAN_CONF_NEIGHBOR_INFO UIP_CONF_IPV6_RPL
#endif /* SLIPLOWPAN_CONF_NEIGHBOR_INFO */


#define BUF                 ((uint8_t *)&uip_buf[UIP_LLH_LEN])
#define UIP_IP_BUF          ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_UDP_BUF          ((struct uip_udp_hdr *)&uip_buf[UIP_LLIPH_LEN])
#define UIP_TCP_BUF          ((struct uip_tcp_hdr *)&uip_buf[UIP_LLIPH_LEN])
#define UIP_ICMP_BUF          ((struct uip_icmp_hdr *)&uip_buf[UIP_LLIPH_LEN])


static rimeaddr_t laddr;
static rimeaddr_t arch_rime_addr;

/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/** \brief Take an IP packet and format it to be sent on an 802.15.4
 *  network using 6lowpan.
 *  \param localdest The MAC address of the destination
 *
 *  The IP packet is initially in uip_buf. Its header is compressed
 *  and if necessary it is fragmented. The resulting
 *  packet/fragments are put in packetbuf and delivered to the 802.15.4
 *  MAC.
 */
static uint8_t
tip_output(uip_lladdr_t *localdest)
{
  uint8_t buf[RIMEADDR_SIZE+4];
  
  rimeaddr_t dest;
 
  /*
   * The destination address will be tagged to each outbound
   * packet. If the argument localdest is NULL, we are sending a
   * broadcast packet.
   */
  if(localdest == NULL) {
     rimeaddr_copy(&dest, &rimeaddr_null);
  } else {
    rimeaddr_copy(&dest, (const rimeaddr_t *)localdest);
  }
  
  if (0 == rimeaddr_cmp(&dest, &laddr)) {
    // send rime addr
    buf[0]='!'; buf[1]='R';
    rimeaddr_copy((rimeaddr_t *)&buf[2],(const rimeaddr_t *)localdest);
    slip_write(buf, 2+RIMEADDR_SIZE);
    rimeaddr_copy(&laddr, (const rimeaddr_t *)localdest);
  }
#if DEBUG
  {
      uint16_t i;
  PRINTF(">>>len(%u) ", uip_len);
  for (i=0; i<uip_len; i++)PRINTF(":%02X", BUF[i]);
  PRINTF("\r\n");
  }
#endif
  packetbuf_set_addr(PACKETBUF_ADDR_RECEIVER, &laddr);
  slip_write(&uip_buf[UIP_LLH_LEN], uip_len);
  return 1;
}

/*---------------------------------------------------------------------------*/
#include "net/neighbor-info.h"
#include "net/uip-ds6.h"


void slip_input_call(void)
{
  int i;
  if (uip_len > 0) {
  PRINTF("<<<len(%u): ", uip_len);
  for (i=0; i<uip_len; i++)PRINTF("%02X ", BUF[i]);
  PRINTF("\r\n");
  }

  if (BUF[0]=='!') {
    if (BUF[1]=='S') {
#if SLIPLOWPAN_CONF_NEIGHBOR_INFO
      PRINTF("slip-net-6lowpan:lower layers call _neighbor_info_packet_sent(%u,%u)\n",
             BUF[3], BUF[4]);
      neighbor_info_packet_sent(BUF[3], BUF[4]);
#endif
      uip_len = 0;
    } else
    if (BUF[1]=='R') { 
#if SLIPLOWPAN_CONF_NEIGHBOR_INFO
       const rimeaddr_t *src = (const rimeaddr_t *)&BUF[2]; 
       PRINTF("neighbor-info: packet received from %d.%d\n",
        src->u8[sizeof(*src) - 2], src->u8[sizeof(*src) - 1]);
        packetbuf_set_addr(PACKETBUF_ADDR_SENDER, src);
        neighbor_info_packet_received();
#endif
        uip_len = 0;
    } else
      if (BUF[1]=='M') {
        //set_rime_addr(&BUF[2]);
        rimeaddr_copy(&arch_rime_addr, (rimeaddr_t *)&BUF[2]);
         uip_len = 0;
      }
  }

}



void
slipnet_init(void)
{
  tcpip_set_outputfunc(tip_output);
  slip_set_input_callback(slip_input_call);

  slip_arch_init(115200);
  process_start(&slip_process, NULL);
 
}

#define SLIP_END     0300
#define SLIP_ESC     0333
#define SLIP_ESC_END 0334
#define SLIP_ESC_ESC 0335
/*---------------------------------------------------------------------------*/
#include "dev/uart2.h"
static int uart_rbyte(unsigned char c)
{
  static unsigned char st = 0;
  static unsigned char idx = 0;
  switch(st) {
  case 0:
    if (c == SLIP_END){
      st = 1;
      idx = 0;
    } else {
      if (c=='\r'||c=='\n' || (c>=' ' && c<='~'))printf("%c", c);
    }
    return 0;
  case 1:
    if (c == SLIP_ESC) {
      st = 2;
      return 0;
    } else if (c == SLIP_END) {
      /* got packet */
      if (idx == RIMEADDR_SIZE+2 && uip_buf[0]=='!' && uip_buf[1]=='M') {
        rimeaddr_copy(&arch_rime_addr, (rimeaddr_t *)&uip_buf[2]); 
      }
      idx = 0;
      st = 0;
      return 1;
    }
    break;
  case 2:
    if (c == SLIP_ESC_END) c = SLIP_END,st=1;
    else if (c == SLIP_ESC_ESC) c = SLIP_ESC,st=1;
    else st = 0;
  }
  if (idx < sizeof(uip_buf)) {
    uip_buf[idx++] = c; 
  } else {
    st = 0;
    idx = 0;
  }
  return 0;
}

char *get_arch_rime_addr(void)
{
  struct timer tm;
  
  uart2_set_input(uart_rbyte);
  while (rimeaddr_cmp(&arch_rime_addr, &rimeaddr_null)) {
    printf("send rime addr request!\r\n");
    slip_write("?M", 2); 
    timer_set(&tm, CLOCK_SECOND*2);
    while (!timer_expired(&tm));
  }
  return (char *)&arch_rime_addr;
}

/*---------------------------------------------------------------------------*/
