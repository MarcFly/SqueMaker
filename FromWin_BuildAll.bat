mkdir cmake_bloat_towin & 
cd cmake_bloat_towin & 
cmake ../ -UToAndroid -UToLinux -DToWindows=1 -DCMAKE_BUILD_TYPE=Debug

cmake --build . --config Release
cmd /k