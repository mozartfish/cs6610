#version 330 core 

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normalPos;

out vec4 normalColor;

uniform mat4 mvp;
uniform mat4 mv;

void main() 
{
	gl_Position = mvp * vec4(0.05 * pos, 1);
	normalColor = normalize(mv * vec4(normalPos, 1));
}