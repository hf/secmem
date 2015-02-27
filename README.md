# SecMem

This is a small, minimal, header-only C++ library that creates a secure memory
mapping in the virutal memory subsystem.

Secure memory implies:

+ it will not be shared among processes
+ will not be persisted to swap, i.e. always locked in RAM
+ it is securely clearable with zeros or another value
+ it allows specific protection, i.e. generates SEGFAULT on disallowed access

This library uses reference counting to manage the memory mapping.

It is useful for managing memory in cryptographic applications.

## License

Copyright &copy; 2015 Stojan Dimitrovski

All code herein (except where otherwise noted) is distributed under
[The MIT License](http://opensource.org/licenses/MIT).

See the included file `LICENSE.txt` for the full text.
