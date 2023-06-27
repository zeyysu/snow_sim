#version 330
layout (location = 0) in vec3 inVertex;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 texCoords;

out vec2 TexCoords;


uniform mat4 viewingMatrix;
uniform mat4 modelingMatrix; 
uniform mat4 projectionMatrix;

void main()
{
    TexCoords = texCoords;    
    gl_Position = projectionMatrix * viewingMatrix * modelingMatrix * vec4(inVertex, 1.0);
}