#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "dev/leds.h"

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



RESOURCE(leds, METHOD_GET | METHOD_POST | METHOD_PUT , "control/led", "title=\"LEDs: ?led=1|2, POST/PUT mode=on|off|toggle\";rt=\"Control\"");

void
leds_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  size_t len = 0;
  const char *nb = NULL;
  const char *mode = NULL;
  uint8_t led = 0xff;
 

  static char buf[32];
  
  if ((len=REST.get_query_variable(request, "led", &nb))) {
    if (len==1 && nb[0]>='1' && nb[0]<='7')  {
      led = nb[0] - '0';
    } else {
      REST.set_response_status(response, REST.status.BAD_REQUEST);
      return;
    }
  }
 
  if ((METHOD_GET != coap_get_rest_method(request))
      && (len=REST.get_post_variable(request, "mode", &mode))) {
        if (strncmp(mode, "on", len)==0) {
           leds_on(led);
        } else if (strncmp(mode, "off", len)==0) {
           leds_off(led);
        } else if (strncmp(mode, "toggle", len)==0) {
           leds_toggle(led);
        } else {
          REST.set_response_status(response, REST.status.BAD_REQUEST);
          return;
        }
  }
  len = 0;
  if (led==0xff) {
    for (led=0; led<2; led++) {
      len += snprintf(buf+len, sizeof(buf)-len, "led%d=%s;", led+1, (leds_get()&(1<<led))?"on":"off");      
    }
  } else 
    snprintf(buf, sizeof(buf), "led%d=%s;", led, (leds_get()&(1<<(led-1)))?"on":"off");   
  
  REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
  REST.set_response_payload(response, buf, strlen(buf));
}