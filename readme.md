A node.js addon for Oracle's pdf export library.

# Usage

```javascript
var topdf = require('topdf');

// the default settings
var options = {
    
    watermark: './test/etc/test.png',
    fontdirectory: '/usr/share/fonts/truetype/msttcorefonts',
    override: false,
    gridlines: false,
    headings: false,
    
};

// convert a single document, using custom settings
topdf.convert('./test/etc/foo.xlsx', './test/etc/foo.pdf', options, function (err, success) {
    
    if (err || !success) {
        
        console.log(err);
        
    } else {
        
        console.log(success);
        
    }
    
});
```

# Options

1. watermark
    
    > A path to an image file to be used as the watermark image. This defaults to not watermark image if this propery is omitted.

2. fontdirectory
    
    > A path to a directory containing true type fonts to be used. This defaults to `/usr/share/fonts/truetype/msttcorefonts` if this property is omitted. This option is *required*, if the default does not fit your setup.

3. override
    
    > A `boolean` specifying whether or not you wish to override the settings specified in the document. This defaults to `false`, which means any settings stored in the document to be converted will be used. This must be `true` in order to use any other formatting options.

4. gridlines
    
    > A `boolean` specifying whether or not to print gridlines in spreadsheet documents. This `defaults` to `false`.

5. headings
    
    > A `boolean` specifying whether or not to print headings in spreadsheet documents. This `defaults` to `false`.

# Runtime Prerequisites

Make sure `/usr/local/lib/pdfexport` is in `$LD_LIBRARY_PATH`. Also, I currently have the font directory hard-coded to `/usr/share/fonts/truetype/msttcorefonts`. I plan to make this configurable, soon.

# Compiling Prerequisites

Download the Oracle pdf export SDK, place the header files in `/usr/local/include/pdfexport` and the *entire* contents of the sdk's `redist` directory into `/usr/local/lib/pdfexport`. Make sure `/usr/local/lib/pdfexport` is in `$LD_LIBRARY_PATH`.

# Oracle

You can download Oracle's tools at the following URL: http://www.oracle.com/technetwork/middleware/content-management/downloads/oit-dl-otn-097435.html