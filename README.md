uCAN
=====

Universal API for [Controller Area Network (CAN bus or CAN)](https://en.wikipedia.org/wiki/CAN_bus) adapters.

## Project Objective

While [SocketCAN ](https://en.wikipedia.org/wiki/SocketCAN) is providing a great generic interface for using CAN adapters on Linux, no such implementation exists for Windows or macOS. This complicates the cross-platform development of CAN-based applications.

It is therefore the objective of this open-source project to provide a universal CAN driver (uCAN) that covers all three major operating systems and offers a simple and clean DLL based API.

## Supported OS' and Hardware

uCAN currently supports the following hardware:
* Windows:
  * SL CAN compatible devices (such as [Lawicel](http://www.can232.com) and [Tiny-CAN](http://www.mhs-elektronik.de/))
* macOS:
  * SL CAN
* Linux:
  * Socket CAN
  * SL CAN

## Software Dependencies

The portability of uCAN is based on the [Boost C++ Libraries](http://www.boost.org/). The build of the library is managed by the [SCons](http://scons.org/) software construction tools (in conjunction with [Python](https://www.python.org/)). Please review the [Construction.txt](Construction.txt) file for OS specific details.

## IDE

While uCAN can be built from the command-line, it is recommended that the [Eclipse IDE](https://eclipse.org) be used to edit the source code. For better viewing and formatting of the project's Python files the Eclipse plugin [PyDev](http://www.pydev.org) may be installed.

## License

uCAN is released under the LGPL license. Please see the [LICENSE.txt](LICENSE.txt) file for more information.

## Contributing

As a real software engineer will quickly discover, this code has been primarily written by a non computer scientist. Consequently, suggestions for improvements and general refactoring are more than welcome!

We naturally also very much appreciate your bug reports, bug-fixes and feature contributions. Credit will always be given in the [Contributors.txt](Contributors.txt) file.
