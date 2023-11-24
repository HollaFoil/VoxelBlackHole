//
// Created by dominykas on 24-11-23.
//
#pragma once
#ifndef BLACKHOLE_PROGRAM_H
#define BLACKHOLE_PROGRAM_H

#endif //BLACKHOLE_PROGRAM_H

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
unsigned int VBO, VAO, EBO;
GLFWwindow* window;
Shader shaderProgram;

namespace Program{

    void processInput(GLFWwindow *window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }
    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }
    void InitializeQuad(){
        float vertices[] = {
                // positions          // colors           // texture coords
                1.0f,  1.0f, 0.0f,
                1.0f, -1.0f, 0.0f,
                -1.0f, -1.0f, 0.0f,
                -1.0f,  1.0f, 0.0f
        };
        unsigned int indices[] = {
                0, 1, 3, // first triangle
                1, 2, 3  // second triangle
        };

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

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        glBindVertexArray(0);

    }
    void Begin(){
        // ------------------------------
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Voxels", NULL, NULL);
        if (window == NULL)
        {
            LOG(ERROR) << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            throw;
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            LOG(ERROR) << "Failed to initialize GLAD" << std::endl;
            throw;
        }
        shaderProgram = {"./assets/vertex.glsl", "./assets/fragment.glsl", "./assets/compute.glsl"};
        InitializeQuad();
    }

}