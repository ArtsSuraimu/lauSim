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


/*
 * @Author: C. Jonischkeit 
 * @Date: 2017-07-17 13:00:25
*/

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <sstream>

#include <lausim/plugin.h>
#include "plugin_internal.h"

namespace lauSim {

#define com_actor_id 0x100
#define com_notify_id 0x101
#define logger_id 0x102
#define manager_id 0x103
#define com_all_id 0x104
#define tic_id 0x105

enum backChType {
    BC_NONE,
    BC_SYNC,
    BC_ASYNC
};

/**
 * holds the configuration of the server
 */
class Config {
public:
    static Config *get_instance();
    /**
     * load the configuration from a configuration file
     * 
     * @param filename Name of the file holding the configuration
     * @return 0 on success
     */
    int load_config(const char * filename);
    void cleanup();
    /**
     * Points to the plugin used for communication with the actor
     */
    plugin *com_actor = nullptr;
    /**
     * Points to the plugin used for anouncing faults to external programs
     */
    plugin *com_notify = nullptr;
    /**
     * Points to the plugin, governing the nodes and faults
     */
    plugin *fault_manager = nullptr;
    /**
     * Points to the used logger
     */
    plugin *logger = nullptr;
    /**
     * The tic length in ns, a value of 0 also suppresses skipping tics, to keep up with time
     */
    uint64_t tic_length = 0;
    /**
     * The type of the backchannel that shall be used (deprecated)
     */
    backChType backCh;
private:
    plugin_manager *manager;
    bool is_loaded;
    Config();
    int set_config(int id, std::stringstream &);
    int load_lib(const std::string &, std::stringstream &);

};

}
#endif
