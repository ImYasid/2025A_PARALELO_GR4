#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION 
#include <learnopengl/stb_image.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// primera persona
bool vistaPrimeraPersona1 = false;
bool vistaPrimeraPersona2 = false;
bool cambiarVistaPresionado = false;

// settings
const unsigned int SCR_WIDTH = 1820;
const unsigned int SCR_HEIGHT = 980;

// Cámaras para cada jugador
Camera cameraJugador1(glm::vec3(3.5f, -10.0f, 5.0f));  // Jugador 1
Camera cameraJugador2(glm::vec3(3.5f, -5.0f, 5.0f));  // Jugador 2


// variables para el control del vehículo
glm::vec3 carPosition1(0.0f, -10.0f, 0.0f);
float carRotation1 = 0.0f;
float carSpeed1 = 0.0f;

glm::vec3 carPosition2(5.0f, -10.0f, 0.0f); // Empieza más a la derecha
float carRotation2 = 0.0f;
float carSpeed2 = 0.0f;


const float maxSpeed = 7.0f;
const float acceleration = 4.0f;
const float deceleration = 15.0f;
const float rotationSpeed = 2.0f;

glm::vec3 actualCameraPos1 = cameraJugador1.Position;
glm::vec3 actualCameraPos2 = cameraJugador2.Position;


float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Examen Bimestral - Conduccion 3ra Persona", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    Shader ourShader("shaders/shader_exercise16_mloading.vs", "shaders/shader_exercise16_mloading.fs");

    // load models
    Model ourModelDRT("models/DRT/DriftTrack3.obj");
    Model ourModelnebula("models/nebula/Sin_nombre.obj");
    Model ourModelVehiculo1("models/carro1/carro1.obj");
	Model ourModelVehiculo2("models/carro2/carro1.obj");


    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // render
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // usar shader
        ourShader.use();

        float cameraDistance1 = glm::mix(6.0f, 12.0f, glm::clamp(abs(carSpeed1) / maxSpeed, 0.0f, 1.0f));
        float cameraDistance2 = glm::mix(6.0f, 12.0f, glm::clamp(abs(carSpeed2) / maxSpeed, 0.0f, 1.0f));
        float cameraHeight = 5.0f;

        // ==================== JUGADOR 1 (vista superior) ====================
        glViewport(0, 0, SCR_WIDTH / 2, SCR_HEIGHT);

        glm::vec3 camPos1;

        if (vistaPrimeraPersona1) {
            // Colocamos la cámara justo encima de la posición del carro
            glm::vec3 arriba(0.0f, 1.2f, 0.0f);  // Ajusta la altura si es necesario

            // Alinea la dirección del carro
            glm::vec3 adelante = glm::vec3(-sin(carRotation1), 0.0f, -cos(carRotation1)) * -2.0f;  // Dirección hacia adelante del carro
            glm::vec3 aLaIzquierda = glm::vec3(0.0f, 0.0f, 0.0f);

            // Establece la posición de la cámara en la primera persona
            camPos1 = carPosition1 + arriba + adelante + aLaIzquierda;

            // No interpolar en primera persona para evitar el retroceso
            actualCameraPos1 = camPos1;

        }
        else {
            // Cámara en tercera persona: Interpolación suave
            camPos1 = carPosition1 - glm::vec3(sin(carRotation1) * cameraDistance1, cameraHeight, cos(carRotation1) * cameraDistance1);
            camPos1.y += 40.0f;
            actualCameraPos1 = glm::mix(actualCameraPos1, camPos1, 5.0f * deltaTime);
            cameraJugador1.Position = actualCameraPos1;
            cameraJugador1.Front = glm::normalize(carPosition1 - cameraJugador1.Position);
            cameraJugador1.Up = glm::vec3(0.0f, 1.0f, 0.0f);
        }

        actualCameraPos1 = glm::mix(actualCameraPos1, camPos1, 5.0f * deltaTime);
        cameraJugador1.Position = actualCameraPos1;


        if (vistaPrimeraPersona1) {
            cameraJugador1.Front = glm::vec3(sin(carRotation1), 0.0f, cos(carRotation1)); // mira hacia adelante según rotación
        }
        else {
            cameraJugador1.Front = glm::normalize(carPosition1 - cameraJugador1.Position); // sigue mirando al carro
        }
        cameraJugador1.Up = glm::vec3(0.0f, 1.0f, 0.0f);
        
        
        
        glm::mat4 projection = glm::perspective(glm::radians(cameraJugador1.Zoom), (float)(SCR_WIDTH / 2) / (float)SCR_HEIGHT, 0.08f, 300.0f);
        glm::mat4 view = cameraJugador1.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        ourShader.setVec3("lightColor", glm::vec3(0.2f, 0.2f, 0.4f));
        ourShader.setVec3("lightPos", glm::vec3(0.0f, 10.0f, 0.0f));
        ourShader.setVec3("viewPos", cameraJugador1.Position);
        ourShader.setVec3("emissionColor", glm::vec3(0.5f, 0.5f, 0.5f));


        // Render modelos
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -10.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.3f));
        ourShader.setMat4("model", model);
        ourModelDRT.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(75.0f));
        ourShader.setMat4("model", model);
        ourModelnebula.Draw(ourShader);

        // Vehículo 1
        model = glm::mat4(1.0f);
        model = glm::translate(model, carPosition1);
        model = glm::rotate(model, carRotation1, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.8f));
        ourShader.setMat4("model", model);
        ourModelVehiculo1.Draw(ourShader);

        // Vehículo 2
        model = glm::mat4(1.0f);
        model = glm::translate(model, carPosition2);
        model = glm::rotate(model, carRotation2, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.8f));
        ourShader.setMat4("model", model);
        ourModelVehiculo2.Draw(ourShader);

        // ==================== JUGADOR 2 (vista inferior) ====================
        glViewport(SCR_WIDTH / 2, 0, SCR_WIDTH / 2, SCR_HEIGHT);

        glm::vec3 camPos2;

        if (vistaPrimeraPersona2) {
            // Colocamos la cámara justo encima de la posición del carro
            glm::vec3 arriba(0.0f, 1.2f, 0.0f);  // Ajusta la altura si es necesario

            // Alinea la dirección del carro
            glm::vec3 adelante = glm::vec3(-sin(carRotation2), 0.0f, -cos(carRotation2));  // Dirección hacia adelante del carro
            glm::vec3 aLaIzquierda = glm::vec3(0.0f, 0.0f, 0.0f);

            // Aumenta la distancia al capó
            adelante *= -2.0f;  // Aumenta la distancia de la cámara al capó

            // Establece la posición de la cámara en la primera persona
            camPos2 = carPosition2 + arriba + adelante + aLaIzquierda;

            // No interpolar en primera persona para evitar el retroceso
            actualCameraPos2 = camPos2;

        }
        else {
            // Cámara en tercera persona: Interpolación suave
            camPos2 = carPosition2 - glm::vec3(sin(carRotation2) * cameraDistance2, cameraHeight, cos(carRotation2) * cameraDistance2);
            camPos2.y += 40.0f;
            actualCameraPos2 = glm::mix(actualCameraPos2, camPos2, 5.0f * deltaTime);
            cameraJugador2.Position = actualCameraPos2;
            cameraJugador2.Front = glm::normalize(carPosition2 - cameraJugador2.Position);
            cameraJugador2.Up = glm::vec3(0.0f, 1.0f, 0.0f);
        }

        actualCameraPos2 = glm::mix(actualCameraPos2, camPos2, 5.0f * deltaTime);
        cameraJugador2.Position = actualCameraPos2;


        if (vistaPrimeraPersona2) {
            cameraJugador2.Front = glm::vec3(sin(carRotation2), 0.0f, cos(carRotation2)); // mira hacia adelante según rotación
        }
        else {
            cameraJugador2.Front = glm::normalize(carPosition2 - cameraJugador2.Position); // sigue mirando al carro
        }
        cameraJugador2.Up = glm::vec3(0.0f, 1.0f, 0.0f);


        projection = glm::perspective(glm::radians(cameraJugador2.Zoom), (float)(SCR_WIDTH / 2) / (float)SCR_HEIGHT, 0.08f, 300.0f);
        view = cameraJugador2.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        ourShader.setVec3("lightColor", glm::vec3(0.2f, 0.2f, 0.4f));
        ourShader.setVec3("lightPos", glm::vec3(0.0f, 10.0f, 0.0f));
        ourShader.setVec3("viewPos", cameraJugador2.Position);
        ourShader.setVec3("emissionColor", glm::vec3(0.5f, 0.5f, 0.5f));


        // Render modelos (otra vez, porque es otra vista)
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -10.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.3f));
        ourShader.setMat4("model", model);
        ourModelDRT.Draw(ourShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(75.0f));
        ourShader.setMat4("model", model);
        ourModelnebula.Draw(ourShader);

        // Vehículo 1
        model = glm::mat4(1.0f);
        model = glm::translate(model, carPosition1);
        model = glm::rotate(model, carRotation1, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.8f));
        ourShader.setMat4("model", model);
        ourModelVehiculo1.Draw(ourShader);

        // Vehículo 2
        model = glm::mat4(1.0f);
        model = glm::translate(model, carPosition2);
        model = glm::rotate(model, carRotation2, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.8f));
        ourShader.setMat4("model", model);
        ourModelVehiculo2.Draw(ourShader);

        // === COLISIONES ===
// Matrices de transformación para cada modelo
        glm::mat4 car1Matrix = glm::mat4(1.0f);
        car1Matrix = glm::translate(car1Matrix, carPosition1);
        car1Matrix = glm::rotate(car1Matrix, carRotation1, glm::vec3(0.0f, 1.0f, 0.0f));
        car1Matrix = glm::scale(car1Matrix, glm::vec3(0.8f));

        glm::mat4 car2Matrix = glm::mat4(1.0f);
        car2Matrix = glm::translate(car2Matrix, carPosition2);
        car2Matrix = glm::rotate(car2Matrix, carRotation2, glm::vec3(0.0f, 1.0f, 0.0f));
        car2Matrix = glm::scale(car2Matrix, glm::vec3(0.8f));

        glm::mat4 drtMatrix = glm::mat4(1.0f);
        drtMatrix = glm::translate(drtMatrix, glm::vec3(0.0f, -10.0f, 0.0f)); // Ajusta según tu escena
        drtMatrix = glm::scale(drtMatrix, glm::vec3(0.3f));

        glm::mat4 nebulaMatrix = glm::mat4(1.0f);
        nebulaMatrix = glm::translate(nebulaMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
        nebulaMatrix = glm::scale(nebulaMatrix, glm::vec3(75.0f));

        // Obtener bounding boxes transformadas
        BoundingBox car1Box = ourModelVehiculo1.getTransformedBoundingBox(car1Matrix);
        BoundingBox car2Box = ourModelVehiculo2.getTransformedBoundingBox(car2Matrix);
        BoundingBox drtBox = ourModelDRT.getTransformedBoundingBox(drtMatrix);

        // 1. Colisión entre vehículos
        if (Model::checkCollision(car1Box, car2Box)) {
            // Física simple de rebote
            carSpeed1 = -carSpeed1 * 0.5f;
            carSpeed2 = -carSpeed2 * 0.5f;
        }

        // 2. Colisión vehículos con pista (DRT)
        bool car1OnTrack = Model::checkCollision(car1Box, drtBox);
        bool car2OnTrack = Model::checkCollision(car2Box, drtBox);

        if (!car1OnTrack) {
            std::cout << "¡Auto 1 fuera de pista (DRT)!" << std::endl;
            carSpeed1 *= 0.95f; // Reducir velocidad fuera de pista
        }

        if (!car2OnTrack) {
            std::cout << "¡Auto 2 fuera de pista (DRT)!" << std::endl;
            carSpeed2 *= 0.95f; // Reducir velocidad fuera de pista
        }

        // swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

	// Cambiar vista al presionar 'C' (cambiar entre primera y tercera persona)
    
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !cambiarVistaPresionado)
    {
        // Cambiar vista para el jugador 1
        vistaPrimeraPersona1 = !vistaPrimeraPersona1;

        // Cambiar vista para el jugador 2 
        vistaPrimeraPersona2 = !vistaPrimeraPersona2;

        cambiarVistaPresionado = true;
    }

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE)
    {
        cambiarVistaPresionado = false;
    }
    
   
    // Controles jugador 1 - WASD
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        carSpeed1 += acceleration * deltaTime;
        if (carSpeed1 > maxSpeed) carSpeed1 = maxSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        carSpeed1 -= acceleration * deltaTime;
        if (carSpeed1 < -maxSpeed / 2) carSpeed1 = -maxSpeed / 2;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        carRotation1 += rotationSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        carRotation1 -= rotationSpeed * deltaTime;
    }

    // Controles jugador 2 - Flechas
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        carSpeed2 += acceleration * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        carSpeed2 -= acceleration * deltaTime;
        if (carSpeed2 < -maxSpeed / 2) carSpeed2 = -maxSpeed / 2;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        carRotation2 += rotationSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        carRotation2 -= rotationSpeed * deltaTime;
    }


    // Deceleración cuando no se presionan teclas
    if (glfwGetKey(window, GLFW_KEY_W) != GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_S) != GLFW_PRESS) {
        if (carSpeed1 > 0) {
            carSpeed1 -= deceleration * deltaTime;
            if (carSpeed1 < 0) carSpeed1 = 0;
        }
        else if (carSpeed1 < 0) {
            carSpeed1 += deceleration * deltaTime;
            if (carSpeed1 > 0) carSpeed1 = 0;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_UP) != GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_DOWN) != GLFW_PRESS) {
        if (carSpeed2 > 0) {
            carSpeed2 -= deceleration * deltaTime;
            if (carSpeed2 < 0) carSpeed2 = 0;
        }
        else if (carSpeed2 < 0) {
            carSpeed2 += deceleration * deltaTime;
            if (carSpeed2 > 0) carSpeed2 = 0;
        }
    }

    // Actualizar posición del carro (solo en X y Z, Y se mantiene en -10)
    carPosition1.x += sin(carRotation1) * carSpeed1 * deltaTime;
    carPosition1.z += cos(carRotation1) * carSpeed1 * deltaTime;

    carPosition2.x += sin(carRotation2) * carSpeed2 * deltaTime;
    carPosition2.z += cos(carRotation2) * carSpeed2 * deltaTime;

    float cameraDistance = glm::mix(6.0f, 12.0f, glm::clamp(abs(carSpeed1) / maxSpeed, 0.0f, 1.0f));

    // ==== JUGADOR 1 ====
    float cameraHeight = -2.0f;    // más baja para una vista más horizontal

    glm::vec3 behindDirection = glm::normalize(glm::vec3(
        sin(carRotation1),
        0.0f,
        cos(carRotation1)
    ));
    glm::vec3 targetCameraPos = carPosition1 - behindDirection * cameraDistance;
    targetCameraPos.y += cameraHeight;

    actualCameraPos1 = glm::mix(actualCameraPos1, targetCameraPos, 5.0f * deltaTime);

    cameraJugador1.Position = actualCameraPos1;
    cameraJugador1.Front = glm::normalize(carPosition1 + glm::vec3(0.0f, 1.5f, 0.0f) - actualCameraPos1);  // Mira al carro, ligeramente arriba
    cameraJugador1.Up = glm::vec3(0.0f, 1.0f, 0.0f);

    // ==== JUGADOR 2 ====
    float cameraHeight2 = -2.0f;    // más baja para una vista más horizontal

    glm::vec3 behindDirection2 = glm::normalize(glm::vec3(
        sin(carRotation2),
        0.0f,
        cos(carRotation2)
    ));
    glm::vec3 targetCameraPos2 = carPosition2 - behindDirection2 * cameraDistance;
    targetCameraPos2.y += cameraHeight2;

    actualCameraPos2 = glm::mix(actualCameraPos2, targetCameraPos2, 5.0f * deltaTime);

    cameraJugador2.Position = actualCameraPos2;
    cameraJugador2.Front = glm::normalize(carPosition2 + glm::vec3(0.0f, 1.5f, 0.0f) - actualCameraPos2);  // Mira al carro, ligeramente arriba
    cameraJugador2.Up = glm::vec3(0.0f, 1.0f, 0.0f);

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    cameraJugador1.ProcessMouseMovement(xoffset, yoffset);
    cameraJugador2.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    cameraJugador1.ProcessMouseScroll(yoffset);
    cameraJugador2.ProcessMouseScroll(yoffset);
}
