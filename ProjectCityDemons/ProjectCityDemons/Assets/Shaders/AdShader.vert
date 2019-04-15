#version 420

uniform mat4 uModel; //The pose of the object to be rendered
uniform mat4 uView; //The position and orientation of the camera
uniform mat4 uProj; //Camera Lens
uniform float uTime;//time

layout(location = 0) in vec3 in_vert;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal;

// 'out' variables will be sent forward in the pipeline to the fragment shader.
out vec2 texcoord;
out vec3 norm;
out vec3 pos;

void main()
{
	texcoord = in_uv;
	norm = mat3(uView) * mat3(uModel) * in_normal;
	pos = (uView * uModel * vec4(in_vert, 1.0f)).xyz;
	
	//texcoord.y = 1 - texcoord.y;
	texcoord.y -= uTime / 10;
	//texcoord.y += cos((-uTime / 2) + texcoord.y) * 0.3;

	//float offsetY = cos((in_vert.x * 5.0f) + uTime) * 0.05;
	// Complete the transformations on our modified position.
	gl_Position = uProj * vec4(pos, 1.0f);
}
