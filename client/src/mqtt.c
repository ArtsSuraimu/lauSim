#include <mosquitto.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <interface.h>

#define back_topic "envelope/master"
#define last_will_topic "last_will"
#define check(a,b) do {if ((a) != (b)) return -1;} while(0)

struct mosquitto *con = NULL;
pthread_mutex_t rcv_mutex = PTHREAD_MUTEX_INITIALIZER;
volatile size_t fault_num;
fault *faults = NULL;
fault *faults_save = NULL;
int token;
volatile int rcv;

void on_message(struct mosquitto *cn, void *arg, const struct mosquitto_message *msg);

void on_connect(struct mosquitto *mosq, void * data, int st) {
    printf("Connection: %s\n", mosquitto_connack_string(st));
}

void on_subscribe(struct mosquitto *mosq, void *data, int mid, int qos_count, const int *qos) {
    int i;
    if (mid == token) {
        printf("subscribed! [");
        for (i = 0; i < qos_count - 1; i++) {
            printf("%d, ", qos[i]);
        }
        if (qos_count != 0)
            printf("%d",qos[i]);
        printf("]\n");
    }
}

int init(const char *name, void *opt) {
    char top_name[512];
    struct mqtt_opt *mopt = (struct mqtt_opt*) opt;
    mosquitto_lib_init();
    con = mosquitto_new(name, true, NULL);
    if (con == NULL)
        return 1;
    check(mosquitto_will_set(con, last_will_topic, strlen(name), name, 1, false), MOSQ_ERR_SUCCESS);
    mosquitto_message_callback_set(con, on_message);
    mosquitto_connect_callback_set(con, on_connect);
    mosquitto_subscribe_callback_set(con, on_subscribe);
    check(mosquitto_connect(con, mopt->hostname, mopt->port, 60), MOSQ_ERR_SUCCESS);
    snprintf(top_name, sizeof(top_name) - 1, "envelope/%s/#", name);
    printf("subscribing to: %s\n", top_name);
    check(mosquitto_subscribe(con, &token, top_name, 1), MOSQ_ERR_SUCCESS);
    return 0;
}

int send_back(uint8_t *buf, size_t len) {
    if (con == NULL) return -1;
    check(mosquitto_publish(con, NULL, back_topic, len, buf, 1, false), MOSQ_ERR_SUCCESS);
    return 0;
}

void on_message(struct mosquitto *cn, void *arg, const struct mosquitto_message *msg) {
    char *buf;
    char *cmp;
    size_t len;
    pthread_mutex_lock(&rcv_mutex);
    printf("new message!\n");
    rcv = 1;
    faults = realloc(faults, ++fault_num);
    len = strlen(msg->topic) + 1;
    buf = malloc(len);
    strncpy(buf, msg->topic, len);
    strtok(buf, "/");           // "envelope"
    strtok(NULL, "/");          // "name"
    cmp = strtok(NULL, "/");    // "component"
    if (cmp == NULL) {
        faults[fault_num - 1].component = NULL;
    } else {
        len = strlen(cmp) + 1;
        faults[fault_num - 1].component = malloc(len);
        strncpy(faults[fault_num - 1].component, cmp, len);
    }
    free(buf);
    faults[fault_num - 1].severity = strtoul(msg->payload, NULL, 0);
    pthread_mutex_unlock(&rcv_mutex);
}

fault *get_faults(size_t timeout, size_t *num) {
    *num = 0;
    int x;

    if (con == NULL)
        return NULL;
    pthread_mutex_lock(&rcv_mutex);
    do {
        rcv = 0;
        pthread_mutex_unlock(&rcv_mutex);
        if ((x = mosquitto_loop(con, timeout, 1)) != MOSQ_ERR_SUCCESS) {
            printf("loop failed for reason: %s\n", mosquitto_strerror(x));
            return NULL;
        }
        pthread_mutex_lock(&rcv_mutex);
    } while(rcv);

    // printf("%zd failes\n", fault_num);

    free(faults_save);
    faults_save = faults;
    faults = NULL;
    *num = fault_num;
    fault_num = 0;
    pthread_mutex_unlock(&rcv_mutex);
    return faults_save;
}

void cleanup(void) {
    if (con != NULL) {
        mosquitto_disconnect(con);
        mosquitto_destroy(con);
    }
    mosquitto_lib_cleanup();
}