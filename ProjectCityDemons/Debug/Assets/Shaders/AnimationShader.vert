#version 420

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

layout(location = 0) in vec3 in_vert;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec3 in_vert2;
layout(location = 4) in vec2 in_uv2;
layout(location = 5) in vec3 in_normal2;

uniform float interp = 0.5;
//uniform int index = 0; 

out vec2 texcoord;
out vec3 norm;
out vec3 pos;

vec3 catmull(vec3 p0, vec3 p1, vec3 p2, vec3 p3, float t)
{
	return 0.5f * (
		t * t * t * (-p0 + 3.0f * p1 - 3.0f * p2 + p3) +
		t * t * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) +
		t * (-p0 + p2) +
		(2.0f * p1)		);
}

vec4 catmull(vec4 p0, vec4 p1, vec4 p2, vec4 p3, float t)
{
	return 0.5f * (
		t * t * t * (-p0 + 3.0f * p1 - 3.0f * p2 + p3) +
		t * t * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) +
		t * (-p0 + p2) +
		(2.0f * p1)		);
}

void main()
{

	vec2 interpUV;
	interpUV = mix(in_uv, in_uv2, interp);
	
	texcoord = interpUV;
	
	//normals
	vec3 interpNorm;
	interpNorm = mix(in_normal, in_normal2, interp);
	
	norm = mat3(uView) * mat3(uModel) * interpNorm;
	
	//vertex
	vec3 lerp; 
	lerp = mix(in_vert, in_vert2, interp);
		
	pos = (uView * uModel * vec4(lerp, 1.0f)).xyz;

	gl_Position = uProj * vec4(pos, 1.0);
}
