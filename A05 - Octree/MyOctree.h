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
	*/
	void CheckCollisions(int a_depth);


	/*
	*/
	void RegenerateOctree();


private:
	int m_iDepth;
	vector3 m_v3Center;
	vector3 m_v3Radius;

	MyOctree* m_pParentNode = nullptr;
	MyOctree* m_pChildNodes[8];

	uint m_EntityCount;
	MyEntity** m_pEntityList; // List of entities ptrs under this node

	int IsPositionInTree(vector3 pos);
};

}
#endif

