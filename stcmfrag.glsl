#version 330

uniform samplerCube sampler;
in vec3 texCoord; 
out vec4 fragColor;

void main(void) {

    fragColor = texture(sampler, texCoord); 

}