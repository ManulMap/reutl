# Reverse engineering utilities library

Aimed only for Amd64 Windows.

Library requires support for c++23, so you can use:

* Visual Studio 17.6 Preview 6 with /std:c++latest
* build the latest Microsoft STL from sources and specify it for msvc
* clang/clang-cl 17.0.0 with Microsoft STL
* probably should work with mingw 13

## Features

* memory signature scanner
* safe vmt hooks (replaces the object's vptr with our vmt)
* hard vmt hooks (replaces original vmt cells)
* memory address class with convenient methods

## Todo

* implement wrapper for find_pattern that search in specific process module
  (module should be got from peb not from GetModuleHandle)
* implement benchmarks for signature scanner
* change scanner searching method from default std searcher to
  std::boyer_moore_searcher or std::boyer_moore_horspool_searcher

## Usage

See the test directory for examples.