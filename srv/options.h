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


#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>
#include <lausim/logger.h>
#include "stdoutLog.h"

namespace lauSim {

/**
 * Handles options passed to the program
 */
class Options {
public:
    Options(): cfile(""), loglevel(LL_Warning), sync(false), max_tics(0) {}

    /**
     * parse command line options (usually the ones supplyied to main)
     * @param argc The length of the argument vector
     * @param argv The argument vector
     */
    int parse(int argc, char ** argv);

    /**
     * the config file used
     */
    std::string cfile;
    /**
     * minimum log level
     */
    log_level loglevel;
    /**
     * waiting for all nodes to be ready
     * true: wait
     * false: no wait
     */
    bool sync;
    /**
     * automatically shut down the server after some number of tics
     * 0 means unlimited tic count
     */
    unsigned max_tics;
};

}

#endif