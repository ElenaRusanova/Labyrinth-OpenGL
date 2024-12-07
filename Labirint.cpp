#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Параметры окна
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


// Камера
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

bool firstMouse = true;

// Тайминги
float deltaTime = 0.0f;	// время между текущим кадром и последним кадром
float lastFrame = 0.0f;



#pragma region VERTEX

// Задание вершин (и буфера(ов)) и настройка вершинных атрибутов
float vertices[] = {
    // координаты        // текстурные координаты
   -0.3f, -0.3f, -0.3f,  0.0f, 0.0f,
    0.3f, -0.3f, -0.3f,  1.0f, 0.0f,
    0.3f,  0.3f, -0.3f,  1.0f, 1.0f,
    0.3f,  0.3f, -0.3f,  1.0f, 1.0f,
   -0.3f,  0.3f, -0.3f,  0.0f, 1.0f,
   -0.3f, -0.3f, -0.3f,  0.0f, 0.0f,

   -0.3f, -0.3f,  0.3f,  0.0f, 0.0f,
    0.3f, -0.3f,  0.3f,  1.0f, 0.0f,
    0.3f,  0.3f,  0.3f,  1.0f, 1.0f,
    0.3f,  0.3f,  0.3f,  1.0f, 1.0f,
   -0.3f,  0.3f,  0.3f,  0.0f, 1.0f,
   -0.3f, -0.3f,  0.3f,  0.0f, 0.0f,

   -0.3f,  0.3f,  0.3f,  1.0f, 0.0f,
   -0.3f,  0.3f, -0.3f,  1.0f, 1.0f,
   -0.3f, -0.3f, -0.3f,  0.0f, 1.0f,
   -0.3f, -0.3f, -0.3f,  0.0f, 1.0f,
   -0.3f, -0.3f,  0.3f,  0.0f, 0.0f,
   -0.3f,  0.3f,  0.3f,  1.0f, 0.0f,

    0.3f,  0.3f,  0.3f,  1.0f, 0.0f,
    0.3f,  0.3f, -0.3f,  1.0f, 1.0f,
    0.3f, -0.3f, -0.3f,  0.0f, 1.0f,
    0.3f, -0.3f, -0.3f,  0.0f, 1.0f,
    0.3f, -0.3f,  0.3f,  0.0f, 0.0f,
    0.3f,  0.3f,  0.3f,  1.0f, 0.0f,

   -0.3f, -0.3f, -0.3f,  0.0f, 1.0f,
    0.3f, -0.3f, -0.3f,  1.0f, 1.0f,
    0.3f, -0.3f,  0.3f,  1.0f, 0.0f,
    0.3f, -0.3f,  0.3f,  1.0f, 0.0f,
   -0.3f, -0.3f,  0.3f,  0.0f, 0.0f,
   -0.3f, -0.3f, -0.3f,  0.0f, 1.0f,

   -0.3f,  0.3f, -0.3f,  0.0f, 1.0f,
    0.3f,  0.3f, -0.3f,  1.0f, 1.0f,
    0.3f,  0.3f,  0.3f,  1.0f, 0.0f,
    0.3f,  0.3f,  0.3f,  1.0f, 0.0f,
   -0.3f,  0.3f,  0.3f,  0.0f, 0.0f,
   -0.3f,  0.3f, -0.3f,  0.0f, 1.0f
};

// Мировые координаты наших кубиков
glm::vec3 cubePositions[] = {
    glm::vec3(-2.0f,  0.0f, -4.0f),
    glm::vec3(-2.0f,  0.0f, -4.6f),
    glm::vec3(-2.0f,  0.0f, -5.2f),
    glm::vec3(-1.4f,  0.0f, -5.2f),
    glm::vec3(-0.8f,  0.0f, -5.2f),
    glm::vec3(-0.2f,  0.0f, -5.2f),
    glm::vec3(0.4f,  0.0f, -5.2f),
    glm::vec3(1.0f,  0.0f, -5.2f),
    glm::vec3(1.0f,  0.0f, -4.6f),
    glm::vec3(1.0f,  0.0f, -4.0f),
    glm::vec3(1.6f,  0.0f, -4.0f)
};

unsigned int indices[] = {
    0, 1, 3, // первый треугольник
    1, 2, 3  // второй треугольник
};

#pragma endregion

// Столкновение
const float cubeSize = 0.6f;  
const float cameraRadius = 0.3f;  

bool checkCollision(const glm::vec3& position, const glm::vec3& cubePos, float cubeSize, float cameraRadius) {
    return (abs(position.x - cubePos.x) < (cubeSize / 2 + cameraRadius)) &&
        (abs(position.y - cubePos.y) < (cubeSize / 2 + cameraRadius)) &&
        (abs(position.z - cubePos.z) < (cubeSize / 2 + cameraRadius));
}

bool isColliding(const glm::vec3& newPosition) {
    for (const auto& cubePos : cubePositions) {
        if (checkCollision(newPosition, cubePos, cubeSize, cameraRadius)) {
            return true;
        }
    }
    return false;
}

int main()
{

#pragma region INIZIALIZ
    // Инициализация GLFW и создание окна
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Maze", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Сообщаем GLFW, чтобы он захватил наш курсор
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Загрузка всех указателей на OpenGL-функции
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Конфигурирование глобального состояния OpenGL
    glEnable(GL_DEPTH_TEST);

    // Компилирование шейдеров
    Shader ourShader("shader/shader.vert", "shader/shader.frag");
#pragma endregion

#pragma region TEXTURE
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Устанавливаем параметры наложения и фильтрации текстур (для текущего связанного объекта текстуры)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Загрузка и генерация текстуры
    int width, height, nrChannels;
    unsigned char* data = stbi_load("image/images.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

#pragma endregion

#pragma region VAO,VBO
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Координатные атрибуты
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Цветовые атрибуты
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


#pragma endregion

    // В цикле рендеринга
    while (!glfwWindowShouldClose(window))
    {

        // Логическая часть работы со временем для каждого кадра
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        // Обработка ввода
        processInput(window);

        // Обновление прыжка
        camera.UpdateJump(deltaTime);

        // Рендеринг
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // очищаем буфер цвета и буфер глубины
        
        glBindTexture(GL_TEXTURE_2D, texture);

        ourShader.use();

      
        // Передаем шейдеру матрицу проекции
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // Создаем преобразование камеры/вида
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        // рендерим ящики
        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 11; i++)
        {
            // вычисляем матрицу модели для каждого объекта и передаём ее в шейдер до отрисовки
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i + (float)glfwGetTime() * 20;
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            ourShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Очистка ресурсов
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

// Обработка ввода
void processInput(GLFWwindow* window)
/*
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !camera.isJumping) {
        camera.Jump();
    }
}
*/

{
    glm::vec3 proposedPosition = camera.Position;  // Сохраняем текущую позицию камеры для проверки
    glm::vec3 horizontalPosition = camera.Position; // Позиция для проверки столкновений в плоскости XZ
    glm::vec3 verticalPosition = camera.Position;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        horizontalPosition += glm::vec3(camera.Front.x, 0.0f, camera.Front.z) * deltaTime * camera.MovementSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        horizontalPosition -= glm::vec3(camera.Front.x, 0.0f, camera.Front.z) * deltaTime * camera.MovementSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        horizontalPosition -= glm::vec3(camera.Right.x, 0.0f, camera.Right.z) * deltaTime * camera.MovementSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        horizontalPosition += glm::vec3(camera.Right.x, 0.0f, camera.Right.z) * deltaTime * camera.MovementSpeed;
    }

    // Проверка на столкновение для горизонтального перемещения
    if (!isColliding(horizontalPosition)) {
        proposedPosition.x = horizontalPosition.x;
        proposedPosition.z = horizontalPosition.z;
    
    }

    // Обработка прыжка по оси Y (без проверки столкновения)
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !camera.isJumping) {
        proposedPosition.y += deltaTime * camera.MovementSpeed;  // Задайте подходящую скорость прыжка
        camera.Jump();
    }
    
    // Проверка столкновений для движения по оси Y
    if (!isColliding(glm::vec3(camera.Position.x, verticalPosition.y, camera.Position.z))) {
        proposedPosition.y = verticalPosition.y;
    }
    else {
        camera.isJumping = false; // Останавливаем прыжок при столкновении
    }

    camera.Position = proposedPosition;
}



// Callback для изменения размера окна
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// glfw: всякий раз, когда перемещается мышь, вызывается данная callback-функция
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // перевернуто, так как y-координаты идут снизу вверх

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: всякий раз, когда прокручивается колесико мыши, вызывается данная callback-функция
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

