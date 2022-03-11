#version 330 core

// Input Vectors
in vec3 normalVector;
in vec4 intersect;
in vec3 lightPosVector;
in vec2 texCoord;
in vec2 sptexCoord;

// Output Vectors
layout(location = 0) out vec4 color;

// Texture
uniform sampler2D tex;
uniform sampler2D sptex;

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
float cosTheta = dot(n, w);
float cosPhi = dot(n, h);
float alpha = 50.0f;
// float alpha = 5.0f; // TESTING ONLY

// Shading Constants
//vec3 kd = vec3(1.0f, 0.0f, 0.0f); // surface color / diffuse constant
vec3 kd = vec3(1.0f, 1.0f, 1.0f); // surface color / diffuse constant - TESTING ONLY
vec3 kd_texture = texture(tex, texCoord).rgb; // diffuse texture constant
vec3 ks = vec3(1.0f, 1.0f, 1.0f); // specular constant
vec3 ks_texture = texture(sptex, sptexCoord).rgb; // specular texture constant
vec3 ka = kd; // ambient constant
vec3 ka_texture = kd_texture; // ambient texture constant

// Shading Components
vec3 ambient = ka;
vec3 diffuse = max(0, cosTheta)*kd;
vec3 specular = ks * pow(max(0, cosPhi), alpha);

// Texture Components
vec3 diffuse_texture = diffuse * kd_texture; // diffuse texture shading
vec3 specular_texture = specular * ks_texture; // specular texture shading


// Blinn Shading Components
vec3 blinn = I * (diffuse + specular) + I_a * ambient; // Blinn Shading
vec3 blinn_texture = I * (diffuse_texture + specular_texture); // Blinn Shading Textures

void main() {
color = vec4(blinn_texture, 1); // blinn shading with textures
//color = vec4(specular_texture, 1); // specular texture
//color = vec4(diffuse_texture, 1); // diffuse texture 
//color = vec4(blinn, 1); // blinn shading
//color = vec4(normalVector, 1); // normal shading
//color = vec4(1, 0, 0, 1); // red 
}