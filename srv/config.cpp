/*
 * @Author: C. Jonischkeit 
 * @Date: 2017-07-17 13:00:25
*/

#include <algorithm>
#include <iostream>
#include <fstream>
#include <iterator>
#include <sstream>
#include <cstring>
#include <vector>
#include <utility>

#include "config.h"
using namespace lauSim;

#define config_id 0x0
#define libs_id 0x1
#define none_id 0xFFFF
#define decl_id 0x100

std::vector<std::pair<std::string,int>> tokenids =
    {
        {"[library]", libs_id},
        {"[lib]", libs_id},
        {"[config]", config_id},
        {"com-actor", com_actor_id},
        {"com_actor", com_actor_id},
        {"actor", com_actor_id},
        {"com-notify", com_notify_id},
        {"com_notify", com_notify_id},
        {"notify", com_notify_id},
        {"com", com_all_id},
        {"logger", logger_id},
        {"log", logger_id},
        {"fault-manager", manager_id},
        {"fault_manager", manager_id},
        {"fault", manager_id},
        {"manager", manager_id},
    };

int get_token_identifyer(const std::string &tk) {
    for (std::pair<std::string, int> &item: tokenids) {
        if (0 == strcasecmp(tk.c_str(), item.first.c_str()))
            return item.second;
    }
    return none_id;
}

int config::set_config(int id, std::stringstream &stream, plugin_manager &manager){
    std::string value;
    plugin *plugin;

    if (!(stream >> value) || value != "="){
        std::cout << "[ERROR] '=' expected in assignment" << std::endl;
        return -1;
    }

    if (!(stream >> value)) {
        std::cout << "[ERROR] expected assignment before end of line" << std::endl;
        return -1;
    }

    if (id != tic_id)
        plugin = manager.get_by_name(value.c_str());

    if (id != tic_id && plugin == nullptr) {
        std::cerr << "[ERROR] plugin \"" << value << "\" is not loaded" << std::endl;
        return -1;
    }

    switch(id){
    case com_all_id:
    case com_actor_id:
        if (com_actor != nullptr)
            std::cout << "[WARNING] com actor is reassigned" << std::endl;
        com_actor = plugin;
        if (id == com_actor_id)
            break;
        /* fall through */
    case com_notify_id:
        if (com_notify != nullptr)
            std::cout << "[WARNING] com actor is reassigned" << std::endl;
        com_notify = plugin;
        break;
    case logger_id:
        if (logger != nullptr)
            std::cout << "[WARNING] logger is reassigned" << std::endl;
        logger = plugin;
        break;
    case manager_id:
        if (this->manager != nullptr)
            std::cout << "[WARNING] manager is reassigned" << std::endl;
        this->manager = plugin;
        break;
    case tic_id:
        if (tic_length != 0)
            std::cout << "[WARNING] tic length is reassigned" << std::endl;
        tic_length = std::stoul(value);
    }
    return 0;
}

int config::load_lib(const std::string &name, std::stringstream &args, plugin_manager &manager) {
    std::vector<std::string> argv;
    char **cargs;
    int retv;
    std::copy(std::istream_iterator<std::string>(args),
              std::istream_iterator<std::string>(),
              std::back_inserter(argv));
    
    cargs = new char*[argv.size()];
    for (unsigned int i = 0; i < argv.size(); i++) {
        cargs[i] = (char*) argv[i].c_str();
    }

    retv = manager.load_library((char *) name.c_str(), argv.size(), cargs);
    delete[] cargs;
    return retv;
}

int config::load_config(char *filename, plugin_manager &manager) {
    uint8_t state = 0;
    std::string line;
    std::ifstream conf_file;
    
    conf_file.open(filename, std::ios::in);

    if (!conf_file.is_open()) {
        std::cerr << "unable to open configuration file!" << std::endl;
        return -1;
    }

    while (std::getline(conf_file, line)) {
        std::stringstream linestr(line);
        std::string token;
        int id;

        // skip empty lines
        if (line.empty())
            continue;

        linestr >> token;
        if (token.empty() || token.at(0) == '#')
            continue;
        id = get_token_identifyer(token);

        // if it is not a declaration or none
        if ((id & decl_id) == 0) {
            // update the state
            state = (1 << id);
            continue;
        }

        // if we are in loading lib state
        if (state == (1 << libs_id)) {
            // loab the lib
            if(load_lib(line, linestr, manager))
                return -1;
            continue;
        }

        // else update the config accordingly
        if(set_config(id, linestr, manager))
            return -1;
    }
    return 0;
}