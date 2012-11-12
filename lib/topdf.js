
// node modules
var fs = require('fs');

// local modules
var topdf = require('../build/Release/topdf');


function convert (source, destination, options, callback) {
    
    if (typeof options == 'function') {
        
        callback = options;
        options = { };
        
    }
    
    fs.stat(source, function (err, stats) {
        
        // error on failure
        if (err)
            return callback(err);
        
        // error on non file
        if (!stats.isFile())
            return callback(new Error('source is not a file'));
        
        // initialize the conversion engine
        topdf.initialize(function (err, success) {
            
            if (err || !success)
                return callback(new Error('failed to initialized export engine'));
            
            // set default options
            setDefaultOptions();
            
            // convert the specified file to an image
            topdf.convert(source, destination, options, function (err, details) {
                
                // error on failure
                if (err)
                    return callback(new Error('failed to convert file'));
                
                // file details on success
                return callback(null, details);
                
            }); 
            
        });
        
    });
    
};

function setDefaultOptions () {
    
    topdf.set({
        
        fontsDirectory: '/usr/share/fonts/truetype/msttcorefonts'
        
    });
    
}

// module exports
exports.convert = convert;
exports.setDefaultOptions = setDefaultOptions;
