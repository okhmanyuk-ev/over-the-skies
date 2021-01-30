mkdir build_clang
cd build_clang
cmake .. -DBUILD_DEVELOPER=TRUE -G Ninja -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
ninja
cd ..
pause