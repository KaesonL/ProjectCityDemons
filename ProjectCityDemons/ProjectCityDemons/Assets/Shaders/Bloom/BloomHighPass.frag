#version 420

uniform sampler2D uTex; //Read the source image
uniform float uThreshold;
uniform float uThreshold2;
//uniform bool bloomOn;

in vec2 texcoord;

out vec3 outColor;

void main()
{
    vec3 color = texture(uTex, texcoord).rgb;

    float luminance = abs((color.r - color.g) + (color.r - color.b) + (color.g - color.b)) / 3.0;
    float luminance2 = (color.r + color.g + color.b) /3.0;

    if (luminance < uThreshold && luminance2 > uThreshold2 /*&& bloomOn*/)
    {
        outColor = color;
    }
    else
    {
        outColor = vec3(0.0, 0.0, 0.0);
    }
}