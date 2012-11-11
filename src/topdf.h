#ifndef _TOPDF_H_
#define _TOPDF_H_

#include <v8.h>

using namespace v8;

struct topdf_convert_baton {
    
    bool success;
    uv_work_t req;
    String::Utf8Value* source;
    String::Utf8Value* destination;
    Persistent<Function> callback;
    
};

struct topdf_init_baton {
    
    bool success;
    uv_work_t req;
    Persistent<Function> callback;
    
};

bool hasInitialized = false;

#endif
