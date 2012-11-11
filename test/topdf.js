
var topdf = require('../lib/topdf');

topdf.convert('./test/foo.xlsx', './test/foo.pdf', function (err, details) {
    
    // log out error
    if (err) console.log(err);
    
    // log out details
    console.log(details);
    
});
