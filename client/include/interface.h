#include <stdint.h>
#include <sys/types.h>

typedef struct fault_s {
    char *component;
    unsigned severity;
} fault;

struct mqtt_opt{
    char *hostname;
    uint16_t port;
};

int init(const char *name, void *opt);
fault *get_faults(size_t timeout, size_t *num);
int send_back(uint8_t *buf, size_t len);
void cleanup(void);