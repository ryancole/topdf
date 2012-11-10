#include <node.h>
#include <v8.h>

#ifndef UNIX
    #define UNIX
    #define UNIX_64
#endif

#include <sccex.h>

using namespace v8;

Handle<Value> convert (const Arguments& args) {
    
    HandleScope scope;
    
    return scope.Close(Undefined());
    
}

void initialize (Handle<Object> target) {
    
    // initialize data access module
    if (DAInitEx(SCCOPT_INIT_NOTHREADS, (OI_INIT_NOSAVEOPTIONS | OI_INIT_NOLOADOPTIONS)) != SCCERR_OK)
        ThrowException(Exception::TypeError(String::New("failed to initialize pdf export engine")));
    
    // export convert function
    target->Set(String::NewSymbol("convert"), FunctionTemplate::New(convert)->GetFunction());
    
}

NODE_MODULE(topdf, initialize)
