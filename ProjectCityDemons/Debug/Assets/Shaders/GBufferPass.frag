#version 420

uniform sampler2D uTex;

in vec2 texcoord;
in vec3 norm;
in vec3 pos;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outPosition;

void main()
{
	//Standard color output
	//outColor.rgb = texture(uTex, texcoord).rgb;
	//outColor.a = 1.0;
	outColor = texture(uTex, texcoord);

	//Pack normals
	//in -> [-1, 1]
	//out -> [0, 1]
	outNormal = normalize(norm) * 0.5 + 0.5;

	//View space positions. No need to pack!
	outPosition = pos;
}