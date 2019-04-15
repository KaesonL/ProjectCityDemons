#version 420

//Specified as array because the incoming types might be differnt? check Week 9 video time: 44:00 for clarification
in float size[];
in float alpha[];
//Input type
layout (points) in;
//Output type
layout (triangle_strip, max_vertices = 4) out; //You write a triangle and the next vertex you create is linked with the previous two to create the next triangle
//Explanation at Week 9 time: 44:50 ^

//Output data to fragment
out vec2 texcoord;
out float frag_alpha;

uniform mat4 uProj;

void main()
{
	vec3 position = gl_in[0].gl_Position.xyz;

	//First three blocks will generate the first triangle, the last three will generate the second triangle because we are using a strip

	frag_alpha = alpha[0];
	texcoord = vec2(0.0, 0.0); //We don't need texture coordinates to come from GPU memory, we can generate it here :D
	gl_Position = uProj * vec4(position.xy + vec2(-0.5, -0.5) * size[0], position.z, 1.0);
	//Registers the vertex as ready to go down the pipeline
	EmitVertex();

	frag_alpha = alpha[0]; //Does not need to be here but is left here to be more explicit...?
	texcoord = vec2(1.0, 0.0);
	gl_Position = uProj * vec4(position.xy + vec2(0.5, -0.5) * size[0], position.z, 1.0);
	EmitVertex();

	frag_alpha = alpha[0];
	texcoord = vec2(0.0, 1.0);
	gl_Position = uProj * vec4(position.xy + vec2(-0.5, 0.5) * size[0], position.z, 1.0);
	EmitVertex();

	frag_alpha = alpha[0];
	texcoord = vec2(1.0, 1.0);
	gl_Position = uProj * vec4(position.xy + vec2(0.5, 0.5) * size[0], position.z, 1.0);
	EmitVertex();

	EndPrimitive();
}