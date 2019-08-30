# MList
#### A C++17 TMP Program Library

## Introduction
Long ago, I wrote a TMP program about type lists.
Back then, I was still using C++98, so I have to wrote
the lists in Haskell `cons` format, meaning:
    
    list<a, b, c, d> -> cons<a, cons<b, cons<c, cons<d, null_t>>>>
Many years passed, and varadic template have been added
to C++11, so I decided to rewrote it.

## An algorithm-based design
Nearly 3/4 of `list.hpp` header is about algorithm on lists,
including `select`, `map`, `fold`, `find` and many more.
Go Straight and
#CODING!

## File Structure
`CMakeLists.txt`: a simple CMake configuation

`main.cpp`: a Hello World program, just to test compile issues
as this is a template metaprogram.

`list.hpp`: the main header

### Class Structure
`list`: a simple type list, with very few members

`list_c`: simple valued list (in compile-time). All the algorithms
are wrote for both wrote for two lists.

`wrapper_t`: just a wrapper for compile-time values so they
can be passed to metafunctions

`range_c`: A range of `T`s, you can specify the step.

The latter two have `using` declarations for common type such as `int` and `bool`

### Algorithms
Most algorithms accept both `list` and `list_c`, but sometimes
you may have to wrap values in `wrapper_t` to pass them(such as `find` on `list_c`)

## Compile
None, just use the header
