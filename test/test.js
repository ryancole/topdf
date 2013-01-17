
var topdf = require('../lib/topdf'),
    assert = require('assert');

// set some options
var options = {
    
    headings: true,
    override: true,
    gridlines: true,
    memory: 1024,
    pages: 10
    
};

// convert a single document, using default settings
topdf.convert('./test/etc/foo.xlsx', './test/etc/foo.pdf', options, function (err, success) {
    
    if (err) {
        
        console.log(err);
        
        // no errors
        assert.ifError(err);
        
    }
    
    // successful
    assert.ok(success);
    
});
