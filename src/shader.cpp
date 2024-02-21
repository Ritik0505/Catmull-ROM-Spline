
#include<iostream>
#include"shader.h"
#include<string>
#include<fstream>
#include<sstream>
#include<glad/glad.h>

Shader::Shader(const char** shaderPaths, const GLenum* shaderTypes, int numShaders)
{
    GLuint* shaders = new GLuint[numShaders];

    // Compile each shader
    for (int i = 0; i < numShaders; ++i)
    {
        std::string shaderCode;
        std::ifstream shaderFile;

        shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            shaderFile.open(shaderPaths[i]);

            std::stringstream shaderStream;
            shaderStream << shaderFile.rdbuf();
            shaderFile.close();

            shaderCode = shaderStream.str();
        }
        catch (std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ";
        }

        const char* shaderCodeCStr = shaderCode.c_str();

        shaders[i] = glCreateShader(shaderTypes[i]);
        glShaderSource(shaders[i], 1, &shaderCodeCStr, NULL);
        glCompileShader(shaders[i]);
        CheckCompilationError(shaders[i], "SHADER", shaderTypes[i]);
    }
    
    // Create shader program
    m_id = glCreateProgram();
    for (int i = 0; i < numShaders; ++i)
    {
        glAttachShader(m_id, shaders[i]);
    }
    glLinkProgram(m_id);
    CheckCompilationError(m_id, "PROGRAM");

    // Clean up
    for (int i = 0; i < numShaders; ++i)
    {
        glDetachShader(m_id, shaders[i]);
        glDeleteShader(shaders[i]);
    }
    delete[] shaders;
}
void Shader::CheckCompilationError(unsigned int shader, const std::string& type, const int shader_type)
{
    int success;
    char infoLog[1024];
    if (type == "SHADER")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
              if(shader_type == GL_TESS_CONTROL_SHADER) std::cout << "TESSALATION CONTROL SHADER :: ";
              else if(shader_type == GL_TESS_EVALUATION_SHADER) std::cout << "TESSALATION EVALUATION SHADER :: ";
              else  if(shader_type == GL_VERTEX_SHADER) std::cout << "VERTEX SHADER :: ";
              else  if(shader_type == GL_GEOMETRY_SHADER) std::cout << "GEOMTRY SHADER :: ";
              else if(shader_type == GL_FRAGMENT_SHADER) std::cout << "FRAGMENT SHADER :: ";
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR_TYPE_" << type <<std::endl << infoLog << std::endl;
        }
    }
    else if (type == "PROGRAM")
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR_TYPE_" << type << std::endl << infoLog << std::endl;
        }
    }
}

void Shader::useShader()
{
    glUseProgram(m_id);
}

GLuint Shader::getProgramID(){
  return m_id;
}
