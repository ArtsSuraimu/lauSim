/*
 * @Author: C. Jonischkeit 
 * @Date: 2017-06-26 14:20:50 
 * @Last Modified by:   C. Jonischkeit
 * @Last Modified time: 2017-06-30 12:58:45 
 */
#ifndef COM_H
#define COM_H

#ifdef __cplusplus
#include <cstddef>
#endif

#ifdef __cpluplus
namespace lauSim {
extern "C" {
#endif

typedef struct {
    /**
     *  This methode should notify the backend of node
     *  that a component has failed or degraded
     *  param node the name of the node that failed
     *  param component the name of the component that failed
     *  param severity the severity of the fail (e.g. total fail or just corruptions)
     */
    int (*notify_fail) (char *node, char *component, unsigned severity);
    int (*notify_extern) (char* msg, int* len_msg);
} com;

typedef com *(*get_com_fun) ();

#ifdef __cpluplus
}
}
#endif

#endif
