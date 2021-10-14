Target Utilities
===========================
The <target-utils> package contains sources for common utilities and data
structures to be used by target driver runtime software. TDI (Table Driven
Interface) uses some of the utils and so do some target specific driver
software layers.

The <target-utils> package is organized as follows:
    include/
        Header files needed to make use of the various utilities.

    src/
        Source files implementing the various utilities.

    third-party/
        Files of open-source third-party libraries used to implement various
        utilities.

Building and Installing
=======================
```
target-utils/$$ cmake -B <Build directory> -DCMAKE_INSTALL_PREFIX=<Install directory>
target-utils/$$ cd <Build directory> && make install -j$nproc
```

Artifacts Installed
===================
Header files are installed to: $CMAKE_INSTALL_PREFIX/include/

libtargetutils.[a,la,so] are installed to $CMAKE_INSTALL_PREFIX/lib/
