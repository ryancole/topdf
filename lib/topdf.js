
// node modules
var fs = require('fs');

// local modules
var topdf = require('../build/Release/topdf');


function setOptions (options) {
    
    return topdf.setOptions(options);
    
}

function convert (source, destination, callback) {
    
    fs.stat(source, function (err, stats) {
        
        // error on failure
        if (err) return callback(err);
        
        // error on non file
        if (!stats.isFile())
            return callback(new Error('source is not a file'));
        
        // convert the specified file to an image
        topdf.convert(source, destination, function (err, success) {
            
            // error on failure
            if (err || !success)
                return callback(new Error('failed to convert file'));
            
            fs.exists(destination, function (exists) {
                
                if (!exists)
                    return callback(new Error('failed to save file to destination'));
                
                // success
                return callback(null, true);
                
            });
            
        });
        
    });
    
};

// module exports
exports.convert = convert;
exports.setOptions = setOptions;
