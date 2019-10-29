#include "MyEntityManager.h"
using namespace Simplex;

//  MyEntityManager

// Singleton instance of the MyEntinityManager
MyEntityManager* MyEntityManager::m_pInstance = nullptr;

/*
	Initializes the singleton
*/
void MyEntityManager::Init(void)
{
	m_uEntityCount = 0;
	m_entityList = std::vector<MyEntity*>();
}


/*
	Releases the allocated memeber pointers
*/
void MyEntityManager::Release(void)
{
	// Release every allocated memory for the entity list
	for (int i = 0; i < m_entityList.size(); i++)
	{
		delete m_entityList[i];
		m_entityList[i] = nullptr;
	}

	// Clear the list
	m_entityList.clear();

	// Reset the count to zero
	m_uEntityCount = 0;
}


/*
	Gets the singleton pointer
*/
MyEntityManager* MyEntityManager::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new MyEntityManager();
	}
	return m_pInstance;
}


/*
	Releases the content of the singleton
*/
void MyEntityManager::ReleaseInstance()
{
	if (m_pInstance != nullptr)
	{
		m_pInstance->Release();
	}
}


/*
	Gets the index (from the list of entities) of the entity specified by UniqueID
*/
int Simplex::MyEntityManager::GetEntityIndex(String a_sUniqueID)
{
	for (int i = 0; i < m_entityList.size(); i++)
	{
		if (GetUniqueID(i).compare(a_sUniqueID) == 0)
		{
			return i;
		}
	}

	// Not found
	return -1;
}


#pragma region Accessors


/*
	Gets the model associated with this entity
	if a_uIndex < 0, get the last entity in the list
*/
Model* Simplex::MyEntityManager::GetModel(uint a_uIndex)
{	
	// Compute the index, if positive do nothing. If "negative", get last index.
	uint index = a_uIndex;
	if (index == (-1))
		index = m_entityList.size() - 1;

	// Get the entity and return its model
	MyEntity* entity = GetEntity(index);
	if (entity != nullptr)
	{
		return entity->GetModel();
	}

	return nullptr;
}


/*
	Gets the model associated with this entity
*/
Model* Simplex::MyEntityManager::GetModel(String a_sUniqueID)
{
	// Convert unique ID to index
	int index = GetEntityIndex(a_sUniqueID);
	
	if (index != (-1))
		return GetModel(index);
	else
		return nullptr;
}


/*
	Gets the Rigid Body associated with this entity
	if a_uIndex < 0, get the last entity in the list
*/
RigidBody* Simplex::MyEntityManager::GetRigidBody(uint a_uIndex)
{
	// Compute the index, if positive do nothing. If "negative", get last index.
	uint index = a_uIndex;
	if (index == (-1))
		index = m_entityList.size() - 1;

	// Get the entity and return its rigidbody
	MyEntity* entity = GetEntity(index);
	if (entity != nullptr)
	{
		return entity->GetRigidBody();
	}

	return nullptr;
}


/*
	Gets the Rigid Body associated with this entity
*/
RigidBody* Simplex::MyEntityManager::GetRigidBody(String a_sUniqueID)
{
	// Convert unique ID to index
	int index = GetEntityIndex(a_sUniqueID);

	if (index != (-1))
		return GetRigidBody(index);
	else
		return nullptr;
}


/*
	Gets the model matrix associated with this entity
	if a_uIndex < 0, get the last entity in the list
*/
matrix4 Simplex::MyEntityManager::GetModelMatrix(uint a_uIndex)
{
	// Compute the index, if positive do nothing. If "negative", get last index.
	uint index = a_uIndex;
	if (index == (-1))
		index = m_entityList.size() - 1;

	// Get the entity and return its model matrix
	MyEntity* entity = GetEntity(index);
	if (entity != nullptr)
	{
		return entity->GetModelMatrix();
	}

	return IDENTITY_M4;
}


/*
	Gets the model matrix associated with this entity
*/
matrix4 Simplex::MyEntityManager::GetModelMatrix(String a_sUniqueID)
{
	// Convert unique ID to index
	int index = GetEntityIndex(a_sUniqueID);

	if (index != (-1))
		return GetModelMatrix(index);
	else
		return IDENTITY_M4;
}


/*
	Sets the model matrix associated to the entity specified in the ID
*/
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, String a_sUniqueID)
{
	// Convert unique ID to index
	int index = GetEntityIndex(a_sUniqueID);

	if (index != (-1))
		return SetModelMatrix(a_m4ToWorld, index);
}


/*
	Sets the model matrix associated to the entity specified in the ID
	if a_uIndex < 0, get the last entity in the list
*/
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, uint a_uIndex)
{
	// Compute the index, if positive do nothing. If "negative", get last index.
	uint index = a_uIndex;
	if (index == (-1))
		index = m_entityList.size() - 1;

	// Get the entity and set its model matrix
	MyEntity* entity = GetEntity(index);
	if (entity != nullptr)
	{
		entity->SetModelMatrix(a_m4ToWorld);
	}
}


#pragma endregion

//The big 3
MyEntityManager::MyEntityManager(){Init();}
MyEntityManager::MyEntityManager(MyEntityManager const& other){ }
MyEntityManager& MyEntityManager::operator=(MyEntityManager const& other) { return *this; }
MyEntityManager::~MyEntityManager(){Release();};


// other methods
void Simplex::MyEntityManager::Update(void)
{
	// Clear existing collisions
	for (int i = 0; i < m_entityList.size(); i++)
	{
		MyEntity* this_entity = GetEntity(i);

		this_entity->GetRigidBody()->ClearCollidingList();
	}

	// Check for collsions
	for (int i = 0; i < m_entityList.size(); i++)
	{
		MyEntity* this_entity = GetEntity(i);
		
		for (int j = i + 1; j < m_entityList.size(); j++)
		{
			MyEntity* other_entity = GetEntity(j);

			if (this_entity->IsColliding(other_entity))
			{
				this_entity->GetRigidBody()->AddCollisionWith(other_entity->GetRigidBody());
			}
		}
	}
}


void Simplex::MyEntityManager::AddEntity(String a_sFileName, String a_sUniqueID)
{
	MyEntity* newEntity = new MyEntity(a_sFileName, a_sUniqueID);
	m_entityList.push_back(newEntity);
}


void Simplex::MyEntityManager::RemoveEntity(uint a_uIndex)
{
	// Compute the index, if positive do nothing. If "negative", get last index.
	uint index = a_uIndex;
	if (index == (-1))
		index = m_entityList.size() - 1;

	// Get the entity and set its model matrix
	MyEntity* entity = GetEntity(index);
	if (entity != nullptr)
	{
		delete entity;
		m_entityList.erase(m_entityList.begin()+index);
	}
}


void Simplex::MyEntityManager::RemoveEntity(String a_sUniqueID)
{
	// Convert unique ID to index
	int index = GetEntityIndex(a_sUniqueID);

	if (index != (-1))
	{
		RemoveEntity(index);
	}
}


/*
	Gets the uniqueID name of the entity indexed
	if a_uIndex < 0, get the last entity in the list
*/
String Simplex::MyEntityManager::GetUniqueID(uint a_uIndex)
{
	// Compute the index, if positive do nothing. If "negative", get last index.
	uint index = a_uIndex;
	if (index == (-1))
		index = m_entityList.size() - 1;


	// Get the entity and return its unique ID
	MyEntity* entity = GetEntity(index);
	if (entity != nullptr)
	{
		return entity->GetUniqueID();
	}

	return "";
}


/*
	Gets the entity indexed
	if a_uIndex < 0, get the last entity in the list
*/
MyEntity* Simplex::MyEntityManager::GetEntity(uint a_uIndex)
{
	if (a_uIndex == (-1) || a_uIndex >= m_entityList.size())
		return nullptr;
	else
		return m_entityList[a_uIndex];
}


/*
	Will add the specified entity to the render list
	if a_uIndex < 0 will add all
*/
void Simplex::MyEntityManager::AddEntityToRenderList(uint a_uIndex, bool a_bRigidBody)
{
	// Compute the index, if positive do nothing. If "negative", get last index.
	uint index = a_uIndex;
	if (index == (-1))
	{
		for (int i = 0; i < m_entityList.size(); i++)
		{
			// Get the entity and add it to the render list
			MyEntity* entity = GetEntity(i);
			if (entity != nullptr)
			{
				entity->AddToRenderList(a_bRigidBody);
			}
		}
	}
	else
	{
		// Get the entity and add it to the render list
		MyEntity* entity = GetEntity(index);
		if (entity != nullptr)
		{
			entity->AddToRenderList(a_bRigidBody);
		}
	}
}


/*
	Will add the specified entity to the render list
*/
void Simplex::MyEntityManager::AddEntityToRenderList(String a_sUniqueID, bool a_bRigidBody)
{
	// Convert unique ID to index
	int index = GetEntityIndex(a_sUniqueID);

	if (index != (-1))
	{
		AddEntityToRenderList(index, a_bRigidBody);
	}
}