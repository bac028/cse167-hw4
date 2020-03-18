#version 330 core
// This is a sample fragment shader.

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;
    vec3 color;
};

in vec4 vertexColor;
in vec3 Normal;
in vec3 FragPos;

//Tone shading
in vec3 world_pos;
in vec3 world_normal;

uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float materialShininess;

uniform vec3 viewPos;
uniform vec3 dirLightColor;
uniform vec3 dirLightDirection;

uniform float mode;
uniform float dirEnabled;
uniform float pointEnabled;

out vec4 fragColor;

DirectionalLight dirLight = {vec3(-100, -100, -100), vec3(1, 0.5f, 0.5f)};

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

        vec3 result = vec3(0, 0, 0);

        // adds directional light
        result += calcDirLight(dirLight, material, norm, viewDir);

        fragColor = vec4(result, 1.0f);
    }

    // toon shading mode
    else if (mode == 2.0) {

        vec3 ambientColor = materialAmbient;
 
        const int levels = 3;
        const float scaleFactor = 1.0 / levels;
        vec3 diffuseColor = materialDiffuse;
        
        vec3 L = normalize (dirLight.direction - world_pos);
        vec3 V = normalize (viewPos - FragPos);
 
        float diffuse = max(0, dot(L, world_normal));
        diffuseColor = diffuseColor * floor(diffuse * levels) * scaleFactor;
 
        vec3 H = normalize(L + V);
 
        float specular = 0.0;
 
        if (dot(L, world_normal) > 0.0) {
            specular = pow (max (0, dot(H, world_normal)), materialShininess);
        }
 
        //limit specular
        float specMask = (pow (dot (H, world_normal), materialShininess) > 0.3) ? 1 : 0;
 
        float edgeDetection = (dot (V, world_normal) > 0.3) ? 1 : 0.25f;
        
        //vec3 color = edgeDetection * (ambientColor + diffuseColor + (specular * materialSpecular) * specMask);
        vec3 color = dirLight.color * edgeDetection * (ambientColor + diffuseColor + (specular * materialSpecular) * specMask);
 
        fragColor = vec4(color, 1.0f);
    }
}