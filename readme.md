A node.js addon for Oracle's pdf export library.

# Usage

```javascript
var topdf = require('topdf');

topdf.convert('./test/etc/foo.xlsx', './test/etc/foo.pdf', function (err, success) {
    
    if (err || !success) {
        
        console.log(err);
        
    } else {
        
        console.log(success);
        
    }
    
});
```

# Compiling Prerequisites

Download the Oracle pdf export SDK, place the header files in `/usr/local/include/pdfexport` and the *entire* contents of the sdk's `redist` directory into `/usr/local/lib/pdfexport`. Make sure `/usr/local/lib/pdfexport` is in `$LD_LIBRARY_PATH`.

# Oracle

You can download Oracle's tools at the following URL: http://www.oracle.com/technetwork/middleware/content-management/downloads/oit-dl-otn-097435.html