#include "Includes.h"
INITIALIZE_EASYLOGGINGPP


// settings

unsigned int fb, color, depth;
unsigned int tex;
void GetRandTexture(){
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    time_t t;
    srand((unsigned) time(&t));
    float data[SCR_WIDTH*SCR_HEIGHT*4];
    for(int i = 0; i < SCR_HEIGHT*SCR_WIDTH*4; i+=4 ){
        int num = rand()%255;
        //std::cout << num << "\n";
        data[i] = (float )num/255;
        data[i+1] = (float )num/255;
        data[i+2] = (float)num/255;
        data[i+3] = (float)num/255;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, data);

    glBindImageTexture(0, tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    //glActiveTexture(GL_TEXTURE0);
   // glBindTexture(GL_TEXTURE_2D, tex);
   // glBindImageTexture(0, tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGB);
   // glActiveTexture(GL_TEXTURE0);
   // glBindTexture(GL_TEXTURE_2D, tex);
};

vec3 translation = vec3(0, 0, 0);

void Handle_Controls(GLFWwindow* window){
    float speed = 10;
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        translation.x -=speed;
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        translation.x +=speed;
    }
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
        translation.y +=speed;
    }
    if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
        translation.y -=speed;
    }
}
int main()
{
    Program::Begin();
    GetRandTexture();
   //basis
    vec3 x(1, 0, 0);
    vec3 y(0, 1, 0);
    vec3 z(0, 0, 1);
    vec4 axis = vec4(normalize(vec3(1, 1, 1)), 0.0045); // axis and rotation speed
    auto basis = glm::mat3(1) ;
    while (!glfwWindowShouldClose(window))
    {
        Program::processInput(window);
        // render
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        glUseProgram(shaderProgram.PID);
        glUniform1f(glGetUniformLocation(shaderProgram.PID, "time"), glfwGetTime());
        glUniformMatrix3fv(glGetUniformLocation(shaderProgram.PID, "basis"),1, GL_FALSE, value_ptr(basis));
        glUniform3fv(glGetUniformLocation(shaderProgram.PID, "translation"),1,value_ptr(translation));
        //top of the screen will be random noise cuz division fucks up but whatever
        glDispatchCompute(SCR_WIDTH/16, SCR_HEIGHT/16, 1 );

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        glUseProgram(shaderProgram.ID);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind textures on corresponding texture units
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, tex);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        x = Rotations::Rotate(normalize(axis), x);
        y = Rotations::Rotate(normalize(axis), y);
        z = Rotations::Rotate(normalize(axis), z);
        basis[0] = x;
        basis[1] = y;
        basis[2] = z;

        Handle_Controls(window);
    }
    // optional: de-allocate all resources once they've outlived their purpose (OR DONT???):
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram.ID);

    // glfw: terminate, clearing all previously allocated GLFW resources (yeah yeah blah blah..).
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
