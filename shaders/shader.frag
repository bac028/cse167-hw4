#version 330 core
// This is a sample fragment shader.

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct PointLight {
    vec3 position;
    vec3 color;
};

struct DirectionalLight {
    vec3 direction;
    vec3 color;
};


in vec4 vertexColor;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float materialShininess;

uniform vec3 viewPos;
uniform vec3 pointLightColor;
uniform vec3 pointLightPosition;
uniform vec3 dirLightColor;
uniform vec3 dirLightDirection;

uniform vec3 color;
uniform float mode;
uniform float dirEnabled;
uniform float pointEnabled;

out vec4 fragColor;

vec3 calcDirLight(DirectionalLight light, Material material, vec3 normal, vec3 viewDir) {

    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // combine results
    vec3 ambient = material.ambient;
    vec3 diffuse = diff * material.diffuse;
    vec3 specular = spec * material.specular;
    return light.color * (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, Material material, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (1.0f * distance);

    // combine results
    vec3 ambient = light.color * material.ambient;
    vec3 diffuse = light.color * diff * material.diffuse;
    vec3 specular = light.color * spec * material.specular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

void main()
{
    // normal mode
    if (mode == 0.0) {
        fragColor = vertexColor;
    } 
    
    // phong mode
    else if (mode == 1.0) {
        vec3 norm = normalize(Normal);
        vec3 viewDir = normalize(viewPos - FragPos);

        Material material = {materialAmbient, materialDiffuse, materialSpecular, materialShininess};

        DirectionalLight dirLight = {dirLightDirection, dirLightColor};
        PointLight pointLight = {pointLightPosition, pointLightColor};

        vec3 result = vec3(0,0,0);

        // adds directional light
        if (dirEnabled == 1.0f) result += calcDirLight(dirLight, material, norm, viewDir);

        // adds point light
        if (pointEnabled == 1.0f) result += calcPointLight(pointLight, material, norm, FragPos, viewDir);

        fragColor = vec4(result, 1.0f);
    }

    // show point light
    else if (mode == 2.0) {
        fragColor = vec4(color, 1.0);
    }
}
