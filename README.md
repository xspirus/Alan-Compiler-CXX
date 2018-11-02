# Alan Compiler
Developed for the class "Compilers" of the
National Technical University of Athens for
the year 2018.

## Specifications
* Language : [Alan](http://courses.softlab.ntua.gr/compilers/2018a/alan2018.pdf)
* Implementation : C++17
* Build Tool : [CMake][cmake]

## Versions
* [Clang][clang] (6.0.0)
* Flex (2.5.4)
* Bison (3.0.4)
* [CMake][cmake] (3.10.2)
* [LLVM][llvm] (6.0.0)

## Build
```bash
make alan-release
```

## Project Structure
* main.cpp
* src
  * ast  
    Abstract Syntax Tree
  * lexer  
    Lexical Analysis
  * parser  
    Syntax Analysis
  * symbol  
    Semantic Analysis
  * codegen  
    Codegen essentials
  * general  
    General functions
  * message  
    Errors, warnings, fatals, internals, debug
  * fix  
    Convert escaped chars to normal chars
  * lib  
    Define alan standard library functions

[cmake]: https://cmake.org/
[clang]: https://clang.llvm.org/
[llvm]: https://llvm.org/
