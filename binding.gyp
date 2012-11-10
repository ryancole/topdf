{
    "targets": [
        {
            "target_name": "topdf",
            "sources": ["src/topdf.cc"],
            "include_dirs": ["/usr/local/include/pdfexport"],
            "link_settings": {
                "libraries": ["-lsc_ex", "-lsc_da"],
                "ldflags": ["-L/usr/local/lib/pdfexport"]
            }
        }
    ]
}