#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

struct Material {
	vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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

#define MAX_LIGHTS 10

uniform vec3 viewPos;
uniform Material material;

uniform int numDirLights, numPointLights, numSpotLights;
uniform DirLight dirLights[MAX_LIGHTS];
uniform PointLight pointLights[MAX_LIGHTS];
uniform SpotLight spotLights[MAX_LIGHTS];

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * material.ambient;
    vec3 diffuse  = light.diffuse  * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
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
    vec3 ambient  = light.ambient  * material.ambient;
    vec3 diffuse  = light.diffuse  * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
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
	float spotCos = dot(lightDir, normalize(-light.direction));
	float spotIntensity = smoothstep(light.outerCutoff, light.innerCutoff, spotCos);
    // combine results
    vec3 ambient  = light.ambient  * material.ambient;
    vec3 diffuse  = light.diffuse  * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    return (ambient + diffuse + specular) * attenuation * spotIntensity;
}

void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 color = vec3(0);

	for (int i = 0; i < numDirLights; i++)
		color += CalcDirLight(dirLights[i], norm, viewDir);
	for (int i = 0; i < numPointLights; i++)
		color += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
	for (int i = 0; i < numSpotLights; i++)
		color += CalcSpotLight(spotLights[i], norm, FragPos, viewDir);

    FragColor = vec4(color, 1.0);
}
