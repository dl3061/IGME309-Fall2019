/*----------------------------------------------
Programmer: David Lin (dl3061@rit.edu)
Date: 9/22/2019
----------------------------------------------*/
#ifndef SPACE_INVADER_H
#define SPACE_INVADER_H

#include "Definitions.h"
#include "MyMesh.h"

#include <vector>

enum Motion {
	None,
	Classic,		// Invader goes left to right 6*len, moving down at an edge
	Sine			// Moves in a sine
};

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

	// Sets a motion and resets the timer
	void SetMotion(Motion motion);

	// Sets a scale
	void SetScale(float scale);

	// Render
	void Render(matrix4 projMatrix, matrix4 viewMatrix, matrix4 arcball);

private:

	// The location of the bottom-left block. 
	vector3 basePosition;

	// The size of the block and offset between each block
	float sizePerBlock;

	// The scale of the invader, if any
	float scale;

	// The vector holding all the meshes
	std::vector<MyMesh*> mesh;

	// Function to fill mesh.
	void DrawSpaceInvader();

	// Partially fills a row in the mesh vector with blocks from startIndex to endIndex
	void FillMeshRow(int row, int startIndex, int endIndex);

	// Color of the invader
	vector3 blockColor = vector3(1, 1, 1);

	// The programmed Motion, if any
	Motion motion;

	// Timer for motion
	float elapsedTime;

	float GetHeight() {
		return sizePerBlock * 8;
	}

	float GetWidth() {
		return sizePerBlock * 10;
	}
};

#endif