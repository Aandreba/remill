[windows]
build:
    cmake -E make_directory build
    cmake -B build -DCMAKE_BUILD_TYPE=Debug
    cmake --build build
