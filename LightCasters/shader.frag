#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct SpotLight {
    vec3 position;
	vec3 direction;
	float innerCutoff;
	float outerCutoff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

uniform Material material;
uniform SpotLight light;
uniform vec3 viewPos;

void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDisp = light.position - FragPos;
	float lightDist = length(lightDisp);
	vec3 lightDir = lightDisp / lightDist;

	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
	vec3 color = ambient;

	float theta = dot(lightDir, -light.direction);
	if (theta > light.outerCutoff) {
		float attenuation = smoothstep(light.outerCutoff, light.innerCutoff, theta);
		attenuation /= (light.constant + light.linear*lightDist + light.quadratic*lightDist*lightDist);

		float diff = max(dot(norm, lightDir), 0.0);

		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

		vec3 diffuse = light.diffuse * attenuation * diff * texture(material.diffuse, TexCoords).rgb;
		vec3 specular = light.specular * attenuation * spec * texture(material.specular, TexCoords).rgb;
		color += diffuse + specular;
	}

    FragColor = vec4(color, 1.0);
}
