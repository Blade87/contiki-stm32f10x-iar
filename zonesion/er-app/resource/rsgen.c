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

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]",(lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3],(lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

#include "../er-apps.h"


EVENT_RESOURCE(DI1, METHOD_GET, "sensor/DI1", "title=\"Digital input chanl 1\";obs");
PERIODIC_RESOURCE(AI1, METHOD_GET, "sensor/AI1", "title=\"Periodic Analog input 1\";obs", 5*CLOCK_SECOND);
RESOURCE(DO1, METHOD_GET | METHOD_POST | METHOD_PUT , "control/DO1", "title=\"Ditital output chanl 1\";rt=\"Control\"");
RESOURCE(AO1, METHOD_GET | METHOD_POST | METHOD_PUT , "control/AO1", "title=\"Analog output chanl 1\";rt=\"Control\"");

static int di1_in(void) 
{
  return random_rand()%2;
}
static int ai1_in(void)
{
  return random_rand()%255;
}

static void
DI1_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
  /* Usually, a CoAP server would response with the current resource representation. */
  char msg[4];
  snprintf(msg, sizeof(msg), "%u", di1_in());
  REST.set_response_payload(response, (uint8_t *)msg, strlen(msg));

  /* A post_handler that handles subscriptions/observing will be called for periodic resources by the framework. */
}

/* Additionally, a handler function named [resource name]_event_handler must be implemented for each PERIODIC_RESOURCE defined.
 * It will be called by the REST manager process with the defined period. */
void
DI1_event_handler(resource_t *r) 
{
  static uint16_t event_counter = 0;
 
  ++event_counter;

  PRINTF("TICK %u for /%s\n", event_counter, r->url);
  char msg[4];
  snprintf(msg, sizeof(msg), "%u", di1_in());
  /* Build notification. */
  coap_packet_t notification[1]; /* This way the packet can be treated as pointer as usual. */
  coap_init_message(notification, COAP_TYPE_NON, CONTENT_2_05, 0);
  REST.set_header_content_type(notification, REST.type.APPLICATION_JSON);
  coap_set_payload(notification, msg, strlen(msg));

  /* Notify the registered observers with the given message type, observe option, and payload. */
  REST.notify_subscribers(r, event_counter, notification);
}

/*----------------------------------------------------------------------------*/
static void
AI1_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{

      REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
      snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%u", ai1_in());
      REST.set_response_payload(response, buffer, strlen((char *)buffer));
}

static void
AI1_periodic_handler(resource_t *r)
{
  static uint16_t obs_counter = 0;
  ++obs_counter;
  char msg[4];
  snprintf((char *)msg, sizeof(msg), "%u", ai1_in());
  
  /* Build notification. */
  coap_packet_t notification[1]; /* This way the packet can be treated as pointer as usual. */
  coap_init_message(notification, COAP_TYPE_NON, CONTENT_2_05, TEXT_PLAIN );
  
  coap_set_payload(notification, msg, strlen(msg));
  /* Notify the registered observers with the given message type, observe option, and payload. */
  REST.notify_subscribers(r, obs_counter, notification);
}

/*----------------------------------------------------------------------------*/
static void
DO1_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
        REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
      snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "do1 output ");
      REST.set_response_payload(response, buffer, strlen((char *)buffer));
}

/*----------------------------------------------------------------------------*/
static void
AO1_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
        REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
      snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "ao1 output");
      REST.set_response_payload(response, buffer, strlen((char *)buffer));
}