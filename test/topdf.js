
var topdf = require('../lib/topdf');

// convert a single document, using default settings
topdf.convert('./test/etc/foo.xlsx', './test/etc/foo.pdf', function (err, details) {
    
    // log out error
    if (err) console.log(err);
    
    // log out details
    console.log(details);
    
});
