#version 420

uniform sampler2D uTex;

in vec2 texcoord;
in float frag_alpha;

out vec4 outColor;

void main()
{
	outColor = texture(uTex, texcoord).rgba;
	outColor.a *= frag_alpha; //Explained at Week 9 time: 53:30 (maybe earlier)
}