#version 330 core

layout(location = 0) out vec4 color;

in vec4 mvpos;
in vec3 mvnpos;


void main() {	

// Define Light Location 
vec3 lightPosition = vec3(0.0f, 50.0f, 0.0f);

// Define omega (direction to the light source)
vec3 w = normalize(lightPosition - mvpos.xyz);
vec3 n = normalize(mvnpos);

// Define variables for lighting
float I = 0.5f; // light intensity
vec3 kd = vec3(1.0f, 0.0f, 0.0f); // surface color
vec3 ks = vec3(1.0f, 1.0f, 1.0f); // specular constant
float cosTheta = dot(n, w);

// Diffuse Lighting
vec3 diffuse = I * cosTheta * kd;

// Blinn Shading
vec3 v = normalize(-mvpos.xyz);
vec3 h = normalize(w + v);
float cosPhi = dot(v, h);
float alpha = 5.0f;

vec3 blinn = ks * pow(max(0.0f, cosPhi), alpha);






color = vec4(blinn, 1);
}