#version 420

uniform sampler2D uTex;

in vec2 texcoord;

out vec3 outColor;

void main()
{
	
	outColor = texture(uTex, texcoord).rgb;
}