#version 420

uniform mat4 uModel; //The pose of the object to be rendered
uniform mat4 uView; //The position and orientation of the camera
uniform mat4 uProj; //Camera Lens

layout(location = 0) in vec3 in_vert;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal;

out vec2 texcoord;
out vec3 norm;
out vec3 pos;

void main()
{
	texcoord = in_uv;
	norm = mat3(uView) * mat3(uModel) * in_normal;

	//vec4 viewSpace = uView * uModel * vec4(in_vert, 1.0f);

	//gl_Position = uProj * viewSpace;

	//pos = viewSpace.xyz;
	
	pos = (uView * uModel * vec4(in_vert, 1.0f)).xyz;
	
	gl_Position = uProj * vec4(pos, 1.0f);
	
	
}