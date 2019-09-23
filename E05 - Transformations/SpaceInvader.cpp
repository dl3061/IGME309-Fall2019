#include "SpaceInvader.h"

SpaceInvader::SpaceInvader(float sizePerBlock, vector3 baseCoordinate)
{
	mesh = std::vector<MyMesh*>();

	this->sizePerBlock = sizePerBlock;
	this->basePosition = baseCoordinate;

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
	this->basePosition = other.basePosition;
}

SpaceInvader& SpaceInvader::operator=(SpaceInvader& other)
{
	if (this != &other)
	{
		// Copy the vector of pointers, but don't duplicate meshes. 
		this->mesh = other.mesh;

		this->sizePerBlock = other.sizePerBlock;
		this->basePosition = other.basePosition;
	}
	return *this;
}

std::vector<MyMesh*> SpaceInvader::GetMesh()
{
	return mesh;
}

/*
void SpaceInvader::Render()
{
	for (int i = 0; i < mesh.size; i++)
	{
		mesh[i]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), ToMatrix4(m_qArcBall));
	}
}
*/

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
		std::cout << "Drawing Block at (" << pos.x << ", " << pos.y << ")\n";

		std::cout << "SizePerBlock " << sizePerBlock << "\n";


		MyMesh* block = new MyMesh();
		block->GenerateCube(sizePerBlock, pos, blockColor);

		mesh.push_back(block);
	}
}
