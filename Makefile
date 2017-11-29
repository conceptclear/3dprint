main : main.o ReadSTL.o Patchmodel.o Octree.o Voxelization.o
	g++ -g -o main main.o ReadSTL.o Patchmodel.o Octree.o Voxelization.o -lGL -lGLU -lglut
 
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
clean :
	rm main main.o ReadSTL.o Patchmodel.o Octree.o Voxelization.o
