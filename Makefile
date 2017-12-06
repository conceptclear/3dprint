main : main.o ReadSTL.o Patchmodel.o Octree.o Voxelization.o Voxelization_EdgeChange_Bresenham.o Voxelization_FacetChange_FloodFill.o Voxelization_FacetChange_Scanning.o
	g++ -g -o main main.o ReadSTL.o Patchmodel.o Octree.o Voxelization.o Voxelization_EdgeChange_Bresenham.o Voxelization_FacetChange_FloodFill.o Voxelization_FacetChange_Scanning.o -lGL -lGLU -lglut
 
main.o : main.cpp Octree/Voxelization.h Patchmodel/Patchmodel.h
	g++ -g -c main.cpp
ReadSTL.o : Patchmodel/ReadSTL.cpp Patchmodel/Patchmodel.h
	g++ -g -c Patchmodel/ReadSTL.cpp
Patchmodel.o : Patchmodel/Patchmodel.cpp Patchmodel/Patchmodel.h
	g++ -g -c Patchmodel/Patchmodel.cpp
Octree.o : Octree/Octree.cpp Octree/Octree.h
	g++ -g -c Octree/Octree.cpp
Voxelization.o : Octree/Voxelization.cpp Octree/Voxelization.h
	g++ -g -c Octree/Voxelization.cpp
Voxelization_EdgeChange_Bresenham.o : Octree/Voxelization_EdgeChange_Bresenham.cpp Octree/Voxelization.h
	g++ -g -c Octree/Voxelization_EdgeChange_Bresenham.cpp
Voxelization_FacetChange_FloodFill.o : Octree/Voxelization_FacetChange_FloodFill.cpp Octree/Voxelization.h
	g++ -g -c Octree/Voxelization_FacetChange_FloodFill.cpp
Voxelization_FacetChange_Scanning.o : Octree/Voxelization_FacetChange_Scanning.cpp Octree/Voxelization.h
	g++ -g -c Octree/Voxelization_FacetChange_Scanning.cpp
clean :
	rm main main.o ReadSTL.o Patchmodel.o Octree.o Voxelization.o Voxelization_EdgeChange_Bresenham.o Voxelization_FacetChange_FloodFill.o Voxelization_FacetChange_Scanning.o
