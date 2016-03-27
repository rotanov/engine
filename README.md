# engine

## Building

Prerequisites on windows:
 - DirectX SDK (June 2010)
 - Visual Studio 2015

```sh
    git clone --recursive https://github.com/rotanov/engine.git
    git submodule update --init --recursive
    mkdir engine/build
    cd engine/build
    cmake -G "Visual Studio 14" ../cmake
```
