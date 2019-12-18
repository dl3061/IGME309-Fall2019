#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUpward(
		vector3(0.0f, 0.0f, 100.0f), //Position
		vector3(0.0f, 0.0f, 99.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

	uint uInstances = 100;

	// Octree
	m_OctreeDepth = 2;
	m_Octree = new MyOctree(m_OctreeDepth, ZERO_V3, vector3(35.0f, 35.0f, 35.0f));
	m_Octree->SetMeshManager(m_pMeshMngr);
	m_pEntityMngr->m_Octree = m_Octree;

	// Set bounds
	m_CubeLimits = 35.0f;

	// Create objects
	for (int i = 0; i < uInstances; i++)
	{
		switch ((i % 10))
		{
		case(0):
			m_pEntityMngr->AddEntity("Planets\\00_Sun.obj");
			break;
		case(1):
			m_pEntityMngr->AddEntity("Planets\\01_Mercury.obj");
			break;
		case(2):
			m_pEntityMngr->AddEntity("Planets\\02_Venus.obj");
			break;
		case(3):
			m_pEntityMngr->AddEntity("Planets\\03_Earth.obj");
			break;
		case(4):
			m_pEntityMngr->AddEntity("Planets\\04_Mars.obj");
			break;
		case(5):
			m_pEntityMngr->AddEntity("Planets\\05_Jupiter.obj");
			break;
		case(6):
			m_pEntityMngr->AddEntity("Planets\\06_Saturn.obj");
			break;
		case(7):
			m_pEntityMngr->AddEntity("Planets\\07_Uranus.obj");
			break;
		case(8):
			m_pEntityMngr->AddEntity("Planets\\08_Neptune.obj");
			break;
		case(9):
			m_pEntityMngr->AddEntity("Planets\\09_Pluto.obj");
			break;
		default:
			m_pEntityMngr->AddEntity("Minecraft\\Cube.obj");
		}
			

		vector3 v3Position = vector3(glm::sphericalRand(m_CubeLimits / 3.0f));
		matrix4 m4Position = glm::translate(v3Position);
		float scale = 1.0 + (i % 10) * .3;
		// scale = scale / 2;
		matrix4 m4Scale = glm::scale(vector3(scale, scale, scale));
		m_pEntityMngr->SetModelMatrix(m4Position * m4Scale);
	}
	
	// Set initial velocity
	for (uint i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	{
		MyEntity* entity = m_pEntityMngr->GetEntity(i);
		entity->UsePhysicsSolver();
		entity->GetSolver()->SetGravity(ZERO_V3);

		// Set a random speed
		float speed = 0.050f;
		speed = speed * (rand() % 10);
		vector3 velocity = vector3(glm::sphericalRand(speed));
		entity->SetVelocity(velocity);

		// Set a random mess
		float mass = 1.0 + (rand() % 50);
		entity->SetMass(mass);
		
		// Set system
		entity->SetSystem(m_pSystem);
	}


	m_pEntityMngr->Update();
}


void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
	
	// Update OctTree every second
	static uint nClock = m_pSystem->GenClock();
	static bool bStarted = false;
	if (m_pSystem->IsTimerDone(nClock) || !bStarted)
	{
		bStarted = true;
		m_Octree->RegenerateOctree();
		m_pSystem->StartTimerOnClock(.1, nClock);
	}

	if (m_OctreeDrawLines)
		m_Octree->DisplayGrid();


	// Update Entity Manager
	m_pEntityMngr->Update();

	// Check collisions
	for (uint i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	{
		MyEntity* entity = m_pEntityMngr->GetEntity(i);

		/*
		// Calc total momentum absolute value
			vector3 totalMomentum = ZERO_V3;

			// Calc total kinectic energy (ratio may be off)
			float totalKE = 0.0f;

			// Include our own
			totalMomentum = entity->GetMass() * entity->GetVelocity();
			totalKE = entity->GetMass() * entity->GetVelocity().length * entity->GetVelocity().length;

			// Include momentum of collision partners
			for (uint j = 0; entity->GetRigidBody()->GetCollidingCount(); j++)
			{
				MyEntity* other = entity->GetRigidBody()->GetCollisionList()[j]->GetEntity();

				totalMomentum = totalMomentum + other->GetMass() * other->GetVelocity();
				totalKE += other->GetMass() * other->GetVelocity().length * other->GetVelocity().length;
			}
		*/

		if (entity->GetRigidBody()->GetCollidingCount() > 0 && (entity->GetJustResolvedCollision() == false))
		{
			// Resolve collision
			MyEntity* entityOther = (MyEntity*) entity->GetRigidBody()->GetCollisionList()[0]->GetEntity();

			if ((entityOther->GetJustResolvedCollision() == false))
			{

				// Get initial velocities and masses
				float m1 = entity->GetMass();
				float m2 = entityOther->GetMass();

				vector3 u1 = entity->GetVelocity();
				vector3 u2 = entityOther->GetVelocity();

				// Convert initial velocities to relative velocities
				vector3 v1 = u1 - u2;
				vector3 v2 = u2 - u2;

				// Calculate final relative velocity
				vector3 V1 = ((m1 - m2) * v1) / (m1 + m2);
				vector3 V2 = ((2.0 * m1) * v1) / (m1 + m2);

				// Convert back to absolute velocities
				/*
				vector3 U1 = (((m1 - m2) * (u1 - u2)) / (m1 + m2)) + u2;
				vector3 U2 = (((2.0 * m1) * (u1 - u2)) / (m1 + m2)) + u2;
				*/
				vector3 U1 = V1 + u2;
				vector3 U2 = V2 + u2;

				// Set the velocities
				entity->SetVelocity(U1);
				entityOther->SetVelocity(U2);

				// Clear collision
				int i = 0;
				while (i < 1000)
				{
					// Speed up these two a bit until they're not touching and retriggering this code area
					entity->GetRigidBody()->ClearCollidingList();
					entityOther->GetRigidBody()->ClearCollidingList();
					entity->Update();
					entityOther->Update();

					// Break case
					if (!entity->IsColliding(entityOther))
						break;

					// Escape just in case
					i++;
				}

				// entity->GetRigidBody()->ClearCollidingList();
				// entityOther->GetRigidBody()->ClearCollidingList();
				entity->SetJustResolvedCollision(true);
				entityOther->SetJustResolvedCollision(true);
			}
		}
		else
		{
			vector3 velocity = entity->GetVelocity();
			float scale = 1.0;

			// Check boundaries
			if (entity->GetPosition().x + scale > m_CubeLimits)
			{
				// Make sure velocity is pointed in
				if (velocity.x > 0)
				{
					velocity.x = (-1.0f) * abs(velocity.x);
				}
			}

			if (entity->GetPosition().x - scale < (-m_CubeLimits))
			{
				// Make sure velocity is pointed in
				if (velocity.x < 0)
				{
					velocity.x = abs(velocity.x);
				}
			}

			// Check boundaries
			if (entity->GetPosition().y + scale > m_CubeLimits)
			{
				// Make sure velocity is pointed in
				if (velocity.y > 0)
				{
					velocity.y = (-1.0f) * abs(velocity.y);
				}

			}

			if (entity->GetPosition().y - scale < (-m_CubeLimits))
			{
				// Make sure velocity is pointed in
				if (velocity.y < 0)
				{
					velocity.y = abs(velocity.y);
				}
			}

			// Check boundaries
			if (entity->GetPosition().z + scale > m_CubeLimits)
			{
				// Make sure velocity is pointed in
				if (velocity.z > 0)
				{
					velocity.z = (-1.0f) * abs(velocity.z);
				}
			}

			if (entity->GetPosition().z - scale < (-m_CubeLimits))
			{
				// Make sure velocity is pointed in
				if (velocity.z < 0)
				{
					velocity.z = abs(velocity.z);
				}
			}

			entity->SetVelocity(velocity);
		}
		

		// Update
		m_pEntityMngr->GetEntity(i)->Update();
	}

	// Check for GUI information
	m_TotalMomentum = 0.0f;
	m_TotalKE = 0.0f;
	for (uint i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	{
		MyEntity* entity = m_pEntityMngr->GetEntity(i);
		m_TotalMomentum += entity->GetMass() * entity->GetVelocity().length();
		m_TotalKE += entity->GetMass() * entity->GetVelocity().length() * entity->GetVelocity().length();
	}

	// Draw outer grid
	// Horizontal lines
	m_pMeshMngr->AddLineToRenderList(IDENTITY_M4,
		vector3(-m_CubeLimits, -m_CubeLimits,-m_CubeLimits),
		vector3(m_CubeLimits, -m_CubeLimits, -m_CubeLimits),
		C_YELLOW, C_YELLOW);

	m_pMeshMngr->AddLineToRenderList(IDENTITY_M4,
		vector3(-m_CubeLimits, -m_CubeLimits, m_CubeLimits),
		vector3(m_CubeLimits, -m_CubeLimits, m_CubeLimits),
		C_YELLOW, C_YELLOW);

	m_pMeshMngr->AddLineToRenderList(IDENTITY_M4,
		vector3(-m_CubeLimits, m_CubeLimits, -m_CubeLimits),
		vector3(m_CubeLimits, m_CubeLimits, -m_CubeLimits),
		C_YELLOW, C_YELLOW);

	m_pMeshMngr->AddLineToRenderList(IDENTITY_M4,
		vector3(-m_CubeLimits, m_CubeLimits, m_CubeLimits),
		vector3(m_CubeLimits, m_CubeLimits, m_CubeLimits),
		C_YELLOW, C_YELLOW);

	// Vertical lines
	m_pMeshMngr->AddLineToRenderList(IDENTITY_M4,
		vector3(-m_CubeLimits, -m_CubeLimits, -m_CubeLimits),
		vector3(-m_CubeLimits, m_CubeLimits, -m_CubeLimits),
		C_YELLOW, C_YELLOW);

	m_pMeshMngr->AddLineToRenderList(IDENTITY_M4,
		vector3(-m_CubeLimits, -m_CubeLimits, m_CubeLimits),
		vector3(-m_CubeLimits, m_CubeLimits, m_CubeLimits),
		C_YELLOW, C_YELLOW);

	m_pMeshMngr->AddLineToRenderList(IDENTITY_M4,
		vector3(m_CubeLimits, -m_CubeLimits, -m_CubeLimits),
		vector3(m_CubeLimits, m_CubeLimits, -m_CubeLimits),
		C_YELLOW, C_YELLOW);

	m_pMeshMngr->AddLineToRenderList(IDENTITY_M4,
		vector3(m_CubeLimits, -m_CubeLimits, m_CubeLimits),
		vector3(m_CubeLimits, m_CubeLimits, m_CubeLimits),
		C_YELLOW, C_YELLOW);

	// Coming at you lines
	m_pMeshMngr->AddLineToRenderList(IDENTITY_M4,
		vector3(-m_CubeLimits, -m_CubeLimits, -m_CubeLimits),
		vector3(-m_CubeLimits, -m_CubeLimits, m_CubeLimits),
		C_YELLOW, C_YELLOW);

	m_pMeshMngr->AddLineToRenderList(IDENTITY_M4,
		vector3(-m_CubeLimits, m_CubeLimits, -m_CubeLimits),
		vector3(-m_CubeLimits, m_CubeLimits, m_CubeLimits),
		C_YELLOW, C_YELLOW);

	m_pMeshMngr->AddLineToRenderList(IDENTITY_M4,
		vector3(m_CubeLimits, -m_CubeLimits, -m_CubeLimits),
		vector3(m_CubeLimits, -m_CubeLimits, m_CubeLimits),
		C_YELLOW, C_YELLOW);

	m_pMeshMngr->AddLineToRenderList(IDENTITY_M4,
		vector3(m_CubeLimits, m_CubeLimits, -m_CubeLimits),
		vector3(m_CubeLimits, m_CubeLimits, m_CubeLimits),
		C_YELLOW, C_YELLOW);


	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	//display octree
	/*
	if (m_uOctantID == -1)
		m_pRoot->Display();
	else
		m_pRoot->Display(m_uOctantID);
		*/
	
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui,
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release GUI
	ShutdownGUI();

	// Safe delete
	if (m_Octree != nullptr)
	{
		delete m_Octree;
		m_Octree = nullptr;
	}
}