# Route Planning Project

This repo contains the starter code for the Route Planning project.

<img src="map.png" width="600" height="450" />

## Cloning

git clone https:https://github.com/ArincDemirel/Udacity-Cpp-Nanodegree-Repo.git
```
Go to the Route Planning project directory: cd C_Plus_Plus_Nanodegree/Project_1_Route_Planning
```

## Dependencies for Running Locally
* cmake >= 3.11.3
  * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1 (Linux, Mac), 3.81 (Windows)
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* gcc/g++ >= 7.4.0
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same instructions as make - [install Xcode command line tools](https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)
* IO2D
  * Installation instructions for all operating systems can be found [here](https://github.com/cpp-io2d/P0267_RefImpl/blob/master/BUILDING.md)
  * This library must be built in a place where CMake `find_package` will be able to find it
 

## Compiling and Running

### Compiling
```
mkdir build && cd build
```
From within the `build` directory, then run `cmake` and `make` as follows:
```
cmake ..
make
```

### Running

The executable will be placed in the `build` directory. From within `build`, you can run the project as follows:
```
./OSM_A_star_search
```
Or to specify a map file:
```
./OSM_A_star_search -f ../<your_osm_file.osm>
```


## Usage

The program will ask you to enter the starting position and the ending position at the beginning as stated below:
Please enter start_x, start_y, end_x, end_y in order
For example, just input 10 10 90 90 and press Enter, the shortest path will be drawn on the map.

## Testing

The testing executable is also placed in the `build` directory. From within `build`, you can run the unit tests as follows:
```
./test
```

