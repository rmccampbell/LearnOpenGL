#version 330 core
out vec4 FragColor;

in vec3 LightingColor;
in vec2 TexCoord;

uniform sampler2D colorTexture;

void main()
{
	vec3 result = LightingColor * texture(colorTexture, TexCoord).rgb;
    FragColor = vec4(result, 1.0);
}
