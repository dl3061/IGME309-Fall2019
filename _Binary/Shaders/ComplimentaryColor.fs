#version 330

in vec3 Color;

uniform vec3 SolidColor = vec3(-1,-1,-1);
uniform bool SwapToComplimentary = false;

out vec4 Fragment;

void main()
{
	// Default Fragment
	Fragment = vec4(Color, 1);

	// Check if value of the uniform SolidColor was modified/set. If so, use that. 
	if(SolidColor.r != -1.0 && SolidColor.g != -1.0 && SolidColor.b != -1.0)
	{
		Fragment = vec4(SolidColor, 1);
	}
	
	// Check if swapping
	if (SwapToComplimentary)
	{
		vec3 ComplFragmentColor = vec3(1.0 - Fragment.x, 1.0 - Fragment.y, 1.0 - Fragment.z);
		Fragment = vec4(ComplFragmentColor, 1);
	}

	return;
}