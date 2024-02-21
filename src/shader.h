#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glad/glad.h>

class Shader {
public:
    Shader(const char** shaderPaths, const GLenum* shaderTypes, int numShaders);
    void useShader();
    GLuint getProgramID();

private:
    GLuint m_id;

    void CheckCompilationError(unsigned int shader, const std::string& type, const int shader_type = -1);
};

#endif // SHADER_H

