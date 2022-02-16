#version 330 core 

// Position Attribute
layout(location = 0) in vec3 pos;

// Normal Position Attribute
layout(location = 1) in vec3 normalPos;

uniform mat4 mvp; // model view projection matrix
uniform mat4 mvn; // model view normal matrix 
uniform mat4 mv; // model view matrix

out vec4 mvpos;   // model view vector
out vec3 mvnpos; // model view normal vector


void main() 
{
	gl_Position = mvp * vec4(0.05 * pos, 1);
	mvpos = mv * vec4(pos, 1);	 // position vector 
	mvnpos = mat3(mvn) * normalPos; // normal vector
}