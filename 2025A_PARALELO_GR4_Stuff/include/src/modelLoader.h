#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <learnopengl/model.h>
#include <learnopengl/shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <learnopengl/stb_image.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <string>
#include <iostream>

class ModelLoader {
public:
    Model model;
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;
    glm::vec3 originalPosition;
    bool useLighting;

    // IDs de texturas
    unsigned int diffuseMap = 0;
    unsigned int specularMap = 0;
    unsigned int normalMap = 0;

    // Constructor
    ModelLoader(const std::string& path,
        glm::vec3 pos = glm::vec3(0.0f),
        glm::vec3 scl = glm::vec3(1.0f),
        glm::vec3 rot = glm::vec3(0.0f),
        bool lighting = true)
        : model(path), position(pos), originalPosition(pos), scale(scl), rotation(rot), useLighting(lighting)
    {
        loadMaterialTextures(path);
    }

    // Dibujar el modelo con iluminación
    void draw(Shader& lightingShader, Shader& noLightingShader) {
        glm::mat4 modelMatrix = glm::mat4(1.0f);

        // Aplicar transformaciones
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMatrix = glm::scale(modelMatrix, scale);

        // Seleccionar el shader correcto
        Shader& shader = useLighting ? lightingShader : noLightingShader;
        shader.use();
        shader.setMat4("model", modelMatrix);

        // Enlazar texturas si están disponibles
        if (diffuseMap) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, diffuseMap);
            shader.setInt("map_kd", 0);
        }
        if (specularMap) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, specularMap);
            shader.setInt("map_ks", 1);
        }
        if (normalMap) {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, normalMap);
            shader.setInt("map_normal", 2);
        }

        // Dibujar el modelo
        model.Draw(shader);
    }

    // Métodos adicionales
    void moveTo(glm::vec3 newPos) {
        position = newPos;
    }

    void resetPosition() {
        position = originalPosition;
    }

    void rotateY(float degrees) {
        rotation.y += degrees;
    }

    glm::vec3 getPosition() const {
        return position;
    }

private:
    // Método para cargar las texturas desde el archivo .mtl usando Assimp
    void loadMaterialTextures(const std::string& modelPath) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }

        std::string directory = modelPath.substr(0, modelPath.find_last_of('/'));

        for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
            aiMaterial* material = scene->mMaterials[i];

            // Solo cargar la textura si está presente en el material
            if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
                loadTextureFromMaterial(material, aiTextureType_DIFFUSE, diffuseMap, directory);
            }
            if (material->GetTextureCount(aiTextureType_SPECULAR) > 0) {
                loadTextureFromMaterial(material, aiTextureType_SPECULAR, specularMap, directory);
            }
            // Finalmente, carga la textura de normales (map_Bump o map_normal)
            if (material->GetTextureCount(aiTextureType_HEIGHT) > 0) {  // Algunos programas exportan normales como HEIGHT
                loadTextureFromMaterial(material, aiTextureType_HEIGHT, normalMap, directory);
            }

        }
    }

    // Cargar textura de material si existe
    void loadTextureFromMaterial(aiMaterial* mat, aiTextureType type, unsigned int& textureID, const std::string& directory) {
        aiString str;
        if (mat->GetTexture(type, 0, &str) == AI_SUCCESS) { // Verifica si la textura existe
            std::string texturePath = directory + "/" + str.C_Str();

            std::cout << "Cargando textura: " << texturePath << " Tipo: " << type << std::endl;

            textureID = loadTexture(texturePath.c_str());
        }
        else {
            std::cout << "No se encontró textura para el tipo: " << type << std::endl;
            textureID = 0;  // Asegurar que no se usa una textura incorrecta
        }
    }

    unsigned int loadTexture(const char* path) {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);

        if (data) {
            GLenum format = (nrComponents == 1) ? GL_RED : (nrComponents == 3) ? GL_RGB : GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else {
            std::cerr << "ERROR: No se pudo cargar la textura: " << path << std::endl;
            std::cerr << "STB Image Error: " << stbi_failure_reason() << std::endl;

            glDeleteTextures(1, &textureID); // Eliminar la textura vacía
            textureID = 0; // Asegurar que no se enlaza una textura inválida
        }

        return textureID;
    }
    
};

#endif
