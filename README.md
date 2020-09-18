# Build

## Prerequisites
Python  CMake

## Step 1: Install Conan
pip install conan

## Step 2: Install Dependencies
git clone https://github.com/vsensu/LearnCraft.git

$ cd LearnCraft

$ mkdir build && cd build

$ conan install ..

## Step 3: Build
(win)

$ cmake .. -G "Visual Studio 16"

$ cmake --build . --config Release

(linux, mac)

$ cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release

$ cmake --build .

...

## Step 4: Run
$ ./bin/LearnCraft

Enjoy!