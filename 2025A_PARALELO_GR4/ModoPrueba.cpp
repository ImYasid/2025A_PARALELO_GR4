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
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

// camera
Camera camera(glm::vec3(0.0f, -5.0f, 5.0f)); // Posición inicial más alta

// variables para el control del vehículo
glm::vec3 carPosition(0.0f, -10.0f, 0.0f); // Y = -10 para que esté sobre la pista
float carRotation = 0.0f;
float carSpeed = 0.0f;
const float maxSpeed = 20.0f;
const float acceleration = 5.0f;
const float deceleration = 7.0f;
const float rotationSpeed = 2.0f;
glm::vec3 actualCameraPos = camera.Position;

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
    Model ourModelVehiculo("models/vehiculos/Lamborini_countach/Lamborgini_countach.obj");
    Model ourModelVehiculo1("models/carro1/carro1.obj");

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

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.08f, 300.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // iluminación nocturna
        ourShader.setVec3("lightColor", glm::vec3(0.2f, 0.2f, 0.4f));
        ourShader.setVec3("lightPos", glm::vec3(0.0f, 10.0f, 0.0f));
        ourShader.setVec3("viewPos", camera.Position);
        ourShader.setVec3("emissionColor", glm::vec3(0.5f, 0.5f, 0.5f));

        // modelo 1: DriftTrack
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -10.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.3f));
        ourShader.setMat4("model", model);
        ourModelDRT.Draw(ourShader);

        // modelo 2: Nebula
        glm::mat4 model2 = glm::mat4(1.0f);
        model2 = glm::translate(model2, glm::vec3(0.0f, 0.0f, 0.0f));
        model2 = glm::scale(model2, glm::vec3(75.0f));
        ourShader.setMat4("model", model2);
        ourModelnebula.Draw(ourShader);

        // modelo 3: Vehículo (estático)
        glm::mat4 model3 = glm::mat4(1.0f);
        model3 = glm::translate(model3, glm::vec3(0.0f, -10.0f, 5.0f)); // Ajustado a Y = -10
        model3 = glm::scale(model3, glm::vec3(5.0f));
        ourShader.setMat4("model", model3);
        ourModelVehiculo.Draw(ourShader);

        // modelo 4: Vehículo (controlable)
        glm::mat4 model4 = glm::mat4(1.0f);
        model4 = glm::translate(model4, carPosition);
        model4 = glm::rotate(model4, carRotation, glm::vec3(0.0f, 1.0f, 0.0f));
        model4 = glm::scale(model4, glm::vec3(1.0f));
        ourShader.setMat4("model", model4);
        ourModelVehiculo1.Draw(ourShader);

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

    // Control del vehículo
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        carSpeed += acceleration * deltaTime;
        if (carSpeed > maxSpeed) carSpeed = maxSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        carSpeed -= acceleration * deltaTime;
        if (carSpeed < -maxSpeed / 2) carSpeed = -maxSpeed / 2;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        carRotation += rotationSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        carRotation -= rotationSpeed * deltaTime;
    }

    // Deceleración cuando no se presionan teclas
    if (glfwGetKey(window, GLFW_KEY_W) != GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_S) != GLFW_PRESS) {
        if (carSpeed > 0) {
            carSpeed -= deceleration * deltaTime;
            if (carSpeed < 0) carSpeed = 0;
        }
        else if (carSpeed < 0) {
            carSpeed += deceleration * deltaTime;
            if (carSpeed > 0) carSpeed = 0;
        }
    }

    // Actualizar posición del carro (solo en X y Z, Y se mantiene en -10)
    carPosition.x += sin(carRotation) * carSpeed * deltaTime;
    carPosition.z += cos(carRotation) * carSpeed * deltaTime;

    // Configurar cámara en tercera persona
    float cameraDistance = 5.0f;
    float cameraHeight = 5.0f;
    glm::vec3 cameraPos = carPosition - glm::vec3(sin(carRotation) * cameraDistance,
        cameraHeight,
        cos(carRotation) * cameraDistance);
    cameraPos.y += 40.0f; // Ajuste adicional de altura

    // Suavizado de movimiento de cámara
    actualCameraPos = glm::mix(actualCameraPos, cameraPos, 5.0f * deltaTime);
    camera.Position = actualCameraPos;
    camera.Front = glm::normalize(carPosition - camera.Position);
    camera.Up = glm::vec3(0.0f, 1.0f, 0.0f); // Vector up normalizado
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

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}