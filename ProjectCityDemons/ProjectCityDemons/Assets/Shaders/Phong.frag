#version 420

uniform vec4 LightPosition;

//Colour
uniform vec3 LightAmbient;
uniform vec3 LightDiffuse;
uniform vec3 LightSpecular;

//Scalars
uniform float LightSpecularExponent;
uniform float Attenuation_Constant;
uniform float Attenuation_Linear;
uniform float Attenuation_Quadratic;

//You can create multiple of these to create multiple slots?
uniform sampler2D uTex;

in vec2 texcoord;
in vec3 norm;
//Position in view space of the particular surface of the model we are working at
in vec3 pos;

out vec4 outColor;

void main()
{	
	//Alpha component is determined by the alpha in the texture
	outColor.rgb = LightAmbient;

	//Account for rasterizer interpolating
	vec3 normal = normalize(norm);

	vec3 lightVec = LightPosition.xyz - pos;	//Full vector
	float dist = length(lightVec);				//Distance of the vector
	vec3 lightDir = lightVec / dist;			//Normalized direction vector

	float NdotL = dot(normal, lightDir);

	if (NdotL > 0.0)
	{
		//The light contributes to this surface

		//Calculate attenuation (falloff)
		float attenuation = 1.0 / (Attenuation_Constant + (Attenuation_Linear * dist) + (Attenuation_Quadratic * dist * dist));

		//Calculate diffuse contribution
		outColor.rgb += LightDiffuse * NdotL * attenuation;

		//Blinn-Phong half vector
		float NdotHV = max(dot(normal, normalize(lightDir + normalize(-pos))), 0.0);

		//Calculate specular contribution
		outColor.rgb += LightSpecular * pow(NdotHV, LightSpecularExponent) * attenuation;
	}

	//First parameter is the slot you want to read from, second is the texture coordinate you to read at
	vec4 textureColor = texture(uTex, texcoord);
	outColor.rgb *= textureColor.rgb;
	outColor.a = textureColor.a;

}