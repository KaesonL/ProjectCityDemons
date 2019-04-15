#version 420

uniform sampler2D uSceneAlbedo;
//uniform sampler2D uShadowMap;
uniform sampler2D uNormalMap;
uniform sampler2D uPositionMap;
uniform sampler2D uEdgeMap;
uniform sampler2D uStepTexture;

//uniform vec3 uSceneDiffuse = vec3(0.0, 1.0, 0.0);
uniform vec3 uLightPosition = vec3(3.0, 0.0, 0.0);
uniform vec3 uLightColor = vec3(0.0, 1.0, 0.0);
uniform float uLightSpecularExponent = 8.0;

uniform bool uToonActive;

in vec2 texcoord;
//in vec3 norm;
//in vec3 pos;

out vec4 outColor;

void main()
{
	vec3 textureColor = texture(uSceneAlbedo, texcoord).rgb;
	vec3 normal = texture(uNormalMap, texcoord).xyz * 2.0 - 1.0; //Unpack
	vec3 pos = texture(uPositionMap, texcoord).xyz;
	
	float edgeFactor = texture(uEdgeMap, texcoord).r; //Will be zero if there is an edge
	
	outColor.rgb = vec3(0.0);
	outColor.a = 0.5;

	// Fix length after rasterizer interpolates
	//vec3 normal = normalize(norm);

	vec3 lightVec = uLightPosition - pos;
	float dist = length(lightVec);
	vec3 lightDir = lightVec / dist;

	float NdotL = dot(normal, lightDir);

	// If the normal is facing the light
	if (NdotL > 0.0)
	{
		// Normalized vector pointing towards the camera
		vec3 eye = normalize(-pos);
		
		// Calculate attenuation (falloff)
		// Add a small number to avoid divide by zero.
		float attenuation = 1.0 / (0.5f + dist * dist * 0.1);

		// Calculate the diffuse contribution
		outColor.rgb += uLightColor * NdotL * attenuation;
		
		vec3 reflection = reflect(-lightDir, normal);
		
		float specularStrength = dot(reflection, eye);
		specularStrength = max(specularStrength, 0.0f); // don't let it fall before zero

		// Calculate the specular contribution
		outColor.rgb += uLightColor * pow(specularStrength, uLightSpecularExponent) * attenuation;
		
		if(uToonActive){
			float blocky = texture(uStepTexture, vec2(outColor.r, 0.5)).r;
			outColor.rgb = vec3(blocky,blocky,blocky) * edgeFactor;
		}
	}
	//outColor.rgb *= 0.001;
	//outColor.rgb += vec3(1,0,0);
	//outColor = max(attenuation * diffuse + specular, uSceneAmbient);
}
