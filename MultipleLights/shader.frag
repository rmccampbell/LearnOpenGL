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

	float constant;
	float linear;
	float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
	vec3 direction;
	float innerCutoff;
	float outerCutoff;

	float constant;
	float linear;
	float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define N_POINT_LIGHTS 4

uniform vec3 viewPos;
uniform Material material;

uniform DirLight dirLight;
uniform PointLight pointLights[N_POINT_LIGHTS];
uniform SpotLight spotLight;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
	vec3 diffColor = texture(material.diffuse, TexCoords).rgb;
    vec3 ambient  = light.ambient  * diffColor;
    vec3 diffuse  = light.diffuse  * diff * diffColor;
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;
    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDisp = light.position - fragPos;
	float lightDist = length(lightDisp);
	vec3 lightDir = lightDisp / lightDist;
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// attenuation
	float attenuation = 1 / (light.constant + light.linear*lightDist + light.quadratic*lightDist*lightDist);
    // combine results
	vec3 diffColor = texture(material.diffuse, TexCoords).rgb;
    vec3 ambient  = light.ambient  * diffColor;
    vec3 diffuse  = light.diffuse  * diff * diffColor;
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;
    return (ambient + diffuse + specular) * attenuation;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDisp = light.position - fragPos;
	float lightDist = length(lightDisp);
	vec3 lightDir = lightDisp / lightDist;
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// attenuation
	float attenuation = 1 / (light.constant + light.linear*lightDist + light.quadratic*lightDist*lightDist);
	// spotlight intensity
	float theta = dot(lightDir, normalize(-light.direction));
	float intensity = smoothstep(light.outerCutoff, light.innerCutoff, theta);
    // combine results
	vec3 diffColor = texture(material.diffuse, TexCoords).rgb;
    vec3 ambient  = light.ambient  * diffColor;
    vec3 diffuse  = light.diffuse  * diff * diffColor;
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;
    return (ambient + diffuse + specular) * attenuation * intensity;
}

void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	vec3 color = CalcDirLight(dirLight, norm, viewDir);
	for (int i = 0; i < N_POINT_LIGHTS; i++)
		color += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
	color += CalcSpotLight(spotLight, norm, FragPos, viewDir);

    FragColor = vec4(color, 1.0);
}
