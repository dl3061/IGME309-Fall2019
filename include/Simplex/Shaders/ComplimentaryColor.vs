#version 330

layout (location = 0) in vec3 Position_b;
layout (location = 1) in vec3 Color_b;

out vec3 Color;

void main()
{
	gl_Position = vec4(Position_b, 1);
	
	Color = Color_b;
}