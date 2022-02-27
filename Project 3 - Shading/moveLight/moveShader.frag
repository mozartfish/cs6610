#version 330 core

layout(location = 0) out vec4 color;

in vec3 normalVector;
in vec4 intersect;
in vec3 lightPosVector;

void main() {
// Define Point Light Position
//vec3 lightPosition = vec3(0.0f, 5.0f, 50.0f);
vec3 lightPosition = lightPosVector;

// Vector definitions
vec3 w = normalize(lightPosition - intersect.xyz);
vec3 n = normalize(normalVector);
vec3 v = normalize(-intersect.xyz);
vec3 h = normalize(w + n);


// Define variables for lighting
float I = 1.0f; // light intensity
float I_a = 0.1f;
vec3 kd = vec3(1.0f, 0.0f, 0.0f); // surface color
vec3 ks = vec3(1.0f, 1.0f, 1.0f); // specular constant
vec3 ka = kd; // diffuse constant
float cosTheta = dot(n, w);
float cosPhi = dot(n, h);
float alpha = 50.0f;

// Diffuse Lighting
//vec3 diffuse = I * cosTheta * kd + I;

// Blinn Model
vec3 ambient = ka;
vec3 diffuse = max(0, cosTheta) * kd;
vec3 specular = ks * pow(max(0, cosPhi), alpha);
vec3 blinn = I * (diffuse + specular) + I_a * ambient;

color = vec4(blinn, 1);
}