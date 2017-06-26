/*
 * @Author: D. Yang
 * @Date: 2017-06-26 14:20:29 
 * @Last Modified by:   D. Yang 
 * @Last Modified time: 2017-06-26 14:20:29 
 */
#ifndef TYPES_H
#define TYPES_H

#ifdef __cplusplus
namespace lauSim{
extern "C"{
#endif  //__cplusplus

typedef struct tag_com com;

typedef int (*init) (int argc, char *argv[]);
typedef int (*cleanup) ();
typedef com *(*get_com) ();
typedef failure_manager *(*get_failure_manager) ();

typedef unsigned char rate_t;
typedef unsigned char percentage_t;


struct tag_com{
	int version;
    /**
     *  This methode should notify the backend of node
     *  that a component has failed or degraded
     *  param node the name of the node that failed
     *  param component the name of the component that failed
     *  param severity the severity of the fail (e.g. total fail or just corruptions)
     */
    int (*notify_fail) (char *node, char *component, char *notice);
    int (*notify_extern) (char* msg, int* len_msg);
    int (*isr_injection) (...);  //TODO: Understand This
};

struct tag_math{
    int version; 
    
    int (*rand_n) ();
    rate_t (*calc_rate) (rate_t* rates);  //TODO: Understand this
}

typedef enum tag_loglevel{
    LL_None = 0,
    LL_Debug, 
    LL_Info,
    LL_Warning,
    LL_Error,
    LL_Fatal
}LogLevel;

#ifndef __cplusplus
typedef unsigned char bool;
#endif //__cplusplus

#ifdef __cplusplus
}
}
#endif  //__cplusplus

#endif  //TYPES_H