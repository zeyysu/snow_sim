#version 330

uniform mat4 viewingMatrix;
uniform mat4 modelingMatrix; 
uniform mat4 projectionMatrix;

layout(location=0) in vec3 inVertex; 
out vec3 texCoord;

void main(void) {

    texCoord = inVertex; // texture coord equal to vertex position
    gl_Position = projectionMatrix * viewingMatrix * modelingMatrix * vec4(inVertex, 1); 

}