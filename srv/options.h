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
 * This class parses and stores options passed to the program via command line.
 */
class Options {
public:
    Options(): cfile(""), loglevel(LL_Warning), sync(false), max_tics(0) {}

    /**
     * This function parses command line options (usually the ones supplyied to main)
     * @param argc The length of the argument vector
     * @param argv The argument vector
     */
    int parse(int argc, char ** argv);

    /**
     * Holds the name of the configuration file that should be used.
     */
    std::string cfile;

    /**
     * Holds the minimum log level. Any message with a log level at least as high
     * shall be logged and any message with a lower level shall be discarded.
     */
    log_level loglevel;

    /**
     * Indicates weather to wait for all nodes to become ready. This sould be set for normal operation
     * but unset if you want to record a run without interaction with actual actors.
     * Value | Semantic
     * ---:|:---
     * true (!= 0)  | wait until all nodes have reported that they are ready
     * false (== 0) | do not wait for any ready messages
     */
    bool sync;
    /**
     * This denotes an upper limit for the number of server tics for this run.
     * Once the tic counter exceeds max_tics the server will shutdown. This is especially
     * usefull when a run is recorded and the server does not wait between tics.
     * Setting this value to 0 is equivalent to not setting an upper limit.
     */
    unsigned max_tics;
};

}

#endif