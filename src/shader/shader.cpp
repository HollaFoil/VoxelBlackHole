//
// Created by nedas on 17-11-23.
//

#pragma once
#include "shader.h"
#include "../log/easylogging++.h"


Shader::Shader(const char *inputVertex, const char *inputFragment) {
    PID = -1;
    LOG(INFO) << "Start loading shader files";

    unsigned int vertex, fragment;
    const char* source = Shader::Parse(inputVertex);
    const char* source2 = Shader::Parse(inputFragment);

    LOG(INFO) << "Building vertex shader";
    BuildShaders(vertex, source, GL_VERTEX_SHADER);
    LOG(INFO) << "Building fragment shader";
    BuildShaders(fragment, source2, GL_FRAGMENT_SHADER);

    LOG(INFO) << "Creating shader program";
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    Link(ID);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}
Shader::Shader(const char *inputVertex, const char *inputFragment, const char* inputCompute) {
    unsigned int vertex, fragment, compute;
    const char* source = Shader::Parse(inputVertex);
    const char* source2 = Shader::Parse(inputFragment);
    const char* source3 = Shader::Parse(inputCompute);

    BuildShaders(vertex, source, GL_VERTEX_SHADER);
    BuildShaders(fragment, source2, GL_FRAGMENT_SHADER);
    BuildShaders(compute, source3, GL_COMPUTE_SHADER);

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    Link(ID);

    PID = glCreateProgram();
    glAttachShader(PID, compute);
    Link(PID);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glDeleteShader(compute);
}
//converts input string to char array
char* Shader::Parse(const std::string& input){
    LOG(INFO) << "Parsing shader file: " << input;
    std::ifstream in(input);
    if(!in.is_open()) {
        LOG(ERROR) << "File not found.";
        throw;
    }
    std::stringstream buffer;
    buffer << in.rdbuf();
    std::string s = buffer.str();
    char* out = (char*)malloc(sizeof(char)*(s.length()+1));
    for(int i = 0; i < s.length(); i ++){
        out[i] = s[i];
    }
    out[s.length()] = '\0';
    return out;
}

void Shader::BuildShaders(unsigned int &shader, const char* source, uint32_t shader_type){
    shader = glCreateShader(shader_type);
    glShaderSource(shader, 1,  &source, nullptr);
    int success;
    char err[512];
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(shader, 512, nullptr, err);
        std::string shaderType = shader_type == GL_VERTEX_SHADER ? " (Vertex) " :
                        shader_type == GL_FRAGMENT_SHADER ? " (Fragment) " :
                        shader_type == GL_COMPUTE_SHADER ? " (Compute) " : " (Unknown shader type) ";
        LOG(ERROR) << "Shader compilation failed" << shaderType << err << "\n";
        throw;
    }
}
void Shader::Link(unsigned int&program){
    glLinkProgram(program);
    int success;
    char err[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(program, 512, nullptr, err);
        LOG(ERROR) << "Shader linking failed" << err << "\n";
        throw;
    }
}