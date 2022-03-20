float cosTheta = dot(n, w);
float cosPhi = dot(n, h);
//float alpha = 50.0f;
float alpha = 5.0f; // TESTING ONLY

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
vec3 blinn_texture = I * (diffuse_texture + specular_texture); // Blinn Shading Textures
vec3 blinn = I * (diffuse + specular) + I_a * ambient; // Blinn Shading

void main() {
color = vec4(blinn_texture, 1); // blinn shading with textures
//color = vec4(specular_texture, 1); // specular texture
//color = vec4(diffuse_texture, 1); // diffuse texture 
//color = vec4(blinn, 1); // blinn shading
//color = vec4(specular, 1); // specular shading
//color = vec4(diffuse, 1); // diffuse shading
//color = vec4(normalVector, 1); // normal shading
//color = vec4(1, 0, 0, 1); // red 
}