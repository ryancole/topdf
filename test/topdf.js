
var topdf = require('../lib/topdf');

// convert a single document, using default settings
topdf.convert('/home/ryan/repos/topdf/test/etc/foo.xlsx', '/home/ryan/repos/topdf/test/etc/foo.pdf', function (err, success) {
    
    if (err || !success) {
        
        console.log(err);
        
    } else {
        
        console.log(success);
        
    }
    
});
