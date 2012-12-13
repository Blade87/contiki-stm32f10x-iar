#include "contiki.h"
#include "net/netstack.h"
#include "net/uip.h"
#include "net/packetbuf.h"
#include "dev/slip.h"
#include <stdio.h>

#define SLIP_END     0300
#define SLIP_ESC     0333
#define SLIP_ESC_END 0334
#define SLIP_ESC_ESC 0335

#define DEBUG 1
#if DEBUG
#define PRINTF(...)  printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif


#if UIP_CONF_LL_802154 == 0
static void _output(void)
{
  PRINTF("slipnet slip_output: sending packet len %d\n", uip_len);
  slip_send();  
}

static void _init(void)
{
}
/*---------------------------------------------------------------------------*/

struct uip_fallback_interface slipnet_interface = {
  _init, _output
};
#endif