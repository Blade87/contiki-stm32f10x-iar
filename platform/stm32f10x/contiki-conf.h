#ifndef __CONTIKI_CONF_H__CDBB4VIH3I__
#define __CONTIKI_CONF_H__CDBB4VIH3I__

#include <stdint.h>

#define CCIF
#define CLIF

#define WITH_UIP 1
#define WITH_ASCII 1

#define CLOCK_CONF_SECOND 100

/* These names are deprecated, use C99 names. */
typedef uint8_t u8_t;
typedef uint16_t u16_t;
typedef uint32_t u32_t;
typedef int8_t s8_t;
typedef int16_t s16_t;
typedef int32_t s32_t;

typedef unsigned int clock_time_t;
typedef unsigned int uip_stats_t;

#ifndef BV
#define BV(x) (1<<(x))
#endif
#define AUTOSTART_ENABLE        1

#ifdef WITH_II_802154
#define UIP_CONF_LL_802154           1
#define RIMEADDR_CONF_SIZE           8

#ifdef WITH_RPL
#define UIP_CONF_IPV6_RPL            1
#define RPL_CONF_STATS               0
#define RPL_CONF_MAX_DAG_ENTRIES     1
#ifndef RPL_CONF_OF
#define RPL_CONF_OF rpl_of_etx
#endif  //RPL_CONF_OF
#else   //WITH_RPL
#define UIP_CONF_IPV6_RPL       0
#endif  //WITH_RPL
#else //WITH_II_802154
#define UIP_CONF_LL_802154      0
#define UIP_CONF_IPV6_RPL       0
#endif
//#define NETSTACK_CONF_MAC     nullmac_driver
//#define NETSTACK_CONF_RDC     contikimac_driver
//#define NETSTACK_CONF_NETWORK slipnet_driver
//#define NETSTACK_CONF_FRAMER  no_framer

#ifdef WITH_SLIP_NET 
#if UIP_CONF_LL_802154 == 0
#define UIP_FALLBACK_INTERFACE  slipnet_interface
#endif
#endif

#ifdef WITH_IPV6

#define UIP_CONF_IPV6           1

#define UIP_CONF_ICMP6          0
#define UIP_CONF_ROUTER         1
#define UIP_CONF_DS6_AADDR_NBU  1

#define UIP_CONF_TCP_FORWARD    0

#define UIP_CONF_ND6_SEND_RA    0


#define UIP_CONF_DS6_DEFRT_NBU  2
#define UIP_CONF_DS6_NBR_NBU    3
#define UIP_CONF_DS6_ROUTE_NBU  4

#endif

/* uIP configuration */
#define UIP_CONF_TCP              1
#define UIP_CONF_UDP              1

#define UIP_CONF_LLH_LEN          0
#define UIP_CONF_BROADCAST        1
#define UIP_CONF_LOGGING          1
#define UIP_CONF_BUFFER_SIZE      130

#define UIP_CONF_ND6_RETRANS_TIMER  3000

/* Prefix for relocation sections in ELF files */
#define REL_SECT_PREFIX ".rel"

#define CC_BYTE_ALIGNED __attribute__ ((packed, aligned(1)))

#define USB_EP1_SIZE 64
#define USB_EP2_SIZE 64

#define RAND_MAX 0x7fff
#endif /* __CONTIKI_CONF_H__CDBB4VIH3I__ */
