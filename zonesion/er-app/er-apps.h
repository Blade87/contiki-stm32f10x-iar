
#define RESOURCE_ANNOUNCE(name)   extern resource_t resource_##name
#define PERIODIC_RESOURCE_ANNOUNCE(name) extern periodic_resource_t periodic_resource_##name


#define RS_HELLOWORLD   1
#define RS_VOLTAGE      1
#define RS_UIPV6        1


RESOURCE_ANNOUNCE(helloworld);
RESOURCE_ANNOUNCE(voltage);
PERIODIC_RESOURCE_ANNOUNCE(voltage);



PERIODIC_RESOURCE_ANNOUNCE(uipv6);