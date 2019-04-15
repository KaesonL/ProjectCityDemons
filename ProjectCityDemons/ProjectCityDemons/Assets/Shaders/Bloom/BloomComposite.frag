#version 420

uniform sampler2D uScene;
uniform sampler2D uBloom;

in vec2 texcoord;

out vec3 outColor;

void main()
{
	vec3 colorA = texture(uScene, texcoord).rgb;
	vec3 colorB = texture(uBloom, texcoord).rgb;
	//If one of the inputs is black and then other isnt, you will get the one that is not black. So that is good (and racist :P)
	outColor = 1.0 - (1.0 - colorA) * (1.0 - colorB);
}