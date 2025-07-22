#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "ModelLoader.h"
#include <learnopengl/shader.h>
#include <glm/glm.hpp>

class Scene {
public:
    std::vector<ModelLoader> models;
    glm::vec3 lightPosition;  // Posición de la luz
    glm::vec3 cameraPosition; // Posición de la cámara
    float shininess;          // Exponente de brillo especular

    // Constructor
    Scene(glm::vec3 lightPos = glm::vec3(5.0f, 5.0f, 5.0f), float shininessValue = 32.0f)
        : lightPosition(lightPos), shininess(shininessValue) {}

    // Agregar modelo a la escena
    void addModel(const ModelLoader& model) {
        models.push_back(model);
    }

    // Configurar la iluminación en el shader
    void setLightingUniforms(Shader& shader) {
        shader.use();
        shader.setVec3("lightPos", lightPosition);     // Posición de la luz
        shader.setVec3("viewPos", cameraPosition);     // Posición de la cámara
        shader.setFloat("shininess", shininess);       // Brillo especular
    }

    // Dibujar todos los modelos en la escena
    void draw(Shader& lightingShader, Shader& noLightingShader) {
        setLightingUniforms(lightingShader);
        for (auto& model : models) {
            model.draw(lightingShader,noLightingShader);
        }
    }

    // Obtener referencia a los modelos (útil para manipulaciones externas)
    std::vector<ModelLoader>& getModels() {
        return models;
    }
};

#endif
