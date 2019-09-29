#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}


void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}


MyMesh::MyMesh()
{
	Init();
}


MyMesh::~MyMesh() { Release(); }


MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}


MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}


void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}


void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}


void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}


void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}


void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}


void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}


void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}


void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
		// AddVertexPosition(a_vBottomLeft);
		// AddVertexPosition(a_vBottomRight);
		// AddVertexPosition(a_vTopLeft);
	AddTri(a_vBottomLeft, a_vBottomRight, a_vTopLeft);

		// AddVertexPosition(a_vTopLeft);
		// AddVertexPosition(a_vBottomRight);
		// AddVertexPosition(a_vTopRight);
	AddTri(a_vTopLeft, a_vBottomRight, a_vTopRight);
}


void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}


void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}


void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// -------------------------------

	// Calculate the degrees per division divided among a_nSubdivisions
	float degreesPerDivision = 360.0 / a_nSubdivisions;

	// Calculate the points of the cone (centered at origin). 
		// Tip of cone 1/2 a_fHeight above
		// Base of cone is 1/2 a_fHeight below
		// Radius is... a_fRadius
	float halfHeight = a_fHeight / 2.0f;
	vector3 coneTip = vector3(0.0f, halfHeight, 0.0f);
	vector3 baseCenter = vector3(0.0f, -halfHeight, 0.0f);

	// Calculate and add the points around the radius of the base. 
		// Calculate as if it was a unit circle on the xz plane, starting on the positive x-axis, 
	vector3 outerPoint = vector3 (0.0f, -halfHeight, 0.0f);
	vector3 outerPointNext = vector3(0.0f, -halfHeight, 0.0f);

	// Following code is reworked from E04 - Circle Creation
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		float currDegrees = degreesPerDivision * i;
		float nextDegrees = degreesPerDivision * (i + 1);

		// "Calc" outerPoint
			// outerPoint starts at zero degrees for the first division.
			// It then is always the previous divisions outerPointNext, so don't re-calcculate. 
		if (i == 0)
		{
			outerPoint.x = a_fRadius;
			outerPoint.z = 0;
		}
		else
		{
			outerPoint = outerPointNext;
		}

		// Calc outerPointNext (the next point in the triangle, counter-clockwise, or a few degrees along the unit circle.)
			// If it's the last division, it should go back to 360degrees or 0 degrees.
		if (i == a_nSubdivisions - 1)
		{
			// Rounding errors can leave a degree or few sliced out of the circle.
			//	Just snap to the 0-degrees on the unit circle. 
			outerPointNext.x = a_fRadius;
			outerPointNext.z = 0;
		}
		else
		{
			outerPointNext.x = a_fRadius * cos(nextDegrees * (PI / 180));
			outerPointNext.z = - a_fRadius * sin(nextDegrees * (PI / 180));
				// I was wondering why my shapes were inverted...
					// I had the pos/neg z axis confused so what I thought was going counterclockwise was actually clockwise.
					// Add negative to go counterclockwise (looking top-down onto the shape). 
		}

		// Add the side of the cone
		AddTri(outerPoint, outerPointNext, coneTip);
		
		// Add the part of the base
			// Reverse the order since we're looking from bottom
		AddTri(baseCenter, outerPointNext, outerPoint);
	}

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}


void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// -------------------------------

	// Calculate the degrees per division divided among a_nSubdivisions
	float degreesPerDivision = 360.0 / a_nSubdivisions;

	// Calculate the points of the cone (centered at origin). 
		// Tip of cylinder 1/2 a_fHeight above
		// Base of cylinder is 1/2 a_fHeight below
		// Radius is... a_fRadius
	float halfHeight = a_fHeight / 2.0f;
	vector3 ceilOffset = vector3(0.0f, halfHeight, 0.0f);
	vector3 baseOffset = vector3(0.0f, -halfHeight, 0.0f);

	vector3 ceilCenter = vector3(0.0f, 0.0f, 0.0f) + ceilOffset;
	vector3 baseCenter = vector3(0.0f, 0.0f, 0.0f) + baseOffset;

	// Calculate and add the points around the radius of the base. 
		// Calculate as if it was a unit circle on the xz plane, starting on the positive x-axis, 
	vector3 outerPoint = vector3(0.0f, 0.0f, 0.0f);
	vector3 outerPointNext = vector3(0.0f, 0.0f, 0.0f);

	// Following code is reworked from E04 - Circle Creation
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		float currDegrees = degreesPerDivision * i;
		float nextDegrees = degreesPerDivision * (i + 1);

		// "Calc" outerPoint
			// outerPoint starts at zero degrees for the first division.
			// It then is always the previous divisions outerPointNext, so don't re-calcculate. 
		if (i == 0)
		{
			outerPoint.x = a_fRadius;
			outerPoint.z = 0;
		}
		else
		{
			outerPoint = outerPointNext;
		}

		// Calc outerPointNext (the next point in the triangle, counter-clockwise, or a few degrees along the unit circle.)
			// If it's the last division, it should go back to 360degrees or 0 degrees.
		if (i == a_nSubdivisions - 1)
		{
			// Rounding errors can leave a degree or few sliced out of the circle.
			//	Just snap to the 0-degrees on the unit circle. 
			outerPointNext.x = a_fRadius;
			outerPointNext.z = 0;
		}
		else
		{
			outerPointNext.x = a_fRadius * cos(nextDegrees * (PI / 180));
			outerPointNext.z = - a_fRadius * sin(nextDegrees * (PI / 180));
		}

		// Sanity check
		outerPoint.y = 0.0f;
		outerPointNext.y = 0.0f;

		// Add the side of the cylinder
	
		AddQuad(outerPoint + baseOffset,
			outerPointNext + baseOffset,
			outerPoint + ceilOffset,
			outerPointNext + ceilOffset);
			
		/*
		AddQuad(
			outerPointNext + baseOffset,
			outerPoint + baseOffset,
			outerPointNext + ceilOffset,
			outerPoint + ceilOffset);
		*/

		// Add the upper base / ceiling
		AddTri(outerPoint + ceilOffset, outerPointNext + ceilOffset, ceilCenter);

		// Add the bottom base
		AddTri(baseCenter, outerPointNext + baseOffset, outerPoint + baseOffset);
	}

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}


void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// -------------------------------

	// Calculate the degrees per division divided among a_nSubdivisions
	float degreesPerDivision = 360.0 / a_nSubdivisions;

	// Calculate the points of the cone (centered at origin). 
		// Tip of cylinder 1/2 a_fHeight above
		// Base of cylinder is 1/2 a_fHeight below
		// Radius is... a_fRadius
	float halfHeight = a_fHeight / 2.0f;
	vector3 ceilOffset = vector3(0.0f, halfHeight, 0.0f);
	vector3 baseOffset = vector3(0.0f, -halfHeight, 0.0f);

	vector3 ceilCenter = vector3(0.0f, 0.0f, 0.0f) + ceilOffset;
	vector3 baseCenter = vector3(0.0f, 0.0f, 0.0f) + baseOffset;

	// Calculate and add the points around the outer and inner radius of the base. 
		// Calculate as if it was a unit circle on the xz plane, starting on the positive x-axis, 
	vector3 outerPoint = vector3(0.0f, 0.0f, 0.0f);
	vector3 outerPointNext = vector3(0.0f, 0.0f, 0.0f);
	vector3 innerPoint = vector3(0.0f, 0.0f, 0.0f);
	vector3 innerPointNext = vector3(0.0f, 0.0f, 0.0f);

	// Following code is reworked from E04 - Circle Creation
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		float currDegrees = degreesPerDivision * i;
		float nextDegrees = degreesPerDivision * (i + 1);

		// "Calc" outerPoint
			// outerPoint starts at zero degrees for the first division.
			// It then is always the previous divisions outerPointNext, so don't re-calcculate. 
		if (i == 0)
		{
			outerPoint.x = a_fOuterRadius;
			outerPoint.z = 0;
			innerPoint.x = a_fInnerRadius;
			innerPoint.z = 0;
		}
		else
		{
			outerPoint = outerPointNext;
			innerPoint = innerPointNext;
		}

		// Calc outerPointNext (the next point in the triangle, counter-clockwise, or a few degrees along the unit circle.)
			// If it's the last division, it should go back to 360degrees or 0 degrees.
		if (i == a_nSubdivisions - 1)
		{
			// Rounding errors can leave a degree or few sliced out of the circle.
			//	Just snap to the 0-degrees on the unit circle. 
			outerPointNext.x = a_fOuterRadius;
			outerPointNext.z = 0;

			innerPointNext.x = a_fInnerRadius;
			innerPointNext.z = 0;
		}
		else
		{
			outerPointNext.x = a_fOuterRadius * cos(nextDegrees * (PI / 180));
			outerPointNext.z = -a_fOuterRadius * sin(nextDegrees * (PI / 180));

			innerPointNext.x = a_fInnerRadius * cos(nextDegrees * (PI / 180));
			innerPointNext.z = -a_fInnerRadius * sin(nextDegrees * (PI / 180));
		}

		// Add the outer side of the tube
		AddQuad(outerPoint + baseOffset,
			outerPointNext + baseOffset,
			outerPoint + ceilOffset,
			outerPointNext + ceilOffset);

		// Add the inner side of the tube
		AddQuad(
			innerPointNext + baseOffset,
			innerPoint + baseOffset,
			innerPointNext + ceilOffset,
			innerPoint + ceilOffset);

		// Add the upper base / ceiling
		AddQuad(outerPoint + ceilOffset, 
			outerPointNext + ceilOffset, 
			innerPoint + ceilOffset, 
			innerPointNext + ceilOffset);

		// Add the bottom base
		AddQuad(innerPoint + baseOffset,
			innerPointNext + baseOffset,
			outerPoint + baseOffset, 
			outerPointNext + baseOffset);
	}

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}


void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// Replace this with your code
	GenerateCube(a_fOuterRadius * 2.0f, a_v3Color);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}


void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();

	// Replace this with your code
	GenerateCube(a_fRadius * 2.0f, a_v3Color);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}

