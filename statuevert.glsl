#version 330

layout(location=0) in vec3 inVertex;
layout(location=1) in vec3 inNormal;

uniform mat4 viewingMatrix;
uniform mat4 modelingMatrix; 
uniform mat4 projectionMatrix;

out vec4 fragPos;
out vec4 N;

void main(void)
{
    fragPos = modelingMatrix * vec4(inVertex, 1);
    mat4 modelMatInvTr = inverse(transpose(modelingMatrix));
    N = normalize(modelMatInvTr * vec4(inNormal, 0));
    gl_Position = projectionMatrix * viewingMatrix * modelingMatrix * vec4(inVertex, 1.0);
}