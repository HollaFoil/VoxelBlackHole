#include "glad/glad.h"
#include "shader/shader.h"
#include "log/easylogging++.h"
#include <GLFW/glfw3.h>

#include <iostream>
#include <format>

INITIALIZE_EASYLOGGINGPP

void framebuffer_size_callback([[maybe_unused]] GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
const unsigned int SCR_WIDTH = 1536;
const unsigned int SCR_HEIGHT = 864;
double lastTime = 0.0f;
int frameCount = 0;


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        LOG(ERROR) << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOG(ERROR) << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSwapInterval(0);

    Shader shaderProgram(  "./assets/vertex.glsl",
                         "./assets/fragment.glsl",
                         "./assets/compute.glsl");
    float vertices[] = {
            1.0f,  1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f
    };
    unsigned int indices[] = {
            0, 1, 3,
            1, 2, 3
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned int texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    int work_grp_cnt[3];

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

    LOG(INFO) << "Maximum global (total) work group counts - "
              << "x: " << work_grp_cnt[0]
              << " y: " << work_grp_cnt[1]
              << " z: " << work_grp_cnt[2];

    int work_grp_inv;
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
    LOG(INFO) << "Maximum local work group invocations - " << work_grp_inv;

    double avgSum = 0;
    while (!glfwWindowShouldClose(window))
    {

        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        avgSum += 1/deltaTime;
        if (++frameCount % 1000 == 0) {
            LOG(INFO) << "Current average fps: " << avgSum/(double)frameCount;
        }
        glUseProgram(shaderProgram.PID);
        glDispatchCompute(SCR_WIDTH/32, SCR_HEIGHT/32, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        processInput(window);


        glUseProgram(shaderProgram.ID);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram.ID);
    glfwTerminate();
    return 0;
}
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    LOG(INFO) << "Resize event: width - " << width << ", height - " << height;
    glViewport(0, 0, width, height);
}