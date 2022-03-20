#version 330 core

in vec2 UV;

layout(location = 0) out vec4 color;

uniform sampler2D renderTexture;

void main() {
	//color = vec4(1.0f, 0.0f, 1.0f, 1);
	color = vec4(texture(renderTexture, UV).rgb, 1.0f);	
}