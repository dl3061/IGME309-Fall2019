/*----------------------------------------------
Programmer: David Lin (dl3061@rit.com)

Represents a node in the octree
----------------------------------------------*/

#ifndef __MY_OCTREE_H
#define __MY_OCTREE_H

#include "MyEntity.h"
#include <vector>

namespace Simplex{

class MyOctree
{
public:
	MyOctree(int depth, vector3 center, vector3 radius);
	~MyOctree();

	/*
		Tries to add an entity to this node.
		Success if it can. Failure if it cannot, ie does not lie within this node's dimensions.
	*/
	int AddEntity(MyEntity* a_entity);


	/*
		Checks if two entities are in the same node at a given depth.
	*/
	int AreEntitiesInSameNode(MyEntity* a_entity_A, MyEntity* a_entity_B, int a_depth);


	/*
		Performs collision check on all entities at a specified internal depth.
	*/
	void CheckCollisions(int a_depth);


	/*
		Regenerates the tree to a new specified depth, or regenerates at the same depth if not specified. 
		Will recheck all object locations when generating new tree.
	*/
	void RegenerateOctree(int a_depth = -1);

	/*
		Displays the grid.
	*/
	void DisplayGrid();

	void SetMeshManager(MeshManager* a_pMeshMngr);

private:
	int m_iDepth;
	vector3 m_v3Center;
	vector3 m_v3Radius;

	MyOctree* m_pParentNode = nullptr;
	MyOctree* m_pChildNodes[8];

	uint m_EntityCount;
	MyEntity** m_pEntityList; // List of entities ptrs under this node

	MeshManager* m_pMeshMngr = nullptr; //Mesh Manager

	int IsPositionInTree(vector3 pos);
};

}
#endif

