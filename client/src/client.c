/*
   Copyright 2017 Clemens Jonischkeit

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <stdio.h>
#include <stdlib.h>
#include <interface.h>

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