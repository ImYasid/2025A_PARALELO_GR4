#ifndef VEHICULO_H
#define VEHICULO_H

// Includes necesarios
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

class Vehiculo {
public:
    glm::vec3 Posicion;
    glm::vec3 Velocidad;

    Vehiculo(
        glm::vec3 posicion = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 velocidad = glm::vec3(0.0f, 0.0f, 0.0f)
    ) {
        this->Posicion = posicion;
        this->Velocidad = velocidad;
    }

    void ProcesarEntrada() {

    }

private:
    // Puedes agregar atributos privados aquí, como modelo, color, etc.
};

#endif // VEHICULO_H