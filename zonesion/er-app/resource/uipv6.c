#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"

/* For CoAP-specific example: not required for normal RESTful Web service. */
#if WITH_COAP == 3
#include "er-coap-03.h"
#elif WITH_COAP == 7
#include "er-coap-07.h"
#else
#warning "Erbium example without CoAP-specifc functionality"
#endif /* CoAP-specific example */

#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]",(lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3],(lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

extern uip_ds6_nbr_t uip_ds6_nbr_cache[];
extern uip_ds6_defrt_t uip_ds6_defrt_list[]; 
extern uip_ds6_route_t uip_ds6_routing_table[];

/*
 * Example for a resource that also handles all its sub-resources.
 * Use REST.get_url() to multiplex the handling of the request depending on the Uri-Path.
 */
PERIODIC_RESOURCE(uipv6, METHOD_GET, ".uipv6", "title=\"uipv6 mash top data\";obs", 3*CLOCK_SECOND);

static void
uipv6_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  //const char *uri_path = NULL;
  //int len = REST.get_url(request, &uri_path);
  //int base_len = strlen(resource_uipv6.url);

      REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
      snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{\"type\":2}");
      REST.set_response_payload(response, buffer, strlen((char *)buffer));
}

static void
uipv6_periodic_handler(resource_t *r)
{
  static uint16_t obs_counter = 0;
//  static char content[11];
//  char *msg = get_voltage();
  ++obs_counter;

  //PRINTF("TICK %u for /%s\n", obs_counter, r->url);

  /* Build notification. */
  coap_packet_t notification[1]; /* This way the packet can be treated as pointer as usual. */
  coap_init_message(notification, COAP_TYPE_NON, CONTENT_2_05, APPLICATION_X_OBIX_BINARY );
  
  static char buf[REST_MAX_CHUNK_SIZE];
  int len = 1;
  if (obs_counter % 3 == 0) {
    int i;
    buf[0] = 'D';
    for(i = 0; i < UIP_DS6_DEFRT_NB && len<REST_MAX_CHUNK_SIZE; i++) {
      if(uip_ds6_defrt_list[i].isused) {
        memcpy(&buf[len], &uip_ds6_defrt_list[i].ipaddr.u8[8], 8);
        buf[len] ^= 2;
        len += 8;
      }
    }
  } else if (obs_counter % 3 == 1) {
    int i;
    buf[0] = 'R';
    for(i = 0; i < UIP_DS6_DEFRT_NB && len<REST_MAX_CHUNK_SIZE; i++) {
      if(uip_ds6_routing_table[i].isused) {
        memcpy(&buf[len], &uip_ds6_routing_table[i].ipaddr.u8[8], 8);
        buf[len] ^= 2;
        len += 8;
      }
    }   
  } else {
    int i;
    buf[0] = 'N';
    for(i = 0; i < UIP_DS6_DEFRT_NB && len<REST_MAX_CHUNK_SIZE; i++) {
      if(uip_ds6_nbr_cache[i].isused) {
        memcpy(&buf[len], &uip_ds6_nbr_cache[i].ipaddr.u8[8], 8);
        buf[len] ^= 2;
        len += 8;
      }
    } 
  }
  
  coap_set_payload(notification, buf, len);

  /* Notify the registered observers with the given message type, observe option, and payload. */
  REST.notify_subscribers(r, obs_counter, notification);
}