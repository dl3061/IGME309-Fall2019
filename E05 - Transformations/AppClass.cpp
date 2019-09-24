#include "AppClass.h"
void Application::InitVariables(void)
{
	//Make the Space Invader
	m_SpaceInvader = new SpaceInvader(1.0f, vector3(-1.0, 0, 0));
	m_SpaceInvader->SetMotion(Motion::Sine);

	// Make an army
	m_SpaceInvaderArmy = std::vector<SpaceInvader*>();
	for (int row = 0; row <= 5; row++)
	{
		float invaderSize = 0.1f; // +0.01f * row;
		float invaderScale = 0.5f + 0.1f * row;
		float invaderPosY = INVADER_ARMY_POS_INIT_Y - INVADER_ARMY_POS_SEP_Y * row;

		for (int col = 0; col <= 11; col++)
		{
			float invaderPosX = INVADER_ARMY_POS_INIT_X + INVADER_ARMY_POS_SEP_X * col;
			SpaceInvader* invader = new SpaceInvader(invaderSize, vector3(invaderPosX, invaderPosY, 0.0f));
			invader->SetMotion(Motion::Classic);
			invader->SetScale(invaderScale);

			m_SpaceInvaderArmy.push_back(invader);
		}
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

	// Get the Projection Matrix and View Matrix out of the camera
	matrix4 viewMatrix = m_pCameraMngr->GetViewMatrix();
	matrix4 projMatrix = m_pCameraMngr->GetProjectionMatrix();
	matrix4 arcbMatrix = ToMatrix4(m_qArcBall);

	// Display the Space Invader's meshes
	m_SpaceInvader->Render(projMatrix, viewMatrix, arcbMatrix);

	for (int i = 0; i < m_SpaceInvaderArmy.size(); i++)
	{
		m_SpaceInvaderArmy[i]->Render(projMatrix, viewMatrix, arcbMatrix);
	}
		
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
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
	if (m_SpaceInvader != nullptr)
	{
		delete m_SpaceInvader;
	}

	//release GUI
	ShutdownGUI();
}