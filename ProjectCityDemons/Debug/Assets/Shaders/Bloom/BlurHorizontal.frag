#version 420

uniform sampler2D uTex; //Read the source image
uniform float uPixelSize; //1.0 / window_width

in vec2 texcoord;

out vec3 outColor;

void main()
{
	//Sample pixels in a horizontal row
	//The weights add up to 1***
	outColor = vec3(0.0, 0.0, 0.0);

	//The more you do this the more the blur there is?
	outColor += texture(uTex, vec2(texcoord.x - 4.0 * uPixelSize, texcoord.y)).rgb * 0.06;
	outColor += texture(uTex, vec2(texcoord.x - 3.0 * uPixelSize, texcoord.y)).rgb * 0.09;
	outColor += texture(uTex, vec2(texcoord.x - 2.0 * uPixelSize, texcoord.y)).rgb * 0.12;
	outColor += texture(uTex, vec2(texcoord.x -		  uPixelSize, texcoord.y)).rgb * 0.15;
	outColor += texture(uTex, vec2(texcoord.x,					  texcoord.y)).rgb * 0.16;
	outColor += texture(uTex, vec2(texcoord.x +       uPixelSize, texcoord.y)).rgb * 0.15;
	outColor += texture(uTex, vec2(texcoord.x + 2.0 * uPixelSize, texcoord.y)).rgb * 0.12;
	outColor += texture(uTex, vec2(texcoord.x + 3.0 * uPixelSize, texcoord.y)).rgb * 0.09;
	outColor += texture(uTex, vec2(texcoord.x + 4.0 * uPixelSize, texcoord.y)).rgb * 0.06;
}