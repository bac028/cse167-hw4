#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

// Uniform variables can be updated by fetching their location and passing values to that location
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

// Outputs of the vertex shader are the inputs of the same name of the fragment shader.
out vec4 vertexColor;

out vec3 FragPos;
out vec3 FragVert;
out vec3 FragNormal;
out vec3 Normal;

out vec3 world_pos;
out vec3 world_normal;

void main()
{
    // phong shading
    Normal = mat3(transpose(inverse(model))) * normal;
    FragPos = vec3(model * vec4(position, 1.0));

    // normal shading
    gl_Position = projection * view * model * vec4(position, 1.0);
    vertexColor = vec4(0.5 * normalize(normal) + 0.5, 1);

    // toon shading
    world_pos = vec3(model * vec4(position, 1.0));
    world_normal = normalize(mat3(model) * normal);
}