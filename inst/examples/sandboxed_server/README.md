
## Sandboxed Server

The example provided in this directory requires `RInside` to be built with
callback support.  This can be enabled by uncommenting one line in the file
`RInsideConfig.h` to define the preprocessor constant `RINSIDE_CALLBACKS`,
and then rebuilding.

Alternatively, adding a compiler flag `-DRINSIDE_CALLBACKS` and reinstalling
would do too; this also requires using the same flag when building the
example or similar applications.
