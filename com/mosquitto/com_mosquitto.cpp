
#include <sstream>
#include <cstdio>
#include <cstring>
#include <string>
#include <unistd.h>


#include "../../include/node.h"
#include "../../include/component.h"
#include "../../include/mosquitto/com_mosquitto.h"

#define HOST_NAME_MAX 255
#define LAST_WILL_TOPIC "last_will"

using namespace lauSim;
static bool init_state = false;

int
ComMosquitto::init_mosquitto() {
    int ret = 0;
    if (!init_state) {
        ret = mosqpp::lib_init();
        if (!ret)
            init_state = true;
        
    }
    return ret;
}

int
ComMosquitto::cleanup_mosquitto() {
    int ret = 0;
    if (init_state) {
        ret = mosqpp::lib_cleanup();
    }
    return ret;
}

int ComMosquitto::init(const char * id, const char * addr, int port, unsigned keep_alive) {
    char buf[HOST_NAME_MAX + 1];
    if (is_init)
        return -1;
    con = mosqpp::mosquittopp(id);
    con.connect(addr, port, keep_alive);
    memset(buf, 0, sizeof(buf));
    gethostname(buf, sizeof(buf));
    hostname = std::string(buf);
    con.will_set(LAST_WILL_TOPIC, strlen(buf), buf);
    is_init = true;
    return 0;
}

bool ComMosquitto::send_fail(Node *target, Component *cmp, failure_type *type, unsigned severity){
    std::stringstream topic;
    std::stringstream msg_buf;
    if (!is_init)
        return false;
    topic << "envelope/" << target->name << "/" << cmp->name;
    msg_buf << type->name << " 0x" << std::hex << severity;
    const char *msg = msg_buf.str().c_str();
    con.publish(NULL, topic.str().c_str(), strlen(msg), msg);
    return true;
}
