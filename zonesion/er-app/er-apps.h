
#define RESOURCE_ANNOUNCE(name)   extern resource_t resource_##name
#define PERIODIC_RESOURCE_ANNOUNCE(name) extern periodic_resource_t periodic_resource_##name


#define RS_HELLOWORLD   0

#define RS_VOLTAGE      1
#define VOLTAGE_PERIODIC   0

#define RS_UIPV6        1
#define RS_LEDS         1
#define RS_BUTTON       1

#define RS_TANDH        1
#define TANDH_PERIODIC  1


RESOURCE_ANNOUNCE(helloworld);
//RESOURCE_ANNOUNCE(voltage);
//RESOURCE_ANNOUNCE(DHT11);
RESOURCE_ANNOUNCE(leds);
RESOURCE_ANNOUNCE(button);

RESOURCE_ANNOUNCE(DI1);
PERIODIC_RESOURCE_ANNOUNCE(AI1);
RESOURCE_ANNOUNCE(DO1);
RESOURCE_ANNOUNCE(AO1);

void button_event_handler(resource_t *r);

#if VOLTAGE_PERIODIC
PERIODIC_RESOURCE_ANNOUNCE(voltage);
#else
RESOURCE_ANNOUNCE(voltage);
#endif

#if TANDH_PERIODIC
PERIODIC_RESOURCE_ANNOUNCE(DHT11);
#else
RESOURCE_ANNOUNCE(DHT11);
#endif

PERIODIC_RESOURCE_ANNOUNCE(uipv6);