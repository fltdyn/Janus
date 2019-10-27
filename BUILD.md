## Library Only ##

From within the top-level directory:

 ```mkdir build
 cd build
 cmake ..
 make
 ```

## Library and Examples ##

From within the top-level directory:

 ```
 mkdir build
 cd build
 cmake -DBUILD_EXAMPLES=ON ..
 make
 ```