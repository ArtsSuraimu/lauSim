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


#include <unistd.h>
#include <iostream>

#include "options.h"

using namespace lauSim;
char optstr[] = "c:l:";

int Options::parse(int argc, char **argv) {
    int opt;

    while((opt = getopt(argc, argv, optstr)) > 0) {
        switch(opt) {
        case 'c':
            cfile = std::string(optarg);
            break;
        case 'l':
            loglevel = log_level_from_int((unsigned) strtoul(optarg, NULL, 0));
            break;
        default:
            std::cerr << "unrecognized option '" << (char) opt << "'" << std::endl;
            return -1;
        }
    }
    
    return 0;
}