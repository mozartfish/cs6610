#version 330 core 

layout(location = 0) in vec3 cubePos;

uniform mat4 mvp;

out vec3 dir;
void main() 
{
	gl_Position = mvp * vec4(0.15 * cubePos, 1);
	dir = cubePos.xyz;
}