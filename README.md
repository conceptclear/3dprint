# 3dprint
now this program can read the STL model and transform the vertex positions of a primitive into a regular eulerian grid
use makefile to run it
``` javascript
cd 3dprint/
make
```
I uploaded an example for it `Oblique cube.STL`
the program can be run by
``` javascript
./main Obique cube.STL 6
```
the last parameter is the depth of the octree
