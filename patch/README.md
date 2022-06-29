MPKAlloc Patch
==============

A patch to embed MPKAlloc into the Chromium web browser. Development was done
starting from the following commit

    122ad80ac8017ce772dfa15316ce5b1beb4a2fe6

in the following repository:

    https://chromium.googlesource.com/chromium/src.git

Several features that were implemented to perform our analysis (i.e., counting
the number of MPK context switches) are given in the patch inside comments.

Until recently, it was difficult to find commodity hardware with MPKs. If
you're running Chromium + MPKAlloc on a remote server, you can run Chromium in
a headless fashion with the following from the Chromium source tree:

    ./out/Default/chrome --no-sandbox --headless --disable-gpu --disable-dev-shm-usage --screenshot https://www.google.com

