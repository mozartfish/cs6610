#version 330 core

// Input Vectors
in vec3 normalVector;
in vec4 intersect;
in vec3 lightPosVector;

// Output Vectors
layout(location = 0) out vec4 color;

// Texture

// Light Position
vec3 lightPosition = lightPosVector;

// Vector Definitions 
vec3 w = normalize(lightPosition - intersect.xyz); // direction vector where the light hits the scene
vec3 n = normalize(normalVector); // normal vector
vec3 v = normalize(-intersect.xyz); // view space vector
vec3 h = normalize(w + n); // half vector

// Lighting Variables
vec3 I = vec3(1.0f, 1.0f, 1.0f); // light intensity
vec3 I_a = vec3(1.0f, 1.0f, 1.0f); // ambient light intensity
vec3 kd = vec3(1.0f, 0.0f, 0.0f); // surface color / diffuse constant
vec3 ks = vec3(1.0f, 1.0f, 1.0f); // specular constant
vec3 ka = kd; // ambient constant
float cosTheta = dot(n, w);
float cosPhi = dot(n, h);
float alpha = 50.0f;

// Blinn Shading Components
vec3 ambient = ka;
vec3 diffuse = max(0, cosTheta)*kd;
vec3 specular = ks * pow(max(0, cosPhi), alpha);
vec3 blinn = I * (diffuse + specular) + I_a * ambient;

void main() {

color = vec4(blinn, 1); // blinn shading
//color = vec4(normalVector, 1); // normal shading
//color = vec4(1, 0, 0, 1); // red 
}