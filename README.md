MPKAlloc
========

Using memory protection keys (MPKs) available on Intel CPUs to protect meta-data in memory allocators.

To cite our [paper][1], please use the following:
 
    @inproceedings{MPKAlloc,
        title={MPKAlloc: Efficient Heap Meta-Data Integrity Through Hardware Memory Protection Keys},
        author={Blair, William and Robertson, William and Egele Manuel},
        booktitle={Conference on Detection of Intrusions and Malware, and Vulnerability Assessment},
        year={2022}
    }

# Installation 

To start, first set up a Chromium build environment using the following
instructions. MPKAlloc has only been tested on Linux, but given support from
the operating system the approach may work on other platforms as well.

    https://chromium.googlesource.com/chromium/src/+/main/docs/linux/build_instructions.md

The patch for embedding MPKAlloc into the Chromium source tree can be found in the `patch` directory. Alternatively, `tcmalloc` provides the files necessary to embed MPKAlloc within the standalone `tcmalloc` allocator found in `gperftools` version 2.7.
 
# Project Layout

MPKAlloc consists of the following:

   - tcmalloc
   Files that embed MPKAlloc into tcmalloc found in gperftools version 2.7. 
   - patch 
   A patch to the Chromium source tree that embeds MPKAlloc into the tcmalloc
   and PartitionAlloc allocators. There is also support for counting the frequency
   of context switches which may be enabled by removing comments.
   - tools 
   Scanning tools that detect meta-data pointers within the Chromium process
   and during execution.
   - example.png 
   A fun website rendered with Chromium + MPKAlloc.


[1]: https://link.springer.com/chapter/10.1007/978-3-031-09484-2_8
