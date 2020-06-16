# [IFE] Project

A belote project for our C class.
Have a look at the [report](report.pdf) for more information!

## Installation

Clone this repository:

```sh
git clone https://github.com/adri326/ife-project --recursive
cd ife-project
```

### With CMake

Build the project like any CMake project:

```sh
mkdir build && cd build
cmake ..
make
```

You will need to have `SDL2`, `SDL2_image`, `SDL2_ttf` and `SDL2_mixer` installed on your system or instruct CMake on how to find them.

Two projects will be built: `ife-project` (which is the game itself), and `ife-project-test` (which are the tests for the game).

### CodeBlocks on Windows

Simply open the `/project.cbp` project file. Libraries are included in the `/lib` folder and will be statically linked with the `Release` build option.

To run the tests, open the `/project-test.cbp` project file.

This project has only been tested with the MinGW suite. Other suites are not officially supported as of the writing of this.
