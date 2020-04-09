# Anywhere Door

A simple single file transferring application. 

## Dependencies
1. Stand-alone ASIO ([https://think-async.com/Asio/AsioStandalone.html](https://think-async.com/Asio/AsioStandalone.html)).
2. {fmt} ([https://github.com/fmtlib/fmt](https://github.com/fmtlib/fmt)).
3. FlatBuffers ([https://github.com/google/flatbuffers](https://github.com/google/flatbuffers))

## Building
1. Install all dependencies with Microsoft's <a href="https://github.com/Microsoft/vcpkg">vcpkg</a>.
    * `vcpkg install asio`
    * `vcpkg install fmt`
    * `vcpkg install flatbuffers`
2. Clone the project: `git clone https://github.com/phuongtran7/Anywhere_Door`.
3. Build the project:
```
cd Anywhere_Door
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg.cmake ..
cmake --build .
```

## Usage
1. Put the compiled binary next to files that need to be transferred and another at destination.
2. Start the destination binary first.