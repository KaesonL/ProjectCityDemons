#version 420

uniform sampler2D uTex;
uniform float uMax;//maxHP
uniform float uHP;//current
uniform int uPlayer;//player
uniform bool uGrey;//grey bar

in vec2 texcoord;

out vec4 outColor;

void main()
{
	//decide which way to scale from
	float pos;
	if(uPlayer == 0)
		pos = texcoord.x;
	else
		pos = 1 - texcoord.x;
	
	
	if(pos > (uHP/uMax)){
		outColor = vec4(0,0,0,0);
	}
	else{
		float depth = texture(uTex, texcoord).r;
		outColor.a = texture(uTex, texcoord).a;
		
		if(uGrey == true)
			outColor.rgb = vec3(0.9f + depth,depth,depth);
		else{
			outColor.rgb = vec3(1 - uHP/uMax + ((1-pos) * 0.5f) + depth, sqrt(1.0f - ((1-pos) * 0.5f)) + depth, depth);
		}
	}
}