:: run cmake 
cmake -S . -B .\_build -DWITH_EXAMPLES=True -DCMAKE_BUILD_TYPE=Release
:: build it now
cmake --build .\_build --target all_build --config Release