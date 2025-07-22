#ifndef RENDERER_H
#define RENDERER_H

#include "Scene.h"
#include <learnopengl/camera.h>

class Renderer {
public:
    void render(Scene& scene, Shader& lightingShader, Shader& noLightingShader, Camera& camera) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Configurar shaders de iluminación
        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        noLightingShader.use();
        noLightingShader.setMat4("projection", projection);
        noLightingShader.setMat4("view", view);

        scene.draw(lightingShader, noLightingShader);
    }
};

#endif
