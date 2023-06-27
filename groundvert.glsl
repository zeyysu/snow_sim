#version 330
layout (location = 0) in vec3 aPosition;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
uniform sampler2D displacementMap;
const float displacementScale = 0.2;


uniform mat4 viewingMatrix;
uniform mat4 modelingMatrix; 
uniform mat4 projectionMatrix;

void main()
{
    float displacement = texture(displacementMap, aTexCoord).r;
    vec3 displacedPosition = aPosition + vec3(0.0, displacement * displacementScale, 0.0);
    FragPos = vec3(modelingMatrix * vec4(displacedPosition,1.0));
    gl_Position = projectionMatrix * viewingMatrix * vec4(FragPos,1.0);
    TexCoords = aTexCoord;
    float dx = (texture(displacementMap, aTexCoord+ vec2(1.0 / 512.0, 0.0)).r - texture(displacementMap, aTexCoord - vec2(1.0 / 512.0, 0.0)).r) * displacementScale;
    float dz = (texture(displacementMap, aTexCoord + vec2(0.0, 1.0 / 512.0)).r - texture(displacementMap, aTexCoord - vec2(0.0, 1.0 / 512.0)).r) * displacementScale;
    Normal = normalize(vec3(-dx, 1.0, -dz));
}