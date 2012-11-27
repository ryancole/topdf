
var topdf = require('../lib/topdf');

// set some options
var options = {
    
    override: true,
    gridlines: true,
    headings: true
    
};

// convert a single document, using default settings
topdf.convert('./test/etc/foo.xlsx', './test/etc/foo.pdf', options, function (err, success) {
    
    if (err || !success) {
        
        console.log(err);
        
    } else {
        
        console.log(success);
        
    }
    
});
