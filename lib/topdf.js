
// node modules
var fs = require('fs');

// local modules
var topdf = require('../build/Release/topdf');


function convert (source, destination, options, callback) {
    
    if (typeof options == 'function') {
        
        // shift the callback back
        callback = options;
        
        // default options
        options = { };
        
    }
    
    fs.stat(source, function (err, stats) {
        
        // error on failure
        if (err) return callback(err);
        
        // error on non file
        if (!stats.isFile())
            return callback(new Error('source is not a file'));
        
        // convert the specified file to an image
        topdf.convert(source, destination, options, function (err, success) {
            
            // error on failure
            if (err) return callback(err);
            
            // error on failure
            if (!success) return callback(new Error('failed to convert file for an unknown reason'));
            
            fs.exists(destination, function (exists) {
                
                if (!exists)
                    return callback(new Error('convert operation did not result in a file'));
                
                // success
                return callback(null, true);
                
            });
            
        });
        
    });
    
};

// module exports
exports.convert = convert;
