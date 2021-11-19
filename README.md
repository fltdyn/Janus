# Janus

Janus is a Dynamic Aerospace Vehicle Exchange Mark-up Language (DAVE-ML) C++ Interpreter. 

This repository contains the Janus library, a supporting library called Ute, and example use cases.

## Resources ##

- DST Group Janus website: https://www.dst.defence.gov.au/opportunity/janus-dynamic-aerospace-vehicle-exchange-mark-language-dave-ml-c-interpreter

- DAVE-ML website: http://daveml.org/intro.html 

## Build ##

### Library Only ###

From within the top-level directory:

 ```
 mkdir build
 cd build
 cmake ..
 make
 ```

 For newer versions of CMake:

 ```
 mkdir build
 cmake . -B build [options]
 cmake --build build [options]
 ```

 When building with MSVC, use the "--config" flag to set the build type:

 ```
 cmake --build build --config [Release|Debug]
 ```

### Library and Examples ###

From within the top-level directory:

 ```
 mkdir build
 cd build
 cmake -DBUILD_EXAMPLES=ON ..
 make
 ```

 To run the examples, you will need to copy the XML files in the Examples/ directory to the location of the built binaries.

## Primary Contributers ##

- Geoff Brian
- Shane Hill
- Dan Newman
- Rob Curtin
- Rob Porter
- Mike Young
- Jon Dansie
- Kylie Bedwell
- Mike Grant

## Licence ##

### Janus ###

Janus is released under the MIT licence. Licence text:

DST Janus Library (Janus DAVE-ML Interpreter Library)

Defence Science and Technology (DST) Group
Department of Defence, Australia.
506 Lorimer St
Fishermans Bend, VIC
AUSTRALIA, 3207

Copyright 2005-2021 Commonwealth of Australia

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the "Software"), to deal in the Software
without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be included in all copies
or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

### Ute ###

Ute is released under the MIT licence. Licence text:

DST Ute Library (Utilities Library)

Defence Science and Technology (DST) Group
Department of Defence, Australia.
506 Lorimer St
Fishermans Bend, VIC
AUSTRALIA, 3207

Copyright 2005-2021 Commonwealth of Australia

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the "Software"), to deal in the Software
without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be included in all copies
or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.