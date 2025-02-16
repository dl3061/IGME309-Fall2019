#include "MyRigidBody.h"
using namespace Simplex;
//Allocation


void MyRigidBody::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_bVisibleBS = true;
	m_bVisibleOBB = true;
	m_bVisibleARBB = false;

	m_fRadius = 0.0f;

	m_v3ColorColliding = C_RED;
	m_v3ColorNotColliding = C_WHITE;

	m_v3Center = ZERO_V3;
	m_v3MinL = ZERO_V3;
	m_v3MaxL = ZERO_V3;

	m_v3MinG = ZERO_V3;
	m_v3MaxG = ZERO_V3;

	m_v3HalfWidth = ZERO_V3;
	m_v3ARBBSize = ZERO_V3;

	m_m4ToWorld = IDENTITY_M4;
}
void MyRigidBody::Swap(MyRigidBody& a_pOther)
{
	std::swap(m_pMeshMngr, a_pOther.m_pMeshMngr);
	std::swap(m_bVisibleBS, a_pOther.m_bVisibleBS);
	std::swap(m_bVisibleOBB, a_pOther.m_bVisibleOBB);
	std::swap(m_bVisibleARBB, a_pOther.m_bVisibleARBB);

	std::swap(m_fRadius, a_pOther.m_fRadius);

	std::swap(m_v3ColorColliding, a_pOther.m_v3ColorColliding);
	std::swap(m_v3ColorNotColliding, a_pOther.m_v3ColorNotColliding);

	std::swap(m_v3Center, a_pOther.m_v3Center);
	std::swap(m_v3MinL, a_pOther.m_v3MinL);
	std::swap(m_v3MaxL, a_pOther.m_v3MaxL);

	std::swap(m_v3MinG, a_pOther.m_v3MinG);
	std::swap(m_v3MaxG, a_pOther.m_v3MaxG);

	std::swap(m_v3HalfWidth, a_pOther.m_v3HalfWidth);
	std::swap(m_v3ARBBSize, a_pOther.m_v3ARBBSize);

	std::swap(m_m4ToWorld, a_pOther.m_m4ToWorld);

	std::swap(m_CollidingRBSet, a_pOther.m_CollidingRBSet);
}
void MyRigidBody::Release(void)
{
	m_pMeshMngr = nullptr;
	ClearCollidingList();
}
//Accessors
bool MyRigidBody::GetVisibleBS(void) { return m_bVisibleBS; }
void MyRigidBody::SetVisibleBS(bool a_bVisible) { m_bVisibleBS = a_bVisible; }
bool MyRigidBody::GetVisibleOBB(void) { return m_bVisibleOBB; }
void MyRigidBody::SetVisibleOBB(bool a_bVisible) { m_bVisibleOBB = a_bVisible; }
bool MyRigidBody::GetVisibleARBB(void) { return m_bVisibleARBB; }
void MyRigidBody::SetVisibleARBB(bool a_bVisible) { m_bVisibleARBB = a_bVisible; }
float MyRigidBody::GetRadius(void) { return m_fRadius; }
vector3 MyRigidBody::GetColorColliding(void) { return m_v3ColorColliding; }
vector3 MyRigidBody::GetColorNotColliding(void) { return m_v3ColorNotColliding; }
void MyRigidBody::SetColorColliding(vector3 a_v3Color) { m_v3ColorColliding = a_v3Color; }
void MyRigidBody::SetColorNotColliding(vector3 a_v3Color) { m_v3ColorNotColliding = a_v3Color; }
vector3 MyRigidBody::GetCenterLocal(void) { return m_v3Center; }
vector3 MyRigidBody::GetMinLocal(void) { return m_v3MinL; }
vector3 MyRigidBody::GetMaxLocal(void) { return m_v3MaxL; }
vector3 MyRigidBody::GetCenterGlobal(void){	return vector3(m_m4ToWorld * vector4(m_v3Center, 1.0f)); }
vector3 MyRigidBody::GetMinGlobal(void) { return m_v3MinG; }
vector3 MyRigidBody::GetMaxGlobal(void) { return m_v3MaxG; }
vector3 MyRigidBody::GetHalfWidth(void) { return m_v3HalfWidth; }
matrix4 MyRigidBody::GetModelMatrix(void) { return m_m4ToWorld; }
void MyRigidBody::SetModelMatrix(matrix4 a_m4ModelMatrix)
{
	//to save some calculations if the model matrix is the same there is nothing to do here
	if (a_m4ModelMatrix == m_m4ToWorld)
		return;

	//Assign the model matrix
	m_m4ToWorld = a_m4ModelMatrix;

	//Calculate the 8 corners of the cube
	vector3 v3Corner[8];
	//Back square
	v3Corner[0] = m_v3MinL;
	v3Corner[1] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MinL.z);
	v3Corner[2] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MinL.z);
	v3Corner[3] = vector3(m_v3MaxL.x, m_v3MaxL.y, m_v3MinL.z);

	//Front square
	v3Corner[4] = vector3(m_v3MinL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[5] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[6] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MaxL.z);
	v3Corner[7] = m_v3MaxL;

	//Place them in world space
	for (uint uIndex = 0; uIndex < 8; ++uIndex)
	{
		v3Corner[uIndex] = vector3(m_m4ToWorld * vector4(v3Corner[uIndex], 1.0f));
	}

	//Identify the max and min as the first corner
	m_v3MaxG = m_v3MinG = v3Corner[0];

	//get the new max and min for the global box
	for (uint i = 1; i < 8; ++i)
	{
		if (m_v3MaxG.x < v3Corner[i].x) m_v3MaxG.x = v3Corner[i].x;
		else if (m_v3MinG.x > v3Corner[i].x) m_v3MinG.x = v3Corner[i].x;

		if (m_v3MaxG.y < v3Corner[i].y) m_v3MaxG.y = v3Corner[i].y;
		else if (m_v3MinG.y > v3Corner[i].y) m_v3MinG.y = v3Corner[i].y;

		if (m_v3MaxG.z < v3Corner[i].z) m_v3MaxG.z = v3Corner[i].z;
		else if (m_v3MinG.z > v3Corner[i].z) m_v3MinG.z = v3Corner[i].z;
	}

	//we calculate the distance between min and max vectors
	m_v3ARBBSize = m_v3MaxG - m_v3MinG;
}
vector3 Simplex::MyRigidBody::GetLastSATAxis(void)
{
	return m_lastSATaxis;
}
//The big 3
MyRigidBody::MyRigidBody(std::vector<vector3> a_pointList)
{
	Init();
	//Count the points of the incoming list
	uint uVertexCount = a_pointList.size();

	//If there are none just return, we have no information to create the BS from
	if (uVertexCount == 0)
		return;

	//Max and min as the first vector of the list
	m_v3MaxL = m_v3MinL = a_pointList[0];

	//Get the max and min out of the list
	for (uint i = 1; i < uVertexCount; ++i)
	{
		if (m_v3MaxL.x < a_pointList[i].x) m_v3MaxL.x = a_pointList[i].x;
		else if (m_v3MinL.x > a_pointList[i].x) m_v3MinL.x = a_pointList[i].x;

		if (m_v3MaxL.y < a_pointList[i].y) m_v3MaxL.y = a_pointList[i].y;
		else if (m_v3MinL.y > a_pointList[i].y) m_v3MinL.y = a_pointList[i].y;

		if (m_v3MaxL.z < a_pointList[i].z) m_v3MaxL.z = a_pointList[i].z;
		else if (m_v3MinL.z > a_pointList[i].z) m_v3MinL.z = a_pointList[i].z;
	}

	//with model matrix being the identity, local and global are the same
	m_v3MinG = m_v3MinL;
	m_v3MaxG = m_v3MaxL;

	//with the max and the min we calculate the center
	m_v3Center = (m_v3MaxL + m_v3MinL) / 2.0f;

	//we calculate the distance between min and max vectors
	m_v3HalfWidth = (m_v3MaxL - m_v3MinL) / 2.0f;

	//Get the distance between the center and either the min or the max
	m_fRadius = glm::distance(m_v3Center, m_v3MinL);
}
MyRigidBody::MyRigidBody(MyRigidBody const& a_pOther)
{
	m_pMeshMngr = a_pOther.m_pMeshMngr;

	m_bVisibleBS = a_pOther.m_bVisibleBS;
	m_bVisibleOBB = a_pOther.m_bVisibleOBB;
	m_bVisibleARBB = a_pOther.m_bVisibleARBB;

	m_fRadius = a_pOther.m_fRadius;

	m_v3ColorColliding = a_pOther.m_v3ColorColliding;
	m_v3ColorNotColliding = a_pOther.m_v3ColorNotColliding;

	m_v3Center = a_pOther.m_v3Center;
	m_v3MinL = a_pOther.m_v3MinL;
	m_v3MaxL = a_pOther.m_v3MaxL;

	m_v3MinG = a_pOther.m_v3MinG;
	m_v3MaxG = a_pOther.m_v3MaxG;

	m_v3HalfWidth = a_pOther.m_v3HalfWidth;
	m_v3ARBBSize = a_pOther.m_v3ARBBSize;

	m_m4ToWorld = a_pOther.m_m4ToWorld;

	m_CollidingRBSet = a_pOther.m_CollidingRBSet;
}
MyRigidBody& MyRigidBody::operator=(MyRigidBody const& a_pOther)
{
	if (this != &a_pOther)
	{
		Release();
		Init();
		MyRigidBody temp(a_pOther);
		Swap(temp);
	}
	return *this;
}
MyRigidBody::~MyRigidBody() { Release(); };
//--- a_pOther Methods
void MyRigidBody::AddCollisionWith(MyRigidBody* a_pOther)
{
	/*
		check if the object is already in the colliding set, if
		the object is already there return with no changes
	*/
	auto element = m_CollidingRBSet.find(a_pOther);
	if (element != m_CollidingRBSet.end())
		return;
	// we couldn't find the object so add it
	m_CollidingRBSet.insert(a_pOther);
}
void MyRigidBody::RemoveCollisionWith(MyRigidBody* a_pOther)
{
	m_CollidingRBSet.erase(a_pOther);
}
void MyRigidBody::ClearCollidingList(void)
{
	m_CollidingRBSet.clear();
}
bool MyRigidBody::IsColliding(MyRigidBody* const a_pOther)
{
	//check if spheres are colliding as pre-test
	bool bColliding = (glm::distance(GetCenterGlobal(), a_pOther->GetCenterGlobal()) < m_fRadius + a_pOther->m_fRadius);

	// Run it always
	uint SAT_result = SAT(a_pOther) != eSATResults::SAT_NONE;
	
	//if they are colliding check the SAT
	if (bColliding)
	{
		if (SAT_result != eSATResults::SAT_NONE)
			bColliding = false;// reset to false
	}

	if (bColliding) //they are colliding
	{
		this->AddCollisionWith(a_pOther);
		a_pOther->AddCollisionWith(this);
	}
	else //they are not colliding
	{
		this->RemoveCollisionWith(a_pOther);
		a_pOther->RemoveCollisionWith(this);
	}

	return bColliding;
}
void MyRigidBody::AddToRenderList(void)
{
	if (m_bVisibleBS)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
		else
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
	}

	if (m_bVisibleOBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorColliding);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorNotColliding);
	}

	if (m_bVisibleARBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
	}
}

uint MyRigidBody::SAT(MyRigidBody* const a_pOther)
{

	// Get the axises
	vector3 Ax, Ay, Az;
	{
		Ax = vector3(m_m4ToWorld * vector4(vector3(1.0, 0.0, 0.0) + m_v3Center, 1.0f));
		Ay = vector3(m_m4ToWorld * vector4(vector3(0.0, 1.0, 0.0) + m_v3Center, 1.0f));
		Az = vector3(m_m4ToWorld * vector4(vector3(0.0, 0.0, 1.0) + m_v3Center, 1.0f));

		Ax = Ax - vector3(m_m4ToWorld * vector4(m_v3Center, 1.0f));
		Ay = Ay - vector3(m_m4ToWorld * vector4(m_v3Center, 1.0f));
		Az = Az - vector3(m_m4ToWorld * vector4(m_v3Center, 1.0f));
	}

	vector3 Bx, By, Bz;
	{
		Bx = vector3(a_pOther->m_m4ToWorld * vector4(vector3(1.0, 0.0, 0.0) + a_pOther->m_v3Center, 1.0f));
		By = vector3(a_pOther->m_m4ToWorld * vector4(vector3(0.0, 1.0, 0.0) + a_pOther->m_v3Center, 1.0f));
		Bz = vector3(a_pOther->m_m4ToWorld * vector4(vector3(0.0, 0.0, 1.0) + a_pOther->m_v3Center, 1.0f));

		Bx = Bx - vector3(a_pOther->m_m4ToWorld * vector4(a_pOther->m_v3Center, 1.0f));
		By = By - vector3(a_pOther->m_m4ToWorld * vector4(a_pOther->m_v3Center, 1.0f));
		Bz = Bz - vector3(a_pOther->m_m4ToWorld * vector4(a_pOther->m_v3Center, 1.0f));
	}


	// Get the verticies of this rigidbody (in world space)
	vector3 A_v3Min = m_v3MinG;
	vector3 A_v3Max = m_v3MaxG;

	// Get the verticies of the other rigidbody (in world space)
	vector3 B_v3Min = a_pOther->m_v3MinG;
	vector3 B_v3Max = a_pOther->m_v3MaxG;

	// Check if they are colliding against Ax, Ay and Az
	if (IsThereSeparationOnAxis(a_pOther, Ax))
		return eSATResults::SAT_AX;

	if (IsThereSeparationOnAxis(a_pOther, Ay))
		return eSATResults::SAT_AY;

	if (IsThereSeparationOnAxis(a_pOther, Az))
		return eSATResults::SAT_AZ;

	// Check if they are colliding against Bx, By and Bz
	if (IsThereSeparationOnAxis(a_pOther, Bx))
		return eSATResults::SAT_BX;

	if (IsThereSeparationOnAxis(a_pOther, By))
		return eSATResults::SAT_BY;

	if (IsThereSeparationOnAxis(a_pOther, Bz))
		return eSATResults::SAT_BZ;

	// Now check for each cross product

	// Ax cross Bx to Bz
	if (IsThereSeparationOnAxis(a_pOther, glm::cross(Ax, Bx)))
		return eSATResults::SAT_AXxBX;

	if (IsThereSeparationOnAxis(a_pOther, glm::cross(Ax, By)))
		return eSATResults::SAT_AXxBY;

	if (IsThereSeparationOnAxis(a_pOther, glm::cross(Ax, Bz)))
		return eSATResults::SAT_AXxBZ;

	// Ay cross Bx to Bz
	if (IsThereSeparationOnAxis(a_pOther, glm::cross(Ay, Bx)))
		return eSATResults::SAT_AYxBX;

	if (IsThereSeparationOnAxis(a_pOther, glm::cross(Ay, By)))
		return eSATResults::SAT_AYxBY;

	if (IsThereSeparationOnAxis(a_pOther, glm::cross(Ay, Bz)))
		return eSATResults::SAT_AYxBZ;

	// Az cross Bx to Bz
	if (IsThereSeparationOnAxis(a_pOther, glm::cross(Az, Bx)))
		return eSATResults::SAT_AZxBX;

	if (IsThereSeparationOnAxis(a_pOther, glm::cross(Az, By)))
		return eSATResults::SAT_AZxBY;

	if (IsThereSeparationOnAxis(a_pOther, glm::cross(Az, Bz)))
		return eSATResults::SAT_AZxBZ;

	//there is no axis test that separates this two objects
	m_lastSATaxis = ZERO_V3;
	return eSATResults::SAT_NONE;
}


uint MyRigidBody::IsThereSeparationOnAxis(MyRigidBody* const a_pOther, vector3 axis)
{
	// If axis isn't an actual axis, return false
	if (axis.length() == 0.0 || axis == vector3(0.0, 0.0, 0.0))
	{
		return 0;
	}

	// Calculate the 3-dimensional corners of the first rigidbody A
	vector3 A_v3Corner[8];
	{
		// Verticies calculation adapted from the provided MyRigidBody::SetModelMatrix(matrix4 a_m4ModelMatrix);

		//Back square
		A_v3Corner[0] = m_v3MinL;
		A_v3Corner[1] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MinL.z);
		A_v3Corner[2] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MinL.z);
		A_v3Corner[3] = vector3(m_v3MaxL.x, m_v3MaxL.y, m_v3MinL.z);

		//Front square
		A_v3Corner[4] = vector3(m_v3MinL.x, m_v3MinL.y, m_v3MaxL.z);
		A_v3Corner[5] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MaxL.z);
		A_v3Corner[6] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MaxL.z);
		A_v3Corner[7] = m_v3MaxL;

		//Place them in world space
		for (uint uIndex = 0; uIndex < 8; ++uIndex)
		{
			A_v3Corner[uIndex] = vector3(m_m4ToWorld * vector4(A_v3Corner[uIndex], 1.0f));
		}
	}

	// Calculate the 3-dimensional corners of the other rigidbody B
	vector3 B_v3Corner[8];
	{
		// // Verticies calculation adapted from the provided MyRigidBody::SetModelMatrix(matrix4 a_m4ModelMatrix);

		//Back square
		B_v3Corner[0] = a_pOther->m_v3MinL;
		B_v3Corner[1] = vector3(a_pOther->m_v3MaxL.x, a_pOther->m_v3MinL.y, a_pOther->m_v3MinL.z);
		B_v3Corner[2] = vector3(a_pOther->m_v3MinL.x, a_pOther->m_v3MaxL.y, a_pOther->m_v3MinL.z);
		B_v3Corner[3] = vector3(a_pOther->m_v3MaxL.x, a_pOther->m_v3MaxL.y, a_pOther->m_v3MinL.z);

		//Front square
		B_v3Corner[4] = vector3(a_pOther->m_v3MinL.x, a_pOther->m_v3MinL.y, a_pOther->m_v3MaxL.z);
		B_v3Corner[5] = vector3(a_pOther->m_v3MaxL.x, a_pOther->m_v3MinL.y, a_pOther->m_v3MaxL.z);
		B_v3Corner[6] = vector3(a_pOther->m_v3MinL.x, a_pOther->m_v3MaxL.y, a_pOther->m_v3MaxL.z);
		B_v3Corner[7] = a_pOther->m_v3MaxL;

		//Place them in world space
		for (uint uIndex = 0; uIndex < 8; uIndex++)
		{
			B_v3Corner[uIndex] = vector3(a_pOther->m_m4ToWorld * vector4(B_v3Corner[uIndex], 1.0f));
		}
	}

	// Calculate and compare their projections onto the axis
	float A_min = 0;
	float A_max = 0;
	float B_min = 0;
	float B_max = 0;

	// Calculate min and max of projections for A
	for (uint i = 0; i < 8; i++)
	{
		float scalerProj = glm::dot(A_v3Corner[i], axis) / glm::length(axis);

		if ((i == 0) || scalerProj < A_min)
		{
			A_min = scalerProj;
		}
		if ((i == 0) || scalerProj > A_max)
		{
			A_max = scalerProj;
		}
	}

	// Calculate min and max of projections for A
	for (uint i = 0; i < 8; i++)
	{
		float scalerProj = glm::dot(B_v3Corner[i], axis) / glm::length(axis);

		if ((i == 0) || scalerProj < B_min)
		{
			B_min = scalerProj;
		}
		if ((i == 0) || scalerProj > B_max)
		{
			B_max = scalerProj;
		}
	}

	// Check for overlap
	if ((B_min > A_min && B_min < A_max) ||
		(B_max > A_min && B_max < A_max) ||
		(A_min > B_min && A_min < B_max) ||
		(A_max > B_min && A_max < B_max)
		)
	{
		return 0;
	}
	else
	{
		// No overlap at all -> there is separation
		m_lastSATaxis = axis;
		return 1;
	}
}