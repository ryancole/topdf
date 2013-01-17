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

void initializeOptions (Handle<Object> source, topdf_options* destination) {
    
    HandleScope scope;
    
    // font directory
    if (source->Has(String::New("fontdirectory"))) {
        
        // get the provided path
        String::Utf8Value fontdirectory (source->Get(String::New("fontdirectory"))->ToString());
        
        // init destination string size
        destination->fontdirectory = new char[fontdirectory.length() + 1];
        
        // copy in provded path
        strncpy((char*)memset(destination->fontdirectory, '\0', sizeof(char) * (fontdirectory.length() + 1)), *fontdirectory, fontdirectory.length());
        
    } else {
        
        // init default path
        char fontdirectory[] = "/usr/share/fonts/truetype/msttcorefonts";
        
        // init destination string size
        destination->fontdirectory = new char[strlen(fontdirectory) + 1];
        
        // copy in default path
        strncpy((char*)memset(destination->fontdirectory, '\0', sizeof(char) * (strlen(fontdirectory) + 1)), fontdirectory, strlen(fontdirectory));
        
    }
    
    // watermark
    if (source->Has(String::New("watermark"))) {
        
        // get the provided path
        String::Utf8Value watermark (source->Get(String::New("watermark"))->ToString());
        
        // init destination string size
        destination->watermark = new char[watermark.length() + 1];
        
        // copy in provided path
        strncpy((char*)memset(destination->watermark, '\0', watermark.length() + 1), *watermark, watermark.length());
        
    } else {
        
        // init destination buffer
        destination->watermark = new char[1];
        
        // null out buffer
        memset(destination->watermark, '\0', 1);
        
    }
    
    // gridlines
    if (source->Has(String::New("gridlines"))) {
        
        // get the provided setting
        Local<Boolean> gridlines = source->Get(String::New("gridlines"))->ToBoolean();
        
        // set the baton value
        destination->gridlines = gridlines->Value() ? 1 : 0;
        
    } else {
        
        destination->gridlines = 0;
        
    }
    
    // headings
    if (source->Has(String::New("headings"))) {
        
        // get the provided setting
        Local<Boolean> headings = source->Get(String::New("headings"))->ToBoolean();
        
        // set the baton value
        destination->headings = headings->Value() ? 1 : 0;
        
    } else {
        
        destination->headings = 0;
        
    }
    
    // use options specified in document
    if (source->Has(String::New("override"))) {
        
        // get the provided setting
        Local<Boolean> override = source->Get(String::New("override"))->ToBoolean();
        
        // set the baton value
        destination->override = override->Value() ? 0 : 1;
        
    } else {
        
        // default is to use doc page settings
        destination->override = 1;
        
    }
    
    // memory
    if (source->Has(String::New("memory"))) {
        
        destination->memory = source->Get(String::New("memory"))->Int32Value();
        
    } else {
        
        destination->memory = 16;
        
    }
    
    // pages
    if (source->Has(String::New("pages"))) {
        
        destination->pages = source->Get(String::New("pages"))->Int32Value();
        
    } else {
        
        destination->pages = 0;
        
    }
    
}

void setOptions (VTHDOC documentHandle, topdf_options* options) {
    
    // set font directory
    DASetOption(documentHandle, SCCOPT_FONTDIRECTORY, options->fontdirectory, strlen(options->fontdirectory));
    
    // set override
    DASetOption(documentHandle, SCCOPT_USEDOCPAGESETTINGS, &options->override, sizeof(VTBOOL));
    
    // set watermark settings
    if (strlen(options->watermark) > 0) {
        
        WATERMARKIO watermarkSettings;
        WATERMARKPATH watermarkPathSettings;
        
        // set path settings
        watermarkPathSettings.dwMaxSize = SCCUT_FILENAMEMAX;
        strncpy((char*)memset(watermarkPathSettings.szWaterMarkPath, '\0', SCCUT_FILENAMEMAX), options->watermark, strlen(options->watermark));
        
        // set watermark settings
        watermarkSettings.dwType = IOTYPE_UNIXPATH;
        watermarkSettings.Path = watermarkPathSettings;
        
        VTBOOL enabled = 1;
        
        DASetOption(documentHandle, SCCOPT_ENABLEWATERMARK, &enabled, sizeof(VTBOOL));
        DASetOption(documentHandle, SCCOPT_WATERMARKIO, &watermarkSettings, sizeof(WATERMARKIO));
        
    }
    
    // set gridlines
    DASetOption(documentHandle, SCCOPT_DBPRINTGRIDLINES, &options->gridlines, sizeof(VTBOOL));
    DASetOption(documentHandle, SCCOPT_SSPRINTGRIDLINES, &options->gridlines, sizeof(VTBOOL));
    
    // set headings settings
    DASetOption(documentHandle, SCCOPT_DBPRINTHEADINGS, &options->headings, sizeof(VTBOOL));
    DASetOption(documentHandle, SCCOPT_SSPRINTHEADINGS, &options->headings, sizeof(VTBOOL));
    
    // default memory usage setting
    VTDWORD memory = SCCDOCUMENTMEMORYMODE_SMALL;
    
    // optional memory usage settings
    if (options->memory == 4) {
        
        memory = SCCDOCUMENTMEMORYMODE_SMALLEST;
    
    } else if (options->memory == 64) {
        
        memory = SCCDOCUMENTMEMORYMODE_MEDIUM;
    
    } else if (options->memory == 256) {
        
        memory = SCCDOCUMENTMEMORYMODE_LARGE;
        
    } else if (options->memory == 1024) {
        
        memory = SCCDOCUMENTMEMORYMODE_LARGEST;
        
    }
    
    // set memory usage setting
    DASetOption(documentHandle, SCCOPT_DOCUMENTMEMORYMODE, &memory, sizeof(VTDWORD));
    
    // set page count settings
    if (options->pages > 0) {
        
        VTDWORD start = 1;
        VTDWORD what = SCCVW_PRINT_PAGERANGE;
        
        DASetOption(documentHandle, SCCOPT_WHATTOPRINT, &what, sizeof(VTDWORD));
        DASetOption(documentHandle, SCCOPT_PRINTSTARTPAGE, &start, sizeof(VTDWORD));
        DASetOption(documentHandle, SCCOPT_PRINTENDPAGE, &options->pages, sizeof(VTDWORD));
        
    }
    
}

void topdf_convert (uv_work_t* req) {
    
    // init the baton pointer
    topdf_convert_baton* baton = (topdf_convert_baton*) req->data;
    
    VTDWORD loadOptions = OI_INIT_DEFAULT | OI_INIT_NOSAVEOPTIONS | OI_INIT_NOLOADOPTIONS;

    // initialize data access module, which performs some disk io
    baton->error = DAInitEx(DATHREAD_INIT_PTHREADS, loadOptions);

    if (baton->error == DAERR_OK) {
        
        VTHDOC documentHandle;
        
        // make note of the current convert step
        baton->step = 1;

        // open the source document
        baton->error = DAOpenDocument(&documentHandle, IOTYPE_UNIXPATH, baton->source, 0);

        if (baton->error == SCCERR_OK) {
            
            VTHEXPORT exportHandle;
            
            // set options for the documents
            setOptions(documentHandle, baton->options);
            
            // make note of the current convert step
            baton->step = 2;

            // convert the source document
            baton->error = EXOpenExport(documentHandle, FI_PDF, IOTYPE_UNIXPATH, baton->destination, 0, 0, NULL, 0, &exportHandle);

            if (baton->error == SCCERR_OK) {

            	// make note of the current convert step
            	baton->step = 3;

            	// export the converted file to disk
            	baton->error = EXRunExport(exportHandle);

                if (baton->error == SCCERR_OK)
                    baton->success = true;
                
                // close the export engine
                EXCloseExport(exportHandle);
                
            }
            
            // close the opened document
            DACloseDocument(documentHandle);
            
        }
        
        DADeInit();
        
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
        
    	Local<Object> error = Object::New();

    	error->Set(String::NewSymbol("step"), Number::New(baton->step));
    	error->Set(String::NewSymbol("error"), Number::New(baton->error));

        argv[0] = Local<Value>::New(error);
        argv[1] = Local<Value>::New(Boolean::New(false));
        
    }
    
    // execute callback function
    node::MakeCallback(Context::GetCurrent()->Global(), baton->callback, 2, argv);
    
    delete baton;
    
}

Handle<Value> convert (const Arguments& args) {
    
    HandleScope scope;
    
    if (args.Length() != 4)
        ThrowException(Exception::SyntaxError(String::New("expected four parameters")));
    
    if (!args[0]->IsString() || !args[1]->IsString() || !args[2]->IsObject() || !args[3]->IsFunction())
        ThrowException(Exception::TypeError(String::New("expected str, str, obj, func parameters")));
    
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
    baton->step = 0;
    baton->error = 0;
    baton->success = false;
    baton->options = new topdf_options;
    baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[3]));
    
    // copy in string paths
    strncpy((char*)memset(baton->source, '\0', source.length() + 1), *source, source.length());
    strncpy((char*)memset(baton->destination, '\0', destination.length() + 1), *destination, destination.length());
    
    // set the desired options
    initializeOptions(args[2]->ToObject(), baton->options);
    
    // initiate async work on thread pool
    uv_queue_work(uv_default_loop(), &baton->req, topdf_convert, (uv_after_work_cb)topdf_convert_end);
    
    return scope.Close(Undefined());
    
}

void initialize (Handle<Object> target) {
    
    target->Set(String::NewSymbol("convert"), FunctionTemplate::New(convert)->GetFunction());
    
}

NODE_MODULE(topdf, initialize)
