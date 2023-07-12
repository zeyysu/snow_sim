#version 330

in vec4 fragPos;
in vec4 N;
out vec4 FragColor;


vec3 kd = vec3(0.8, 0.8, 0.8);
vec3 ka = vec3(0.3, 0.3, 0.3);
vec3 ks = vec3(0.8, 0.8, 0.8);

vec3 I = vec3(2, 2, 2);

vec3 Iamb = vec3(0.8, 0.8, 0.8);

vec3 color = vec3(0.53, 0.81, 0.94);
vec3 lightPosition = vec3(-0.15,0.4,1);
uniform vec3 eyePos;

void main(void)
{
    vec3 normal = vec3(N);
    vec3 rescolor = Iamb * ka * color;
    vec3 V = normalize(eyePos - vec3(fragPos));


        vec3 L = normalize(lightPosition - vec3(fragPos));
	    vec3 H = normalize(L + V);
        float NdotL = dot( normal, L);
	    float NdotH = dot( normal, H);

        float r2 = dot(lightPosition - vec3(fragPos), lightPosition - vec3(fragPos));

        vec3 diffuseColor = (1/r2) * I * max(0, NdotL) * color * kd;
		vec3 specularColor = (1/r2) * I * pow(max(0, NdotH), 400) * color * ks;

        rescolor += diffuseColor + specularColor;

    FragColor = vec4(rescolor, 1);
    //FragColor = vec4(1,1,1,1);
}