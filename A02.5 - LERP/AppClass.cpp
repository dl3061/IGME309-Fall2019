#include "AppClass.h"
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Alberto Bobadilla - labigm@rit.edu";
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUpward(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}
	
	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = MORBIT_INIT_RADIUS; //initial size of orbits

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - MORBIT_RADIUS_WIDTH, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager
		fSize += MORBIT_RADIUS_INCREMENT; //increment the size for the next orbit
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
	}
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	//Get a timer
	static float fTimer = 0;	//store the new timer
	static uint uClock = m_pSystem->GenClock(); //generate a new clock for that timer
	fTimer += m_pSystem->GetDeltaTime(uClock); //get the delta time for that timer

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	//m4Offset = glm::rotate(IDENTITY_M4, 1.5708f, AXIS_Z);

	// draw a shapes
	for (uint i = 0; i < m_uOrbits; ++i)
	{
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 1.5708f, AXIS_X));

		// Re-calculate the info about the torus

		uint sides = 3 + i;
		float radius = MORBIT_INIT_RADIUS + MORBIT_RADIUS_INCREMENT * i - (MORBIT_RADIUS_WIDTH / 2.0f);
		float degreesPerSide = 360.0f / sides;

		// Calculate the circumference
		float circumference = 0.0f; 
		float circumferencePerSide = 0.0f;
		{
			// The point at 0 degrees
			vector3 tempPointZero = vector3(radius, 0.0f, 0.0f);
			
			// The point at degreesPerSide degrees
			vector3 tempPointOne = vector3();
			tempPointOne.x = radius * cos(degreesPerSide * (PI / 180));
			tempPointOne.y = radius * sin(degreesPerSide * (PI / 180));

			// Get the circumferencePerSide as the difference between the two
			circumferencePerSide = length(tempPointOne - tempPointZero);

			// Get total circumference
			circumference = circumferencePerSide * sides;
		}

		// All orbits move at the same speed.

		// So check between which two sides we are based on time in relation to circumference
		uint lastPointIndex = ((uint) (fTimer / circumferencePerSide)) % sides;
		uint nextPointIndex = (lastPointIndex + 1) % sides;

		// Calc last Point
		vector3 lastPoint = vector3();
		float lastPointDegrees = degreesPerSide * lastPointIndex;
		lastPoint.x = radius * cos(lastPointDegrees * (PI / 180));
		lastPoint.y = radius * sin(lastPointDegrees * (PI / 180));

		// Calc next point
		vector3 nextPoint = vector3();
		float nextPointDegrees = degreesPerSide * nextPointIndex;
		nextPoint.x = radius * cos(nextPointDegrees * (PI / 180));
		nextPoint.y = radius * sin(nextPointDegrees * (PI / 180));

		// Calc the percentage in between points the orbit should be
		float inBetweenDistance = fmod(fTimer, circumferencePerSide) / circumferencePerSide;

		vector3 v3CurrentPos = glm::lerp(lastPoint, nextPoint, inBetweenDistance);
		matrix4 m4Model = glm::translate(m4Offset, v3CurrentPos);

		//draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);
	}

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release GUI
	ShutdownGUI();
}