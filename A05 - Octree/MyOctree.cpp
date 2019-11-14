/*----------------------------------------------
Programmer: David Lin (dl3061@rit.com)

Represents a node in the octree
----------------------------------------------*/

#include "MyOctree.h"
using namespace Simplex;

MyOctree::MyOctree(int depth, vector3 center, vector3 radius)
{
	if (depth < 0)
		depth = 1;

	this->m_iDepth = depth;
	this->m_v3Center = center;
	this->m_v3Radius = radius;
	this->m_EntityCount = 0;

	RegenerateOctree();
}

Simplex::MyOctree::~MyOctree()
{
	delete[] m_pEntityList;

	// Delete children. 
	if (m_iDepth != 1)
	{
		// Each child's destructor will also delete their children.
		for (int i = 0; i < 8; i++)
		{
			delete m_pChildNodes[i];
		}
	}
}

/*
		Tries to add an entity to this node.
		Returns 1 if it can. 
		Returns 0 if it cannot, ie does not lie within this node's dimensions.
*/
int MyOctree::AddEntity(MyEntity* a_entity)
{
	if (a_entity != nullptr)
	{
		MyEntity* entity = a_entity;

		vector3 pos = entity->GetRigidBody()->GetCenterGlobal();

		// If it's in 
		if (IsPositionInTree(pos))
		{
			// Add the entity 
			// create a new temp array with one extra entry
			MyEntity** tempArray = new MyEntity*[m_EntityCount + 1];
			uint uCount = 0;
			if (m_pEntityList)
			{
				for (uint i = 0; i < m_EntityCount; i++)
				{
					tempArray[uCount] = m_pEntityList[i];
					++uCount;
				}

				delete[] m_pEntityList;
			}
			tempArray[uCount] = a_entity;
			m_EntityCount += 1;

			//make the member pointer the temp pointer
			m_pEntityList = tempArray;

			// m_pEntityList.push_back(a_entity);

			// Try to add in each of 8 children
			if (m_iDepth != 1)
			{
				for (int i = 0; i < 8; i++)
				{
					if (m_pChildNodes[i]->AddEntity(a_entity) == 1)
					{
						break;
					}
				}
			}

			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

int MyOctree::AreEntitiesInSameNode(MyEntity* a_entity_A, MyEntity* a_entity_B, int a_depth)
{
	// Check if they're both contained
	int containsA = 0;
	int containsB = 0;
	for (uint i = 0; i < m_EntityCount; i++)
	{
		if (m_pEntityList[i] == a_entity_A)
			containsA = 1;
		if (m_pEntityList[i] == a_entity_B)
			containsB = 1;
	}

	// If they are....
	if (containsA && containsB)
	{
		// And we're at the right depth, return
		if (a_depth == m_iDepth)
		{
			return 1;
		}
		else if (a_depth < m_iDepth)
		{
			// Call child nodes
			int child_contains_both = 0;
			for (uint i = 0; i < 8; i++)
			{
				child_contains_both = m_pChildNodes[i]->AreEntitiesInSameNode(a_entity_A, a_entity_B, a_depth);

				if (child_contains_both)
					break;
			}

			return child_contains_both;
		}
	}

	return 0;
}

void Simplex::MyOctree::CheckCollisions(int a_depth)
{
	if (m_iDepth == a_depth && m_EntityCount > 0)
	{
		// Perform collision check
		uint checkCount = m_EntityCount - 1; // Placed here for breakpoint

		for (uint i = 0; i < checkCount; i++)
		{
			for (uint j = i + 1; j < m_EntityCount; j++)
			{
				vector3 pos1 = m_pEntityList[i]->GetRigidBody()->GetCenterGlobal();
				vector3 pos2 = m_pEntityList[j]->GetRigidBody()->GetCenterGlobal();

				m_pEntityList[i]->IsColliding(m_pEntityList[j]);
			}
		}
	}
	else if (a_depth < m_iDepth)
	{
		// Perform for each child
		for (uint i = 0; i < 8; i++)
		{
			m_pChildNodes[i]->CheckCollisions(a_depth);
		}
	}
}


/*
	
*/
void Simplex::MyOctree::RegenerateOctree(int a_depth)
{
	// 
	vector3 radius = m_v3Radius;
	vector3 center = m_v3Center;

	// If a_depth is specified, change the depth of this tree.
	if (a_depth > 0)
		m_iDepth = a_depth;

	int depth = m_iDepth;

	if (depth > 1)
	{
		// Generate children
		int child_depth = depth - 1;
		vector3 child_radius = radius / 2;
		vector3 child_center = center;

		// 0
		child_center = vector3(center.x - child_radius.x, center.y - child_radius.y, center.z - child_radius.z);
		MyOctree* child_0 = new MyOctree(child_depth, child_center, child_radius);
		m_pChildNodes[0] = child_0;

		// 1
		child_center = vector3(center.x - child_radius.x, center.y - child_radius.y, center.z + child_radius.z);
		MyOctree* child_1 = new MyOctree(child_depth, child_center, child_radius);
		m_pChildNodes[1] = child_1;

		// 2
		child_center = vector3(center.x - child_radius.x, center.y + child_radius.y, center.z - child_radius.z);
		MyOctree* child_2 = new MyOctree(child_depth, child_center, child_radius);
		m_pChildNodes[2] = child_2;

		// 3
		child_center = vector3(center.x - child_radius.x, center.y + child_radius.y, center.z + child_radius.z);
		MyOctree* child_3 = new MyOctree(child_depth, child_center, child_radius);
		m_pChildNodes[3] = child_3;

		// 4
		child_center = vector3(center.x + child_radius.x, center.y - child_radius.y, center.z - child_radius.z);
		MyOctree* child_4 = new MyOctree(child_depth, child_center, child_radius);
		m_pChildNodes[4] = child_4;

		// 5
		child_center = vector3(center.x + child_radius.x, center.y - child_radius.y, center.z + child_radius.z);
		MyOctree* child_5 = new MyOctree(child_depth, child_center, child_radius);
		m_pChildNodes[5] = child_5;

		// 6
		child_center = vector3(center.x + child_radius.x, center.y + child_radius.y, center.z - child_radius.z);
		MyOctree* child_6 = new MyOctree(child_depth, child_center, child_radius);
		m_pChildNodes[6] = child_6;

		// 7
		child_center = vector3(center.x + child_radius.x, center.y + child_radius.y, center.z + child_radius.z);
		MyOctree* child_7 = new MyOctree(child_depth, child_center, child_radius);
		m_pChildNodes[7] = child_7;
	}

	if (m_iDepth > 1)
	{
		// Get each entity
		for (int i = 0; i < m_EntityCount; i++)
		{
			MyEntity* entity = m_pEntityList[i];

			// Try to add entities into children nodes
			for (int child = 0; child < 8; child++)
			{
				if (m_pChildNodes[child]->AddEntity(entity) == 1)
				{
					break;
				}
			}
		}

		// Add mesh manager to each child
		for (int child = 0; child < 8; child++)
		{
			m_pChildNodes[child]->SetMeshManager(m_pMeshMngr);
		}
	}
}

void Simplex::MyOctree::DisplayGrid()
{
	// Only have lowest depth display grid.
	if (m_iDepth > 1)
	{
		for (int i = 0; i < 8; i++)
		{
			m_pChildNodes[i]->DisplayGrid();
		}
	}
	else
	{
		if (m_pMeshMngr != nullptr)
		{
			// Horizontal lines
			m_pMeshMngr->AddLineToRenderList(IDENTITY_M4,
				vector3(m_v3Center.x - m_v3Radius.x, m_v3Center.y - m_v3Radius.y, m_v3Center.z - m_v3Radius.z),
				vector3(m_v3Center.x + m_v3Radius.x, m_v3Center.y - m_v3Radius.y, m_v3Center.z - m_v3Radius.z),
				C_YELLOW, C_YELLOW);

			m_pMeshMngr->AddLineToRenderList(IDENTITY_M4,
				vector3(m_v3Center.x - m_v3Radius.x, m_v3Center.y - m_v3Radius.y, m_v3Center.z + m_v3Radius.z),
				vector3(m_v3Center.x + m_v3Radius.x, m_v3Center.y - m_v3Radius.y, m_v3Center.z + m_v3Radius.z),
				C_YELLOW, C_YELLOW);

			m_pMeshMngr->AddLineToRenderList(IDENTITY_M4,
				vector3(m_v3Center.x - m_v3Radius.x, m_v3Center.y + m_v3Radius.y, m_v3Center.z - m_v3Radius.z),
				vector3(m_v3Center.x + m_v3Radius.x, m_v3Center.y + m_v3Radius.y, m_v3Center.z - m_v3Radius.z),
				C_YELLOW, C_YELLOW);

			m_pMeshMngr->AddLineToRenderList(IDENTITY_M4,
				vector3(m_v3Center.x - m_v3Radius.x, m_v3Center.y + m_v3Radius.y, m_v3Center.z + m_v3Radius.z),
				vector3(m_v3Center.x + m_v3Radius.x, m_v3Center.y + m_v3Radius.y, m_v3Center.z + m_v3Radius.z),
				C_YELLOW, C_YELLOW);

			// Vertical lines
			m_pMeshMngr->AddLineToRenderList(IDENTITY_M4,
				vector3(m_v3Center.x - m_v3Radius.x, m_v3Center.y - m_v3Radius.y, m_v3Center.z - m_v3Radius.z),
				vector3(m_v3Center.x - m_v3Radius.x, m_v3Center.y + m_v3Radius.y, m_v3Center.z - m_v3Radius.z),
				C_YELLOW, C_YELLOW);

			m_pMeshMngr->AddLineToRenderList(IDENTITY_M4,
				vector3(m_v3Center.x - m_v3Radius.x, m_v3Center.y - m_v3Radius.y, m_v3Center.z + m_v3Radius.z),
				vector3(m_v3Center.x - m_v3Radius.x, m_v3Center.y + m_v3Radius.y, m_v3Center.z + m_v3Radius.z),
				C_YELLOW, C_YELLOW);

			m_pMeshMngr->AddLineToRenderList(IDENTITY_M4,
				vector3(m_v3Center.x + m_v3Radius.x, m_v3Center.y - m_v3Radius.y, m_v3Center.z - m_v3Radius.z),
				vector3(m_v3Center.x + m_v3Radius.x, m_v3Center.y + m_v3Radius.y, m_v3Center.z - m_v3Radius.z),
				C_YELLOW, C_YELLOW);

			m_pMeshMngr->AddLineToRenderList(IDENTITY_M4,
				vector3(m_v3Center.x + m_v3Radius.x, m_v3Center.y - m_v3Radius.y, m_v3Center.z + m_v3Radius.z),
				vector3(m_v3Center.x + m_v3Radius.x, m_v3Center.y + m_v3Radius.y, m_v3Center.z + m_v3Radius.z),
				C_YELLOW, C_YELLOW);

			// Coming at you lines
			m_pMeshMngr->AddLineToRenderList(IDENTITY_M4,
				vector3(m_v3Center.x - m_v3Radius.x, m_v3Center.y - m_v3Radius.y, m_v3Center.z - m_v3Radius.z),
				vector3(m_v3Center.x - m_v3Radius.x, m_v3Center.y - m_v3Radius.y, m_v3Center.z + m_v3Radius.z),
				C_YELLOW, C_YELLOW);

			m_pMeshMngr->AddLineToRenderList(IDENTITY_M4,
				vector3(m_v3Center.x - m_v3Radius.x, m_v3Center.y + m_v3Radius.y, m_v3Center.z - m_v3Radius.z),
				vector3(m_v3Center.x - m_v3Radius.x, m_v3Center.y + m_v3Radius.y, m_v3Center.z + m_v3Radius.z),
				C_YELLOW, C_YELLOW);

			m_pMeshMngr->AddLineToRenderList(IDENTITY_M4,
				vector3(m_v3Center.x + m_v3Radius.x, m_v3Center.y - m_v3Radius.y, m_v3Center.z - m_v3Radius.z),
				vector3(m_v3Center.x + m_v3Radius.x, m_v3Center.y - m_v3Radius.y, m_v3Center.z + m_v3Radius.z),
				C_YELLOW, C_YELLOW);

			m_pMeshMngr->AddLineToRenderList(IDENTITY_M4,
				vector3(m_v3Center.x + m_v3Radius.x, m_v3Center.y + m_v3Radius.y, m_v3Center.z - m_v3Radius.z),
				vector3(m_v3Center.x + m_v3Radius.x, m_v3Center.y + m_v3Radius.y, m_v3Center.z + m_v3Radius.z),
				C_YELLOW, C_YELLOW);


			//m_pMeshMngr->AddGridToRenderList(glm::translate(vector3(m_v3Center.y - m_v3Radius.y, 0.0f, 0.0f)) * glm::rotate(IDENTITY_M4, 1.5708f, AXIS_Y));
			//m_pMeshMngr->AddGridToRenderList(glm::translate(vector3(m_v3Center.y + m_v3Radius.y, 0.0f, 0.0f)) * glm::rotate(IDENTITY_M4, 1.5708f, AXIS_Y));
		}
	}
}

void Simplex::MyOctree::SetMeshManager(MeshManager* a_pMeshMngr)
{
	this->m_pMeshMngr = a_pMeshMngr;

	if (m_iDepth > 1)
	{
		for (int i = 0; i < 8; i++)
		{
			m_pChildNodes[i]->SetMeshManager(a_pMeshMngr);
		}
	}
}

int MyOctree::IsPositionInTree(vector3 pos)
{
	vector3 min = m_v3Center - m_v3Radius;
	vector3 max = m_v3Center + m_v3Radius;

	if (pos.x >= min.x && pos.x <= max.x &&
		pos.y >= min.y && pos.y <= max.y &&
		pos.z >= min.z && pos.z <= max.z
		)
	{
		return 1;
	}

	return 0;
}

