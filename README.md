# SnakeDimas

No explanations needed.

## How to build

Dependencies:
* C++14 capable compiler;
* cmake;
* ncurses (Ubuntu package: libncurses5-dev).

First, make sure you have initialized all git submodules:
```bash
$ git submodule update --init --recursive
```

### Building with cmake:

Release build:
```bash
$ mkdir -p build/release
$ cd build/release
$ cmake ../.. -DCMAKE_BUILD_TYPE=Release
$ cmake --build .
```

Debug build:
```bash
$ mkdir -p build/debug
$ cd build/debug
$ cmake ../.. -DCMAKE_BUILD_TYPE=Debug
$ cmake --build .
```

### Parallel builds

If your machine has multiple processors cores you can accelerate the build passing the ```-j<num_of_jobs>``` to ```make``` (assuming you aren't using MSVC). Example:
```bash
$ cmake --build . -- -j4 # create up to 4 parallel jobs
```

## Running

Execute the ```snakedimas``` binary:

```bash
$ ./snakedimas
```
