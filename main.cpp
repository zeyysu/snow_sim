#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <GL/glew.h>   // The GL Header File
#include <OpenGL/gl.h>   // The GL Header File
#include <GLFW/glfw3.h> // The GLFW header
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// #include FT_FREETYPE_H

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

using namespace std;

int width = 900, height = 600;

GLuint skyboxvao;
//0=ground 1=snowflake
GLuint vao[3];
GLuint gVertexAttribBuffer[3], gIndexBuffer[3];
int gVertexDataSizeInBytes[3], gNormalDataSizeInBytes[3];
unsigned int cubemapTexture, groundTexture, snowTexture, displacementMapTexture;
const int textureWidth = 512;
const int textureHeight = 512;     
unsigned char* displacementMapData;
unsigned char* snowHeightData;
float frequency = 0.1;
float amplitude = 0.5;


//0=stcubemap 1=ground 2=snow 3=bunny
GLuint gProgram[4];
struct Vertex
{
	Vertex(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) { }
	GLfloat x, y, z;
};

struct Snowflake{
    Snowflake(glm::vec3 p, glm::vec3 d, float s, float v): position(p), size(s), velocity(v), d(d) {}
    glm::vec3 d;
    glm::vec3 position;
    float size;
    float velocity;
};

struct Texture
{
	Texture(GLfloat inU, GLfloat inV) : u(inU), v(inV) { }
	GLfloat u, v;
};

struct Normal
{
	Normal(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) { }
	GLfloat x, y, z;
};

struct Face
{
	Face(int v[], int t[], int n[]) {
		vIndex[0] = v[0];
		vIndex[1] = v[1];
		vIndex[2] = v[2];
		tIndex[0] = t[0];
		tIndex[1] = t[1];
		tIndex[2] = t[2];
		nIndex[0] = n[0];
		nIndex[1] = n[1];
		nIndex[2] = n[2];
	}
	GLuint vIndex[3], tIndex[3], nIndex[3];
};

vector<Vertex> gVertices[3];
vector<Texture> gTextures[3];
vector<Normal> gNormals[3];
vector<Face> gFaces[3];
vector<Snowflake> snowflakes;

float camRotAngle = 0;
glm::vec3 camPos = glm::vec3(0,0,0);
float velocity = 0;
glm::vec3 lightPosition[] = { glm::vec3(-1,1,1), glm::vec3(1,1,1), glm::vec3(0, 1,-1) };

glm::vec3 vectors[] = {
    glm::vec3(0,-1,0),
    glm::normalize(glm::vec3(0,-1,1)),
     glm::normalize(glm::vec3(0,-1,-1)),
     glm::normalize(glm::vec3(-1,-1,0)),
     glm::normalize(glm::vec3(1,-1,0)),
     glm::normalize(glm::vec3(-1,-1,1)),
     glm::normalize(glm::vec3(-1,-1,-1)),
     glm::normalize(glm::vec3(1,-1,-1)),
     glm::normalize(glm::vec3(1,-1,1))
};

glm::vec3 positions[] = {
    glm::vec3(-0.75,1,-0.75),
    glm::vec3(-0.75,1,-0.25),
    glm::vec3(-0.75,1,0.25),
    glm::vec3(-0.75,1,0.75),
    glm::vec3(-0.25,1,-0.75),
    glm::vec3(-0.25,1,-0.25),
    glm::vec3(-0.25,1,0.25),
    glm::vec3(-0.25,1,0.75),
    glm::vec3(0.25,1,-0.75),
    glm::vec3(0.25,1,-0.25),
    glm::vec3(0.25,1,0.25),
    glm::vec3(0.25,1,0.75),
    glm::vec3(0.75,1,-0.75),
    glm::vec3(0.75,1,-0.25),
    glm::vec3(0.75,1,0.25),
    glm::vec3(0.75,1,0.75)
};

GLfloat skyboxVertices[] = {        
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
};

GLfloat snnowTexCoords[] = {
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f
};

void drawModel(int i)
{
	glBindVertexArray(vao[i]);
	glDrawElements(GL_TRIANGLES, gFaces[i].size() * 3, GL_UNSIGNED_INT, 0);
}

void displayStatues(glm::mat4 &viewing, glm::mat4 &perspective, glm::vec3 eyePos) {
    glUseProgram(gProgram[3]);

    glm::mat4 translate = glm::translate(glm::mat4(1.f), glm::vec3(0.16,-0.001,0.16));
    glm::mat4 scale = glm::scale(glm::mat4(1.f), glm::vec3(0.012,0.012,0.012));

    glUniformMatrix4fv(glGetUniformLocation(gProgram[3], "viewingMatrix"), 1, GL_FALSE, glm::value_ptr(viewing));
    glUniformMatrix4fv(glGetUniformLocation(gProgram[3], "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(perspective));
    glUniform3fv(glGetUniformLocation(gProgram[3],"eyePos"),1, glm::value_ptr(eyePos));

    for(int i = 0; i < 8; i++){
        glm::mat4 rotate = glm::rotate(glm::mat4(1.f), glm::radians(i*45.f), glm::vec3(0,1,0));
        glm::mat4 model = rotate * translate * scale;
        glUniformMatrix4fv(glGetUniformLocation(gProgram[3], "modelingMatrix"), 1, GL_FALSE, glm::value_ptr(model));
        drawModel(2);
    }
}

void displaysnow(glm::mat4 &viewing, glm::mat4 &perspective){
    glUseProgram(gProgram[2]);
    glm::mat4 inverseViewMatrix = glm::inverse(viewing);
    glm::mat3 cameraOrientationMatrix = glm::mat3(inverseViewMatrix); 
    glm::mat4 modelMatrix = glm::mat4(1.0f); // Identity matrix
    glm::mat3 quadOrientationMatrix = glm::mat3(modelMatrix);
    glm::mat3 billboardOrientationMatrix = cameraOrientationMatrix * quadOrientationMatrix;
    modelMatrix = glm::mat4(billboardOrientationMatrix);  
     
    int vecSize = snowflakes.size();
    vector<Snowflake>::iterator iter = snowflakes.begin();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, snowTexture);
    for(iter; iter < snowflakes.end(); iter++)
    {
        int pos = rand() % 9;
        if(iter->position.y>-0.0048){
        iter->d = iter->d + vectors[pos];
        iter->d = glm::normalize(iter->d);
        iter->position = iter->position + iter->d * iter->velocity;
        if(iter->position.y<-0.005) {
            iter->position.y = -0.005;
            float disX = iter->position.x - (-0.5);
            if(disX > 1.0 || disX < 0.0) continue;;
            float disz = iter->position.z - (-0.5);
            if(disz > 1.0 || disz < 0.0) continue;;
            int txtw = disX * (textureWidth-1);
            int txth = 512 - disz * (textureHeight-1);
            int index = txth * textureWidth + txtw;
            snowHeightData[index] += 2;
            if(snowHeightData[index]>255) snowHeightData[index]=255;
        }
        }
        else snowflakes.erase(iter);
        glm::mat4 scale = glm::scale(glm::mat4(1.f), glm::vec3(iter->size,iter->size,iter->size));
        glm::mat4 translate = glm::translate(glm::mat4(1.f), iter->position);
        glm::mat4 model =  translate * modelMatrix * scale;
        glUniformMatrix4fv(glGetUniformLocation(gProgram[2], "viewingMatrix"), 1, GL_FALSE, glm::value_ptr(viewing));
        glUniformMatrix4fv(glGetUniformLocation(gProgram[2], "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(perspective));
        glUniformMatrix4fv(glGetUniformLocation(gProgram[2], "modelingMatrix"), 1, GL_FALSE, glm::value_ptr(model));
        //glUniform1i( glGetUniformLocation(gProgram[2], "texture0"), 0);
        drawModel(1);
    }
}

void displayground(glm::mat4 &viewing, glm::mat4 &perspective){
    glUseProgram(gProgram[1]);
    glBindTexture(GL_TEXTURE_2D, displacementMapTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, textureWidth, textureHeight, GL_RED, GL_UNSIGNED_BYTE, snowHeightData);
    glEnable(GL_DEPTH_TEST);
    glm::mat4 rotate = glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(1, 0, 0));
    glm::mat4 translate = glm::translate(glm::mat4(1.f), glm::vec3(0,-0.0005,0));
    glm::mat4 model = translate;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, displacementMapTexture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, groundTexture);


    glUniformMatrix4fv(glGetUniformLocation(gProgram[1], "viewingMatrix"), 1, GL_FALSE, glm::value_ptr(viewing));
    glUniformMatrix4fv(glGetUniformLocation(gProgram[1], "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(perspective));
    glUniformMatrix4fv(glGetUniformLocation(gProgram[1], "modelingMatrix"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i( glGetUniformLocation(gProgram[1], "groundTexture"), 1);
    glUniform1i( glGetUniformLocation(gProgram[1], "displacementMap"), 0);
    //glDisable(GL_DEPTH_TEST);
    drawModel(0);
}

void displayCM(glm::mat4 &viewing, glm::mat4 &perspective){
    // glDepthFunc(GL_LEQUAL);
    glDisable(GL_DEPTH_TEST);
    glUseProgram(gProgram[0]);

    glBindVertexArray(skyboxvao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    glm::mat4 model = glm::mat4(1.0f);

    glUniformMatrix4fv(glGetUniformLocation(gProgram[0], "viewingMatrix"), 1, GL_FALSE, glm::value_ptr(viewing));
    glUniformMatrix4fv(glGetUniformLocation(gProgram[0], "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(perspective));
    glUniformMatrix4fv(glGetUniformLocation(gProgram[0], "modelingMatrix"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i( glGetUniformLocation(gProgram[0], "sampler"), 0);
    
    // glBindSampler(0, sampler); 

	glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LESS);
    
}


void display(){

    glClearColor(0, 0, 0, 1);
    glClearDepth(1.0f);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glm::mat4 camRotMat = glm::rotate(glm::mat4(1.f),glm::radians(camRotAngle), glm::vec3(0,1,0));
    glm::vec4 camV = camRotMat * glm::vec4(0,0,1,0);
    camPos = camPos + glm::vec3(velocity * camV);

    glm::vec4 eyePos = camRotMat * glm::vec4(0,0,0,1); 
    glm::vec4 lookAt = camRotMat * glm::vec4(0,0,1,1);
    glm::mat4 viewing = glm::lookAt(glm::vec3(eyePos), glm::vec3(lookAt), glm::vec3(0,1,0));
    glm::mat4 perspective = glm::perspective(glm::radians(45.f), (GLfloat)width / (GLfloat)height, 0.001f, 100.0f);
    displayCM(viewing, perspective);
    glm::mat4 camX = glm::translate(glm::mat4(1.f), camPos);
    eyePos = camX * eyePos;
    lookAt = camX * lookAt;
    viewing = glm::lookAt(glm::vec3(eyePos), glm::vec3(lookAt), glm::vec3(0,1,0));
    displayStatues(viewing, perspective, glm::vec3(eyePos));

    displayground(viewing, perspective);
    for(int i=0;i<20;i++){
        int x = rand() % 200 - 100;
        int z = rand() % 200 - 100;
        int v = rand() % 3 + 3;
        int s = rand() % 7 + 3;
        snowflakes.push_back(Snowflake(glm::vec3(x/100.f,1,z/100.f), glm::vec3(0,-1,0), s/10000.f, v/1000.f));
    }
    displaysnow(viewing,perspective);
}

void loadGroundTexture(){
    glGenTextures(1, &groundTexture);
    int width, height, nrComponents;
    unsigned char *data = stbi_load("snowground3.jpg", &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        glBindTexture(GL_TEXTURE_2D, groundTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: ground_texture_sand.jpg" << std::endl;
        stbi_image_free(data);
    }
    GLfloat* textureData = new GLfloat[gTextures[0].size() * 2];
    for (int k = 0; k < gTextures[0].size(); ++k)
	{
		textureData[2 * k] = gTextures[0][k].u;
		textureData[2 * k + 1] =gTextures[0][k].v;
	}
    int texDataSizeInBytes = gTextures[0].size() * 2 * sizeof(GLfloat);
    GLuint uvCoord;
    glBindVertexArray(vao[0]);
    glGenBuffers(1, &uvCoord);
    glBindBuffer(GL_ARRAY_BUFFER,uvCoord);
    glBufferData(GL_ARRAY_BUFFER, texDataSizeInBytes, textureData, GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    delete [] textureData;

    snowHeightData = new unsigned char[textureWidth * textureHeight];
    for (int i = 0; i < textureWidth * textureHeight; i++)
        snowHeightData[i] = 0;
    glGenTextures(1, &displacementMapTexture);
    glBindTexture(GL_TEXTURE_2D, displacementMapTexture);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, textureWidth, textureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, snowHeightData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void loadSnowTexture(){
    glGenTextures(1, &snowTexture);
    int width, height, nrComponents;
    unsigned char *data = stbi_load("snowcircle.png", &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, snowTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: ground_texture_sand.jpg" << std::endl;
        stbi_image_free(data);
    }
    GLuint uvCoord;
    glBindVertexArray(vao[1]);
    glGenBuffers(1, &uvCoord);
    glBindBuffer(GL_ARRAY_BUFFER,uvCoord);
    glBufferData(GL_ARRAY_BUFFER, sizeof(snnowTexCoords), &snnowTexCoords, GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}

void loadCubemap(){

    glGenTextures(1, &cubemapTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    const char* images[] = {"HornstullsStrand2/posx.jpg",
        "HornstullsStrand2/negx.jpg",
        "HornstullsStrand2/posy.jpg",
        "HornstullsStrand2/negy.jpg",
        "HornstullsStrand2/posz.jpg",
        "HornstullsStrand2/negz.jpg"
    };

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < 6; i++)
    {
        unsigned char *data = stbi_load(images[i], &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0,  GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << images[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

}

bool ReadDataFromFile(
    const string& fileName, ///< [in]  Name of the shader file
    string&       data)     ///< [out] The contents of the file
{
    fstream myfile;

    // Open the input 
    myfile.open(fileName.c_str(), std::ios::in);

    if (myfile.is_open())
    {
        string curLine;

        while (getline(myfile, curLine))
        {
            data += curLine;
            if (!myfile.eof())
            {
                data += "\n";
            }
        }

        myfile.close();
    }
    else
    {
        return false;
    }

    return true;
}

void createVS(GLuint& program, const string& filename)
{
    string shaderSource;

    if (!ReadDataFromFile(filename, shaderSource))
    {
        cout << "Cannot find file name: " + filename << endl;
        exit(-1);
    }

    GLint length = shaderSource.length();
    const GLchar* shader = (const GLchar*) shaderSource.c_str();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &shader, &length);
    glCompileShader(vs);

    char output[1024] = {0};
    glGetShaderInfoLog(vs, 1024, &length, output);
    printf("VS compile log: %s\n", output);

    glAttachShader(program, vs);
}

void createFS(GLuint& program, const string& filename)
{
    string shaderSource;

    if (!ReadDataFromFile(filename, shaderSource))
    {
        cout << "Cannot find file name: " + filename << endl;
        exit(-1);
    }

    GLint length = shaderSource.length();
    const GLchar* shader = (const GLchar*) shaderSource.c_str();

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &shader, &length);
    glCompileShader(fs);

    char output[1024] = {0};
    glGetShaderInfoLog(fs, 1024, &length, output);
    printf("FS compile log: %s\n", output);

    glAttachShader(program, fs);
}

void initShaders()
{
    gProgram[0] = glCreateProgram();
    gProgram[1] = glCreateProgram();
    gProgram[2] = glCreateProgram();
    gProgram[3] = glCreateProgram();

    createVS(gProgram[0], "stcmvert.glsl");
    createFS(gProgram[0], "stcmfrag.glsl");

    createVS(gProgram[1], "groundvert.glsl");
    createFS(gProgram[1], "groundfrag.glsl");

    createVS(gProgram[2], "snowvert.glsl");
    createFS(gProgram[2], "snowfrag.glsl");

    createVS(gProgram[3], "statuevert.glsl");
    createFS(gProgram[3], "statuefrag.glsl");
   

    glLinkProgram(gProgram[0]);
    glLinkProgram(gProgram[1]);
    glLinkProgram(gProgram[2]);
    glLinkProgram(gProgram[3]);
    glUseProgram(gProgram[0]);
}

void initSkyboxVBO(){
    GLuint skyboxvbo;
    glGenVertexArrays(1, &skyboxvao);
    assert(skyboxvao > 0);
    glBindVertexArray(skyboxvao);

    assert(glGetError() == GL_NONE);
    glGenBuffers(1, &skyboxvbo);
    assert(skyboxvbo > 0);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void initVBOs()
{
    for(int i=0; i<3; i++){
    glGenVertexArrays(1, &vao[i]);
    assert(vao[i] > 0);
    glBindVertexArray(vao[i]);
    // cout << "vao = " << vao << endl;

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	assert(glGetError() == GL_NONE);

	glGenBuffers(1, &gVertexAttribBuffer[i]);
	glGenBuffers(1, &gIndexBuffer[i]);

	assert(gVertexAttribBuffer[i] > 0 && gIndexBuffer[i] > 0);

	glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBuffer[i]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer[i]);
    gVertexDataSizeInBytes[i] = gVertices[i].size() * 3 * sizeof(GLfloat);
	gNormalDataSizeInBytes[i] = gNormals[i].size() * 3 * sizeof(GLfloat);
	int indexDataSizeInBytes = gFaces[i].size() * 3 * sizeof(GLuint);
	GLfloat* vertexData = new GLfloat[gVertices[i].size() * 3];
	GLfloat* normalData = new GLfloat[gNormals[i].size() * 3];
	GLuint* indexData = new GLuint[gFaces[i].size() * 3];

	float minX = 1e6, maxX = -1e6;
	float minY = 1e6, maxY = -1e6;
	float minZ = 1e6, maxZ = -1e6;

	for (int k = 0; k < gVertices[i].size(); ++k)
	{
		vertexData[3 * k] = gVertices[i][k].x;
		vertexData[3 * k + 1] = gVertices[i][k].y;
		vertexData[3 * k + 2] = gVertices[i][k].z;

		minX = std::min(minX, gVertices[i][k].x);
		maxX = std::max(maxX, gVertices[i][k].x);
		minY = std::min(minY, gVertices[i][k].y);
		maxY = std::max(maxY, gVertices[i][k].y);
		minZ = std::min(minZ, gVertices[i][k].z);
		maxZ = std::max(maxZ, gVertices[i][k].z);
	}

	for (int k = 0; k < gNormals[i].size(); ++k)
	{
		normalData[3 * k] = gNormals[i][k].x;
		normalData[3 * k + 1] = gNormals[i][k].y;
		normalData[3 * k + 2] = gNormals[i][k].z;
	}

	for (int k = 0; k < gFaces[i].size(); ++k)
	{
		indexData[3 * k] = gFaces[i][k].vIndex[0];
		indexData[3 * k + 1] = gFaces[i][k].vIndex[1];
		indexData[3 * k + 2] = gFaces[i][k].vIndex[2];
	}


	glBufferData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes[i] + gNormalDataSizeInBytes[i], 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, gVertexDataSizeInBytes[i], vertexData);
	glBufferSubData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes[i], gNormalDataSizeInBytes[i], normalData);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSizeInBytes, indexData, GL_STATIC_DRAW);

	// done copying; can free now
	delete[] vertexData;
	delete[] normalData;
	delete[] indexData;

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVertexDataSizeInBytes[i]));
    }
}

bool ParseObj(const string& fileName, unsigned int i)
{
    fstream myfile;

    // Open the input 
    myfile.open(fileName.c_str(), std::ios::in);

    if (myfile.is_open())
    {
        string curLine;

        while (getline(myfile, curLine))
        {
            stringstream str(curLine);
            GLfloat c1, c2, c3;
            GLuint index[9];
            string tmp;

            if (curLine.length() >= 2)
            {
                if (curLine[0] == '#') // comment
                {
                    continue;
                }
                else if (curLine[0] == 'v')
                {
                    if (curLine[1] == 't') // texture
                    {
                        str >> tmp; // consume "vt"
                        str >> c1 >> c2;
                        gTextures[i].push_back(Texture(c1, c2));
                    }
                    else if (curLine[1] == 'n') // normal
                    {
                        str >> tmp; // consume "vn"
                        str >> c1 >> c2 >> c3;
                        gNormals[i].push_back(Normal(c1, c2, c3));
                    }
                    else // vertex
                    {
                        str >> tmp; // consume "v"
                        str >> c1 >> c2 >> c3;
                        gVertices[i].push_back(Vertex(c1, c2, c3));
                    }
                }
                else if (curLine[0] == 'f') // face
                {
                    str >> tmp; // consume "f"
					char c;
					int vIndex[3],  nIndex[3], tIndex[3];
                    if(i == 2){
                        str >> vIndex[0]; str >> c >> c; // consume "//"
					str >> nIndex[0]; 
					str >> vIndex[1]; str >> c >> c; // consume "//"
					str >> nIndex[1]; 
					str >> vIndex[2]; str >> c >> c; // consume "//"
					str >> nIndex[2]; 
                    }else{
					str >> vIndex[0]; str >> c; // consume "//"
					str >> tIndex[0];  str >> c;
                    str >> nIndex[0];
					str >> vIndex[1]; str >> c; // consume "//"
					str >> tIndex[1];  str >> c;
                    str >> nIndex[1];
					str >> vIndex[2]; str >> c; // consume "//"
					str >> tIndex[2];  str >> c;
                    str >> nIndex[2];
                    }

					// assert(vIndex[0] == nIndex[0] &&
					// 	   vIndex[1] == nIndex[1] &&
					// 	   vIndex[2] == nIndex[2]); // a limitation for now

					// make indices start from 0
					for (int c = 0; c < 3; ++c)
					{
						vIndex[c] -= 1;
						nIndex[c] -= 1;
						tIndex[c] -= 1;
					}

                    gFaces[i].push_back(Face(vIndex, tIndex, nIndex));
                }
                else
                {
                    cout << "Ignoring unidentified line in obj file: " << curLine << endl;
                }
            }

            //data += curLine;
            if (!myfile.eof())
            {
                //data += "\n";
            }
        }

        myfile.close();
    }
    else
    {
        return false;
    }

	assert(gVertices[i].size() == gNormals[i].size());

    return true;
}


void init() 
{

    ParseObj("obj/square.obj",0);
    ParseObj("obj/quad.obj", 1);
    ParseObj("obj/bunny.obj", 2);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
    initSkyboxVBO();
    initVBOs();
    loadCubemap();
    loadGroundTexture();
    loadSnowTexture();
    initShaders();
}


void reshape(GLFWwindow* window, int w, int h)
{
    w = w < 1 ? 1 : w;
    h = h < 1 ? 1 : h;

    width = w;
    height = h;
    glViewport(0, 0, w, h);
}


void mainLoop(GLFWwindow* window)
{
    while (!glfwWindowShouldClose(window))
    {
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key == GLFW_KEY_A && action == GLFW_PRESS)
    {
        camRotAngle += 2;
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        camRotAngle -= 2;
    }
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        velocity += 0.0001;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
       velocity -= 0.0001;
    }
}

int main(int argc, char** argv)   // Create Main Function For Bringing It All Together
{
    if(argc!=1){
        cout<<"usage: ./main"<<endl;
        exit(1);
    }

    GLFWwindow* window;
    if (!glfwInit())
    {
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    window = glfwCreateWindow(width, height, "Simple Example", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

     if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    // char rendererInfo[512] = {0};
    // strcpy(rendererInfo, (const char*) glGetString(GL_RENDERER));
    // strcat(rendererInfo, " - ");
    // strcat(rendererInfo, (const char*) glGetString(GL_VERSION));
    char title[] = "CENG469_HW2";
    glfwSetWindowTitle(window, title);

    init();

    glfwSetKeyCallback(window, keyboard);

    glfwSetWindowSizeCallback(window, reshape);

    reshape(window, width, height); // need to call this once ourselves
    mainLoop(window); // this does not return unless the window is closed

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}