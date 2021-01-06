# libbuffer

This library holds utility functions for working with string buffers
in C.

The library has been extracted from libgit2 internal functions, and to
make them available for use for other programs, and is used in
production at GitHub.

## Building

To build this library use `make`, which will generate the `libbuffer.so.0` shared library.

Similarly, to install it on your system, you can use `make install`.

The tests can be run using `make test`.

## License

`libbuffer` is under GPL2 with linking exception. This means you can
link to and use the library from any program, proprietary or open
source; paid or gratis. However, if you modify libbuffer itself, you
must distribute the source to your modified version of libbuffer.

See the [COPYING file](COPYING) for the full license text.
