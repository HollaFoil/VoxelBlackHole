//
// Created by nedas on 17-11-23.
//

#ifndef BLACKHOLE_SHADER_H
#define BLACKHOLE_SHADER_H
#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include<sstream>
#include "../glad/glad.h"
#include <GLFW/glfw3.h>

using namespace std;

class Shader {
public:
    unsigned int ID, PID;
    Shader(const char* inputVertex, const char* inputFragment);
    Shader(const char* inputVertex, const char* inputFragment, const char* inputCompute);

    static char* Parse(const string& input);
    static void BuildShaders(unsigned int &shader, const char* source, uint32_t shader_type);
    static void Link(unsigned int&program);
};

#endif //BLACKHOLE_SHADER_H
