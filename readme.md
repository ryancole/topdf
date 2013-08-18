A node.js addon for Oracle's pdf export library.

## Example

```javascript
var topdf = require('topdf');

topdf.convert('./test/etc/foo.xlsx', './test/etc/foo.pdf', options, function (err, success) {
    
    /* ... */
    
});
```

## API

### convert(source, destination, [options,] callback)

Takes a `source` document and saves it to `destination` as a PDF. The `options` parameter is optional.

- `watermark`: A path to an image file to be used as the watermark image. This defaults to no watermark, if this propery is omitted.
- `fontdirectory`: A path to a directory containing true type fonts to be used. This option is *required*, if the default does not fit your setup. This defaults to `/usr/share/fonts/truetype/msttcorefonts`.
- `override`: A `boolean` specifying whether or not you wish to override the settings specified in the document. This defaults to `false`, which means any settings stored in the document to be converted will be used. This must be `true` in order to use any other formatting options.
- `gridlines`: A `boolean` specifying whether or not to print gridlines in spreadsheet documents. This `defaults` to `false`. 
- `headings`: A `boolean` specifying whether or not to print headings in spreadsheet documents. This `defaults` to `false`.
- `memory`: An `integer` specifying the maximum about of memory, in MB, that the PDF converter may use for caching the `source` file. More memory means less disk reads. Must be a value in the following collection: `4`, `16`, `64`, `256`, `1024`. This defaults to `16`.
- `pages`: An `integer` specifying the maximum number of pages to write to the `destination` file. Specifying `0` will save all pages. This `defaults` to `0`.

## Runtime Prerequisites

Make sure `/usr/local/lib/pdfexport` is in `$LD_LIBRARY_PATH`.

## Compiling Prerequisites

Download the Oracle pdf export SDK, place the header files in `/usr/local/include/pdfexport` and the *entire* contents of the sdk's `redist` directory into `/usr/local/lib/pdfexport`. Make sure `/usr/local/lib/pdfexport` is in `$LD_LIBRARY_PATH`.

## Oracle

You can download Oracle's tools at the following URL: http://www.oracle.com/technetwork/middleware/content-management/downloads/oit-dl-otn-097435.html

## .NET

I also wrote a .NET wrapper around some of the Outside In libaries, available at [ryancole/ManagedOTN](https://github.com/ryancole/ManagedOTN).
