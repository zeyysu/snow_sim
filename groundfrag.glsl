#version 330

out vec4 fragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

vec3 lightPosition = vec3(-0.15,0.4,1);
float lightIntensity = 2;

uniform sampler2D displacementMap;
uniform sampler2D groundTexture;


void main()
{    
    vec3 lightDirection = normalize(lightPosition - FragPos);
    float diffuseFactor = max(dot(Normal, lightDirection), 0.0);
    //vec4 texColor = vec4(1.0,0.98,0.98,1.0);
    vec4 texColor = texture(groundTexture,TexCoords);
    vec3 diffuseColor = lightIntensity * vec3(texColor) * diffuseFactor;
    fragColor = vec4(diffuseColor,1.0);
    //float d = texture(displacementMap,TexCoords).r;
    //fragColor = vec4(d,d,d,1.0);
    //fragColor = texture(groundTexture, TexCoords);
    //fragColor = vec4(TexCoords, 0.0, 1.0);
}