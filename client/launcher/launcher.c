#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>

#include <backend_if.h>

/**
 * the methode used for injecting faults
 */
enum lau_methode {
    MTH_PRELOAD,
    MTH_PRIVILIGED
};

/**
 * the configuration that is used
 */
struct lau_opts {
    char *name;
    char *server;
    char *port;
    enum lau_methode methode;
    char *pre_lib;
    char *command;
};

/**
 * meaningfull default options
 */
struct lau_opts options = {
    .name = NULL,
    .server = "localhost",
    .port = "1883",
    .methode = MTH_PRELOAD,
    .pre_lib = NULL,
    .command = NULL
};

/**
 * set to one if a sigint is cought
 */
volatile int req_stop = 0;

void handler(int signum) {
    req_stop = 1;
}

struct sigaction sigact;


void print_help() {
    puts("laucher [OPTIONS]");
    puts("\t-c command   : command to be executed                  (required)");
    puts("\t-n node_name : the name of the node                    (required)");
    puts("\t-s server    : the mqtt server to connect to           (default: localhost)");
    puts("\t-p port      : the port number of the server           (default: 1883)");
    puts("\t-m methode   : methode is either prelaod or privileged (default: preload)");
    puts("\t-l path      : path to the preload library to use      (required with preload methode)");
    puts("\t-h           : prints this help                        (duh)");
}

int main(int argc, char **argv) {
    char *tmp;
    char **args;
    char *preload_str;
    int optchr, stat_val;
    struct stat statbuf;
    pid_t pid, pid2;

    while ((optchr = getopt(argc, argv, "n:s:p:m:l:hc:")) != -1) {
        switch (optchr) {
        case 'n':
            options.name = optarg;
            break;
        case 's':
            options.server = optarg;
            break;
        case 'l':
            options.pre_lib = optarg;
            break;
        case 'h':
            print_help();
            return EXIT_SUCCESS;
        case 'p':
            options.port = optarg;
            break;
        case 'm':
            if (strcasecmp("preload", optarg) == 0)
                options.methode = MTH_PRELOAD;
            else if (strcasecmp("privileged", optarg) == 0)
                options.methode = MTH_PRIVILIGED;
            else {
                puts("only the modes preload and privileged are supported!");
                return EXIT_FAILURE;
            }
            break;
        case 'c':
            options.command = optarg;
            break;
        default:
            printf("option %c not recognized\n", optchr);
            break;
        }
    }

    if (options.command == NULL) {
        puts("a command is required");
        return EXIT_FAILURE;
    }
    
    sigact.sa_sigaction = NULL;
    sigact.sa_handler = handler;
    sigact.sa_flags = 0;
    sigact.sa_restorer = NULL;
    sigemptyset(&sigact.sa_mask);
    sigaction(SIGTERM, &sigact, NULL);
    sigaction(SIGINT, &sigact, NULL);

    if (options.methode == MTH_PRIVILIGED) {
        puts("not supported yet");
    } else if (options.methode == MTH_PRELOAD) {
        if (options.pre_lib == NULL) {
            puts("path to preload library is needed");
            return EXIT_FAILURE;
        }
        pid = fork();
        if (pid == 0) {
            tmp = getenv("LD_PRELOAD");
            if (tmp != NULL) {
                preload_str = malloc(strlen(options.pre_lib) + strlen(tmp) + 0x80);
                if (preload_str == NULL)
                    return EXIT_FAILURE;
                sprintf("%s %s", options.pre_lib, tmp);
            } else
                preload_str = options.pre_lib;
            setenv("LD_PRELOAD", preload_str, 1);
            exit(system(options.command));
        } else {
            while (stat(FIFO_FILE, &statbuf) != 0) {}
            pid2 = fork();
            if (pid2 != 0) {
                while(waitpid(pid2, &stat_val, 0) == -1 && !req_stop) {}
                if (!WIFEXITED(stat_val) || WEXITSTATUS(stat_val))
                    req_stop = 1;
                if (req_stop) {
                    kill(pid2, SIGTERM);
                    waitpid(pid2, &stat_val, 0);
                }
                kill(pid, SIGTERM);
            } else {
                args = calloc(5, sizeof(char*));
                args[0] = "./client";
                args[1] = options.name;
                args[2] = options.server;
                args[3] = options.port;
                args[4] = NULL;
                execv("./client", args);
                // execv returns only in case of an error -> clean up
                // terminating pid2, will also terminate the other childs
                kill(pid2, SIGTERM);
                return EXIT_FAILURE;
            }
        }
    }
}