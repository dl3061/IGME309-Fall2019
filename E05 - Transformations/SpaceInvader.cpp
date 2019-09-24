#include "SpaceInvader.h"

SpaceInvader::SpaceInvader(float sizePerBlock, vector3 baseCoordinate)
{
	mesh = std::vector<MyMesh*>();

	this->sizePerBlock = sizePerBlock;
	this->scale = 1.0f;
	this->basePosition = baseCoordinate;
	
	this->elapsedTime = 0;
	this->motion = Motion::None;

	DrawSpaceInvader();
}

SpaceInvader::~SpaceInvader()
{
	for (int i = 0; i < mesh.size(); i++)
	{
		delete mesh[i];
	}
}

SpaceInvader::SpaceInvader(SpaceInvader& other)
{
	// Copy the vector of pointers, but don't duplicate meshes. 
	this->mesh = other.mesh;

	this->sizePerBlock = other.sizePerBlock;
	this->scale = other.scale;
	this->basePosition = other.basePosition;
	this->elapsedTime = other.elapsedTime;
	this->motion = other.motion;
}

SpaceInvader& SpaceInvader::operator=(SpaceInvader& other)
{
	if (this != &other)
	{
		// Copy the vector of pointers, but don't duplicate meshes. 
		this->mesh = other.mesh;

		this->sizePerBlock = other.sizePerBlock;
		this->scale = other.scale;
		this->basePosition = other.basePosition;
		this->elapsedTime = other.elapsedTime;
		this->motion = other.motion;
	}
	return *this;
}

std::vector<MyMesh*> SpaceInvader::GetMesh()
{
	return mesh;
}

void SpaceInvader::SetMotion(Motion motion)
{
	elapsedTime = 0;

	this->motion = motion;
}

void SpaceInvader::SetScale(float scale)
{
	this->scale = scale;
}


void SpaceInvader::Render(matrix4 projMatrix, matrix4 viewMatrix, matrix4 arcball)
{
	// Local copy of view and proj matrix and arcball, to be modified for transformations.
	matrix4 lViewMatrix = viewMatrix;
	matrix4 lProjMatrix = projMatrix;
	matrix4 lArcball = arcball;

	matrix4 identity = matrix4(1.0f);
	
	//	the scale will always be constant for this exercise.
	matrix4 m4Scale = glm::scale(identity, vector3(scale, scale, scale));	

	// Apply motion
	if (this->motion == Motion::Sine)
	{
		matrix4 m4Translate = glm::translate(identity, vector3(elapsedTime, sin(elapsedTime) * GetHeight(), 0.0f));

		lArcball = lArcball + m4Scale * m4Translate;
	}
	else if (this->motion == Motion::Classic)
	{
		float maxWidth = GetWidth() * 3;
		float rowHeight = GetHeight();
		int row = ((int)(elapsedTime / (maxWidth + rowHeight)));

		float progIntoRow = fmod(elapsedTime, (maxWidth + rowHeight));

		// xDisplacement
			// from 0 to maxWidth, move
			// from maxWidth to maxWidth+rowHeight, wait

		float x = 0;
		if (progIntoRow <= maxWidth)
		{
			// If moving
				// If on an even row, x = elapsed time  from 0 to maxWidth
				// If on an odd row, x = elapsed time from maxWidth down to 0
				// progress along the path is represented in progIntoRow

			if (row % 2)
				x = maxWidth - progIntoRow;
			else
				x = progIntoRow;
		}
		else
		{
			// If not moving, snap to left edge or right edge
			if (row % 2)
				x = 0.0f;
			else
				x = maxWidth;
		}

		// yDisplacement
			// from 0 to maxWidth, wait
			// from maxWidth to maxWidth+rowHeight, move

		float y = 0;
		if (progIntoRow <= maxWidth)
		{
			y = rowHeight * (-row);
		}
		else
		{
			y = rowHeight * (-row) - (progIntoRow - maxWidth);
		}

		matrix4 m4Translate = glm::translate(identity, vector3(x, y, 0.0f));

		lArcball = lArcball + m4Scale * m4Translate;
	}
	
	// Pass time
	elapsedTime += 0.1f;

	// Render
	for (int i = 0; i < mesh.size(); i++)
	{
		mesh[i]->Render(lProjMatrix, lViewMatrix, lArcball);
	}
}

void SpaceInvader::DrawSpaceInvader()
{
	// 0th row
	FillMeshRow(0, 3, 4);
	FillMeshRow(0, 6, 7);

	// 1st row
	FillMeshRow(1, 0, 0);
	FillMeshRow(1, 2, 2);
	FillMeshRow(1, 8, 8);
	FillMeshRow(1, 10, 10);

	// 2nd row
	FillMeshRow(2, 0, 0);
	FillMeshRow(2, 2, 8);
	FillMeshRow(2, 10, 10);

	// 3rd row
	FillMeshRow(3, 0, 10);

	// 4th row
	FillMeshRow(4, 1, 2);
	FillMeshRow(4, 4, 6);
	FillMeshRow(4, 8, 9);

	// 5th row
	FillMeshRow(5, 2, 8);

	// 6th row
	FillMeshRow(6, 3, 3);
	FillMeshRow(6, 7, 7);

	// 7th row
	FillMeshRow(7, 2, 2);
	FillMeshRow(7, 8, 8);
}

void SpaceInvader::FillMeshRow(int row, int startIndex, int endIndex)
{
	int i_startIndex = startIndex;
	int i_endIndex = endIndex;

	if (startIndex > endIndex)
	{
		// Swap em. 
		i_startIndex = endIndex;
		i_endIndex = startIndex;
	}

	for (int i = i_startIndex; i <= i_endIndex; i++)
	{
		vector3 pos = basePosition;
		pos.x += i * sizePerBlock;
		pos.y += row * sizePerBlock;

		MyMesh* block = new MyMesh();
		block->GenerateCube(sizePerBlock, pos, blockColor);

		mesh.push_back(block);
	}
}


