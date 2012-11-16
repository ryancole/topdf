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

void setOptions (VTHDOC documentHandle, Handle<Object> options) {
    
    if (options->Has(String::New("fontDirectory"))) {
        
        // get the option value
        String::Utf8Value value (options->Get(String::New("fontDirectory"))->ToString());
        
        // set the option
        DASetOption(documentHandle, SCCOPT_FONTDIRECTORY, *value, value.length());
        
    }
    
}

void topdf_init (uv_work_t* req) {
    
    // init the baton pointer
    topdf_init_baton* baton = (topdf_init_baton*) req->data;
    
    VTDWORD loadOptions = OI_INIT_DEFAULT | OI_INIT_NOSAVEOPTIONS | OI_INIT_NOLOADOPTIONS;
    
    // initialize data access module, which performs some disk io
    if (DAInitEx(DATHREAD_INIT_NOTHREADS, loadOptions) == DAERR_OK)
        baton->success = true;
    
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
    
    if (baton->success == true) {
        
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
    
    VTHDOC documentHandle;
    
    // open the source document
    if (DAOpenDocument(&documentHandle, IOTYPE_UNIXPATH, baton->source, 0) == SCCERR_OK) {
        
        VTHEXPORT exportHandle;
        
        // convert the source document
        if (EXOpenExport(documentHandle, FI_PDF, IOTYPE_UNIXPATH, baton->destination, 0, 0, NULL, 0, &exportHandle) == SCCERR_OK) {
            
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

void topdf_convert_end (uv_work_t* req) {
    
    HandleScope scope;
    
    // init the baton pointer
    topdf_convert_baton* baton = (topdf_convert_baton*) req->data;
    
    // init callback params
    Local<Value> argv[2];
    
    if (baton->success == true) {
        
        argv[0] = Local<Value>::New(Null());
        argv[1] = Local<Value>::New(Boolean::New(true));
        
    } else {
        
        argv[0] = Exception::Error(String::New("failed to convert file"));
        argv[1] = Local<Value>::New(Boolean::New(false));
        
    }
    
    // execute callback function
    node::MakeCallback(Context::GetCurrent()->Global(), baton->callback, 2, argv);
    
    delete baton;
    
}

Handle<Value> convert (const Arguments& args) {
    
    HandleScope scope;
    
    if (args.Length() != 3)
        ThrowException(Exception::SyntaxError(String::New("expected four parameters")));
    
    if (!args[0]->IsString() || !args[1]->IsString() || !args[2]->IsFunction())
        ThrowException(Exception::TypeError(String::New("expected str, str, func parameters")));
    
    if (hasInitialized == true) {
        
        String::Utf8Value source (args[0]->ToString());
        String::Utf8Value destination (args[1]->ToString());
        
        // instanciate data baton
        topdf_convert_baton* baton = new topdf_convert_baton;
        
        // set async work data thing
        baton->req.data = (void*) baton;
        
        // init string memory locations
        baton->source = new char[source.length() + 1];
        baton->destination = new char[destination.length() + 1];
        
        // set baton properties
        baton->success = false;;
        baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[2]));
        
        // copy in string paths
        strncpy((char*)memset(baton->source, '\0', source.length() + 1), *source, source.length());
        strncpy((char*)memset(baton->destination, '\0', destination.length() + 1), *destination, destination.length());
        
        // initiate async work on thread pool
        uv_queue_work(uv_default_loop(), &baton->req, topdf_convert, topdf_convert_end);
        
    } else {
        
        Local<Value> argv[2] = {
                
            Exception::Error(String::New("the conversion engine has not been initialized")),
            Local<Value>::New(Boolean::New(false))
            
        };
        
        node::MakeCallback(Context::GetCurrent()->Global(), Local<Function>::Cast(args[2]), 2, argv);
        
    }
    
    return scope.Close(Undefined());
    
}

Handle<Value> init (const Arguments& args) {
    
    HandleScope scope;
    
    if (args.Length() != 1)
        ThrowException(Exception::SyntaxError(String::New("expected one parameter")));
    
    if (!args[0]->IsFunction())
        ThrowException(Exception::TypeError(String::New("expected callback parameter")));
    
    if (hasInitialized == true) {
        
        Local<Value> argv[2] = {
                
            Local<Value>::New(Null()),
            Local<Value>::New(Boolean::New(true))
            
        };
        
        node::MakeCallback(Context::GetCurrent()->Global(), Local<Function>::Cast(args[0]), 2, argv);
        
    } else {
        
        // instanciate data baton
        topdf_init_baton* baton = new topdf_init_baton;
        
        // set async work data thing
        baton->req.data = (void*)baton;
        
        // initialize baton properties
        baton->success = false;
        baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));
        
        // queue work on thread pool
        uv_queue_work(uv_default_loop(), &baton->req, topdf_init, topdf_init_end);
        
    }
    
    return scope.Close(Undefined());
    
}

Handle<Value> set (const Arguments& args) {
    
    HandleScope scope;
    
    setOptions((VTHDOC)NULL, args[0]->ToObject());
    
    return scope.Close(Undefined());
    
}

void initialize (Handle<Object> target) {
    
    target->Set(String::NewSymbol("initialize"), FunctionTemplate::New(init)->GetFunction());
    target->Set(String::NewSymbol("convert"), FunctionTemplate::New(convert)->GetFunction());
    target->Set(String::NewSymbol("setOptions"), FunctionTemplate::New(set)->GetFunction());
    
}

NODE_MODULE(topdf, initialize)
