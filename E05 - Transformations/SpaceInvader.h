/*----------------------------------------------
Programmer: David Lin (dl3061@rit.edu)
Date: 9/22/2019
----------------------------------------------*/
#ifndef SPACE_INVADER_H
#define SPACE_INVADER_H

#include "Definitions.h"
#include "MyMesh.h"

#include <vector>

class SpaceInvader
{
public:
	// Constructor
	SpaceInvader(float sizePerBlock = 1.0, vector3 baseCoordnate = vector3(0, 0,0));
	
	// Destructor
	~SpaceInvader();

	// Copy Constructor
	SpaceInvader(SpaceInvader& other);

	// Copy Assignment Construcotr
	SpaceInvader& operator=(SpaceInvader& other);

	// Public accessor for mesh
	std::vector<MyMesh*> GetMesh();

	// 
	// void Render();

private
	:
	// The location of the bottom-left block. 
	vector3 basePosition;

	// The size of the block and offset between each block
	float sizePerBlock;

	// The vector holding all the meshes
	std::vector<MyMesh*> mesh;

	// Function to fill mesh.
	void DrawSpaceInvader();

	// Partially fills a row in the mesh vector with blocks from startIndex to endIndex
	void FillMeshRow(int row, int startIndex, int endIndex);

	//
	vector3 blockColor = vector3(1, 1, 1);
};

#endif