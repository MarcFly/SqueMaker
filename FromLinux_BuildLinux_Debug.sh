#!/bin/bash
cd ../
mkdir cmake_bloat_tolinux
cd cmake_bloat_tolinux

konsole --noclose -e cmake ../ -UToAndroid -DToLinux=1 -WITH_SOLOUD=ON -DWITH_GLMATH=ON -DCMAKE_BUILD_TYPE=Debug
konsole --noclose -e cmake --build .

#cd ../
#mkdir cmake_bloat_toandroid
#cd cmake_bloat_toandroid
#
#konsole --noclose -e cmake ../ -UToLinux -DToAndroid=1 -DCMAKE_BUILD_TYPE=Debug -DNDK_DEBUG=1
#konsole --noclose -e cmake --build .
#konsole --noclose -e make uninstall push