#version 330 core

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emmision;
	float emmisionStr;
	float shininess;
};

struct Light{
	vec3 position;
	bool type;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	float constant;
	float linear;
	float quadratic;
};
#define NR_POINT_LIGHTS 1000


uniform Material material;
uniform Light light[NR_POINT_LIGHTS];
uniform int n_lights;

out vec4 FragColor;

uniform vec3 viewPos;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir);  

void main()
{
	int i;
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 result = vec3(0.0f);
	vec2 p = TexCoords.xy;
	for (i = 0; i < n_lights; i++)
	{
		result += CalcPointLight(light[i], norm, FragPos, viewDir);
	}
	
	float gamma = 2.2f;
	FragColor.rgb = pow(FragColor.rgb, vec3(1.0f/gamma));
	
	
	FragColor = vec4(result, texture(material.diffuse, TexCoords).a);
}

vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	float distance;
	float spec = 0;
	vec3 specular = vec3(0.0f);
	vec3 lightDir, halfwayDir, reflectDir;
	int blinn = 0;
	
	if (light.type)
	{
		lightDir = normalize(light.position);
	}
	else
	{
		lightDir = normalize(light.position - FragPos);
	}
	distance = length(light.position - FragPos);
	
	float diff = max(dot(normal, lightDir), 0.0f);
	
	halfwayDir = normalize(lightDir + viewDir);
	reflectDir = reflect(-lightDir, normal);

	if (material.shininess > 0)
	{
		if (blinn == 1)
		{
			spec = pow(max(dot(viewDir, halfwayDir), 0.0f), material.shininess);
		}
		else
		{
			spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
		}
		specular = light.specular * spec * texture(material.specular, TexCoords).rgb;
	}
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords)) ;
	vec3 emmision = texture(material.emmision, TexCoords).rgb;
	
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	
	return (ambient + diffuse + specular + (emmision * material.emmisionStr));
	
}
