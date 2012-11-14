#ifndef _TOPDF_H_
#define _TOPDF_H_

#include <v8.h>

using namespace v8;

struct topdf_convert_baton {
    
    uv_work_t req;
    
    bool success;
    char* source;
    char* destination;
    Persistent<Function> callback;
    
};

struct topdf_init_baton {
    
    uv_work_t req;
    
    bool success;
    Persistent<Function> callback;
    
};

bool hasInitialized = false;

#endif
