#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 Texcoord;

uniform vec3 objectColor;
uniform vec3 cameraPos;

struct Light {
    vec3 position;  
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
}; 

struct Material {
    vec3 ambient;
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

uniform Light light;  
uniform Material material;

out vec4 FragColor;

void main()
{
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
    		    light.quadratic * (distance * distance)); 

    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, Texcoord).rgb;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    //vec3 diffuse = light.diffuse * (diff * material.diffuse);
    vec3 diffuse = light.diffuse * (diff * texture(material.diffuse, Texcoord).rgb);

    // specular
    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * texture(material.specular, Texcoord).rgb);

    ambient  *= attenuation; 
    diffuse  *= attenuation;
    specular *= attenuation; 

    // combine
    vec3 result = ambient + diffuse + specular;
    result *= objectColor; // apply object color

    FragColor = vec4(result, 1.0);
}

