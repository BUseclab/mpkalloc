Memory Scanner
==============

A course grained scanner that scans the contents of an entry in `/proc/pid/maps` for values occurring within ranges provided on `stdin`. For example, to check whether the `stack` contains any addresses that fall within a specific range, run the following:

    ./scanner 4393 7ffc80779000-7ffc8079a000 [stack]
 
Next, supply the memory ranges for which the tool should scan to `stin`.

    55e8b692b000 18000
 
You can therefore scan an entire process address space by repeatedly running `scanner` on each entry given in `/proc/pid/maps`.
