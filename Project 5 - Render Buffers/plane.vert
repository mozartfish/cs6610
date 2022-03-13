#version 330 core 

layout(location = 0) in vec3 plane_pos;

uniform mat4 plane_mvp;

void main() 
{
	gl_Position = plane_mvp * vec4(0.05 * plane_pos, 1);
}