#version 330 core 

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normalPos;
layout(location = 2) in vec2 txc;

out vec3 normalVector;
out vec3 lightPosVector;
out vec4 intersect;
out vec2 texCoord;

uniform mat4 mvp;
uniform mat4 mvn;
uniform mat4 mv;
uniform vec3 lightPos;


void main() 
{
	gl_Position = mvp * vec4(0.05 * pos, 1);
	normalVector = mat3(mvn) * normalPos;
	intersect = mv * vec4(pos, 1);
	lightPosVector = lightPos;
	texCoord = txc;
}