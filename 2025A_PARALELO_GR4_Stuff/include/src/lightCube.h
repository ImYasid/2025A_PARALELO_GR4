#ifndef LIGHTCUBE_H
#define LIGHTCUBE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <learnopengl/shader.h>
#include <vector>

struct PointLight {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
};

class LightCube {
public:
    unsigned int VAO, VBO;
    Shader lightCubeShader;

    LightCube(const char* vertexPath, const char* fragmentPath)
        : lightCubeShader(vertexPath, fragmentPath) {
        setupLightCube();
    }

    void setupLightCube();
    void render(glm::vec3 position, glm::mat4 view, glm::mat4 projection);
};

#endif