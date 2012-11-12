#include <node.h>
#include <v8.h>

#ifndef UNIX
    #define UNIX
    #define UNIX_64
#endif

#include <scctype.h>
#include <sccex.h>
#include "topdf.h"

using namespace v8;

void setOptions (VTHDOC documentHandle, Local<Object> options) {
    
    // BALLMER PEAK ACTIVE
    VTHDOC handle = documentHandle == NULL ? NULL : documentHandle;
    
    if (options->Has(String::New("fontsDirectory"))) {
        
        // get the font directory path
        String::Utf8Value fontsDirectory (options->Get(String::New("fontsDirectory"))->ToString());
        
        // set the option
        DASetOption(handle, SCCOPT_FONTDIRECTORY, *fontsDirectory, fontsDirectory.length() + 1);
        
    }
    
}

void topdf_init (uv_work_t* req) {
    
    // init the baton pointer
    topdf_init_baton* baton = (topdf_init_baton*) req->data;
    
    if (hasInitialized != true) {
        
        VTDWORD loadOptions = OI_INIT_DEFAULT | OI_INIT_NOSAVEOPTIONS | OI_INIT_NOLOADOPTIONS;
        
        // initialize data access module, which performs some disk io
        if (DAInitEx(DATHREAD_INIT_NOTHREADS, loadOptions) == DAERR_OK)
            baton->success = true;
    
    } else {
        
        // already has been loaded so set true again
        baton->success = true;
        
    }
    
}

void topdf_init_end (uv_work_t* req) {
    
    HandleScope scope;
    
    // init the baton pointer
    topdf_init_baton* baton = (topdf_init_baton*) req->data;
    
    // set global variable making note of a successful load
    if (baton->success == true)
        hasInitialized = true;
    
    // init callback params
    Local<Value> argv[2];
    
    if (hasInitialized == true) {
        
        argv[0] = Local<Value>::New(Null());
        argv[1] = Local<Value>::New(Boolean::New(true));
        
    } else {
        
        argv[0] = Exception::Error(String::New("failed to initialize export engine"));
        argv[1] = Local<Value>::New(Boolean::New(false));
        
    }
    
    // execute callback function
    node::MakeCallback(Context::GetCurrent()->Global(), baton->callback, 2, argv);
    
    delete baton;
    
}

void topdf_convert (uv_work_t* req) {
    
    // init the baton pointer
    topdf_convert_baton* baton = (topdf_convert_baton*) req->data;
    
    // only continue if initialized
    if (hasInitialized == true) {
        
        VTHDOC documentHandle;
        
        // open the source document
        if (DAOpenDocument(&documentHandle, IOTYPE_UNIXPATH, **(baton->source), 0) == SCCERR_OK) {
            
            VTHEXPORT exportHandle;
            
            // convert the source document
            if (EXOpenExport(documentHandle, FI_PDF, IOTYPE_UNIXPATH, **(baton->destination), 0, 0, NULL, 0, &exportHandle) == SCCERR_OK) {
                
                if (EXRunExport(exportHandle) == SCCERR_OK) {
                    
                    baton->success = true;
                    
                }
                
                // close the export engine
                EXCloseExport(exportHandle);
                
            }
            
            // close the opened document
            DACloseDocument(documentHandle);
            
        }
        
    }
    
}

void topdf_convert_end (uv_work_t* req) {
    
    HandleScope scope;
    
    // init the baton pointer
    topdf_convert_baton* baton = (topdf_convert_baton*) req->data;
    
    // init callback params
    Local<Value> argv[2];
    
    if (baton->success == true) {
        
        // create details result object
        Local<Object> details = Object::New();
        
        // set detail properties
        details->Set(String::NewSymbol("path"), String::New("/foo/bar/baz"));
        
        // set callback parameters
        argv[0] = Local<Value>::New(Null());
        argv[1] = Local<Value>::New(details);
        
    } else {
        
        argv[0] = Exception::Error(String::New("failed to convert file"));
        argv[1] = Local<Value>::New(Null());
        
    }
    
    // execute callback function
    node::MakeCallback(Context::GetCurrent()->Global(), baton->callback, 2, argv);
    
    delete baton;
    
}

Handle<Value> convert (const Arguments& args) {
    
    HandleScope scope;
    
    // instanciate data baton
    topdf_convert_baton* baton = new topdf_convert_baton;
    
    // initialize baton properties
    baton->success = false;
    baton->req.data = (void*) baton;
    baton->source = new String::Utf8Value(args[0]->ToString());
    baton->destination = new String::Utf8Value(args[1]->ToString());
    baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[2]));
    
    // initiate async work on thread pool
    uv_queue_work(uv_default_loop(), &baton->req, topdf_convert, topdf_convert_end);
    
    return scope.Close(Undefined());
    
}

Handle<Value> init (const Arguments& args) {
    
    HandleScope scope;
    
    // instanciate data baton
    topdf_init_baton* baton = new topdf_init_baton;
    
    // initialize baton properties
    baton->success = false;
    baton->req.data = (void*)baton;
    baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));
    
    // queue work on thread pool
    uv_queue_work(uv_default_loop(), &baton->req, topdf_init, topdf_init_end);
    
    return scope.Close(Undefined());
    
}

Handle<Value> setGlobalOptions (const Arguments& args) {
    
    HandleScope scope;
    
    // get the provided options object
    Local<Object> options = Local<Object>::Cast(args[0]);
    
    // set the given options with no document handle
    setOptions(NULL, options);
    
    return scope.Close(Undefined());
    
}

void initialize (Handle<Object> target) {
    
    target->Set(String::NewSymbol("initialize"), FunctionTemplate::New(init)->GetFunction());
    target->Set(String::NewSymbol("convert"), FunctionTemplate::New(convert)->GetFunction());
    target->Set(String::NewSymbol("set"), FunctionTemplate::New(setGlobalOptions)->GetFunction());
    
}

NODE_MODULE(topdf, initialize)
