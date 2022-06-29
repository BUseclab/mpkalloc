Meta-Data Scanner
=================

To build, place this folder in the following directory:
 
    pin-version-gcc-linux/source/tools/

and run `make` from this folder.

Running Chromium under PIN requires allocating significant amount of memory for PIN's JIT engine. The following will allow you to run Chromium with the PIN tool:

    ../../../pin -cc_memory_size 0x40000000 -cache_block_size 0x10000000 -t ./obj-intel64/metadata.so -- /path/to/chromium/src/out/Default/chrome --no-sandbox --headless --disable-gpu https://www.google.com 
