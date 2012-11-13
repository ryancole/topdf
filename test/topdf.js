
var topdf = require('../lib/topdf');

// convert a single document, using default settings
topdf.convert('./test/etc/foo.xlsx', './test/etc/foo.pdf', function (err, success) {
    
    if (err || !success) {
        
        console.log(err);
        
    } else {
        
        console.log(success);
        
    }
    
});
