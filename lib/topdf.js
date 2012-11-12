
// node modules
var fs = require('fs');

// local modules
var topdf = require('../build/Release/topdf');


function setOptions (options) {
    
    return topdf.setOptions(options);
    
}

function initialize (callback) {
    
    // initialize the conversion engine
    topdf.initialize(function (err, success) {
        
        if (err || !success)
            return callback(new Error('failed to initialize the conversion engine'));
        
        // set some default options
        setOptions({
            
            fontDirectory: '/usr/share/fonts/truetype/msttcorefonts'
            
        });
        
        // return the success variable
        return callback(null, success);
        
    });
    
}

function convert (source, destination, options, callback) {
    
    if (typeof options == 'function') {
        
        callback = options;
        options = { };
        
    }
    
    fs.stat(source, function (err, stats) {
        
        // error on failure
        if (err) return callback(err);
        
        // error on non file
        if (!stats.isFile())
            return callback(new Error('source is not a file'));
        
        // initialize the conversion engine
        initialize(function (err, success) {
            
            // error on failure
            if (err) return callback(err);
            
            console.log('about to convert');
            
            // convert the specified file to an image
            topdf.convert(source, destination, options, function (err, details) {
                
                console.log('convert returned');
                
                // error on failure
                if (err)
                    return callback(new Error('failed to convert file'));
                
                // file details on success
                return callback(null, details);
                
            }); 
            
        });
        
    });
    
};

// module exports
exports.convert = convert;
exports.setOptions = setOptions;
exports.initialize = initialize;
