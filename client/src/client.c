#include <stdio.h>
#include <stdlib.h>
#include <interface.h>

#define CON MQTT

int main(int argc, char **argv) {
    fault *flt;
    size_t num, i;
#if CON == MQTT
    struct mqtt_opt opt;
#endif

#if CON == MQTT
    if (argc < 4) {
        printf("usage: %s name host port\n", argc ? argv[0] : "com_mqtt");
        return -1;
    }
    opt.hostname = argv[2];
    opt.port = strtoul(argv[3], NULL, 0);
    if (init(argv[1], &opt) != 0){
        printf("mqtt init failed\n");
        return -2;
    }
#endif

    while(1) {
        flt = get_faults(100, &num);
        for (i = 0; i < num; i++) {
            printf("%s failed (%3d)\n", flt[i].component, flt[i].severity);
        }
    }
       
}