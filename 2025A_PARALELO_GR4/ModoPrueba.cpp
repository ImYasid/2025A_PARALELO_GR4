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


const float maxSpeed = 15.0f;
const float acceleration = 5.0f;
const float deceleration = 12.0f;
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

        float cameraDistance = 5.0f;
        float cameraHeight = 5.0f;

        // ==================== JUGADOR 1 (vista superior) ====================
        glViewport(0, 0, SCR_WIDTH / 2, SCR_HEIGHT);

        glm::vec3 camPos1 = carPosition1 - glm::vec3(sin(carRotation1) * cameraDistance, cameraHeight, cos(carRotation1) * cameraDistance);
        camPos1.y += 40.0f;
        actualCameraPos1 = glm::mix(actualCameraPos1, camPos1, 5.0f * deltaTime);
        cameraJugador1.Position = actualCameraPos1;
        cameraJugador1.Front = glm::normalize(carPosition1 - cameraJugador1.Position);
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

        glm::vec3 camPos2 = carPosition2 - glm::vec3(sin(carRotation2) * cameraDistance, cameraHeight, cos(carRotation2) * cameraDistance);
        camPos2.y += 40.0f;
        actualCameraPos2 = glm::mix(actualCameraPos2, camPos2, 5.0f * deltaTime);
        cameraJugador2.Position = actualCameraPos2;
        cameraJugador2.Front = glm::normalize(carPosition2 - cameraJugador2.Position);
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
        // Model matrix del vehículo 1
        glm::mat4 car1ModelMatrix = glm::mat4(1.0f);
        car1ModelMatrix = glm::translate(car1ModelMatrix, carPosition1);
        car1ModelMatrix = glm::rotate(car1ModelMatrix, carRotation1, glm::vec3(0.0f, 1.0f, 0.0f));
        car1ModelMatrix = glm::scale(car1ModelMatrix, glm::vec3(0.8f));

        // Model matrix del vehículo 2
        glm::mat4 car2ModelMatrix = glm::mat4(1.0f);
        car2ModelMatrix = glm::translate(car2ModelMatrix, carPosition2);
        car2ModelMatrix = glm::rotate(car2ModelMatrix, carRotation2, glm::vec3(0.0f, 1.0f, 0.0f));
        car2ModelMatrix = glm::scale(car2ModelMatrix, glm::vec3(0.8f));

        // Model matrix de la pista (DRT)
        glm::mat4 trackModelMatrix = glm::mat4(1.0f);
        trackModelMatrix = glm::translate(trackModelMatrix, glm::vec3(0.0f, -10.0f, 0.0f));
        trackModelMatrix = glm::scale(trackModelMatrix, glm::vec3(0.3f));

        // Obtener bounding boxes transformadas
        BoundingBox car1Box = ourModelVehiculo1.getTransformedBoundingBox(car1ModelMatrix);
        BoundingBox car2Box = ourModelVehiculo2.getTransformedBoundingBox(car2ModelMatrix);
        BoundingBox trackBox = ourModelDRT.getTransformedBoundingBox(trackModelMatrix);

        // Verificar colisiones
        if (Model::checkCollision(car1Box, car2Box)) {
            // Aplicar rebote o detener los autos
            carSpeed1 = -carSpeed1 * 0.5f;
            carSpeed2 = -carSpeed2 * 0.5f;
        }

        if (!Model::checkCollision(car1Box, trackBox)) {
            // Reducir velocidad cuando está fuera de pista
            carSpeed1 *= 0.95f;
        }

        if (!Model::checkCollision(car2Box, trackBox)) {
            // Reducir velocidad cuando está fuera de pista
            carSpeed2 *= 0.95f;
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

    // ==== JUGADOR 1 ====
    float cameraDistance1 = 5.0f;
    float cameraHeight1 = 5.0f;
    glm::vec3 targetCameraPos1 = carPosition1 - glm::vec3(
        sin(carRotation1) * cameraDistance1,
        cameraHeight1,
        cos(carRotation1) * cameraDistance1);
    targetCameraPos1.y += 40.0f;

    actualCameraPos1 = glm::mix(actualCameraPos1, targetCameraPos1, 5.0f * deltaTime);

    cameraJugador1.Position = actualCameraPos1;
    cameraJugador1.Front = glm::normalize(carPosition1 - cameraJugador1.Position);
    cameraJugador1.Up = glm::vec3(0.0f, 1.0f, 0.0f);

    // ==== JUGADOR 2 ====
    float cameraDistance2 = 5.0f;
    float cameraHeight2 = 5.0f;
    glm::vec3 targetCameraPos2 = carPosition2 - glm::vec3(
        sin(carRotation2) * cameraDistance2,
        cameraHeight2,
        cos(carRotation2) * cameraDistance2);
    targetCameraPos2.y += 40.0f;

    actualCameraPos2 = glm::mix(actualCameraPos2, targetCameraPos2, 5.0f * deltaTime);

    cameraJugador2.Position = actualCameraPos2;
    cameraJugador2.Front = glm::normalize(carPosition2 - cameraJugador2.Position);
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
