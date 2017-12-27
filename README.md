# 3dprint
Now this program can read the STL model and transform the vertex positions of a primitive into a regular eulerian grid  
The program used modern OpenGL to display the graph.   
I set four viewports to display the `patch model`, the `voxel model`, the slicing layer with the `same layer thickness` and the slicing layer with `adaptive slicing layer thickness` separately.   
in order to compile and run this program, the `glfw` ,`glew` and `glm` are necessary.   
use makefile to run it
```
cd 3dprint/
make
```
I uploaded some examples for it like `Oblique cube.STL`
The program can be run by
```
./main Obique cube.STL 6 100
```
The second parameter is the name of the STL model.    
The third parameter is the depth of the octree.    
The forth parameter is the numer of the layers of the slcing layered strategy.  
![example] (https://github.com/conceptclear/3dprint/blob/master/pictures/pic1.png "Taj Mahal")
