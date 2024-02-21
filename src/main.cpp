#include "shader.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

std::vector<float> readVertexDataFromFile(const std::string& filename) {
    std::vector<float> vertices;
    std::ifstream file(filename);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string token;
            while (std::getline(iss, token, ',')) {
                float value = std::stof(token);
                vertices.push_back(value);
            }
        }
        file.close();
    } else {
        std::cerr << "Failed to open file: " << filename << std::endl;
    }
    return vertices;
}


int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window =
        glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Catmull-ROM Spline", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");
    ImGui::StyleColorsDark();

    const char *shaderPaths[] = {
        "./src/shaders/vertex.vert", "./src/shaders/fragment.frag",
        "./src/shaders/tcs.tesc", "./src/shaders/tes.tese", "./src/shaders/geometry.geom"};
    GLenum shaderTypes[] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER};
    Shader myShader(shaderPaths, shaderTypes, 5);

    std::vector<float> vertices = readVertexDataFromFile("./src/data/vertices.txt");
    for(int i=0; i < vertices.size();i += 1){
      std::cout << vertices[i] <<" ,"<<"\n";
    }
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    // Create the orthographic projection matrix
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 10.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInput(window);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glBindVertexArray(VAO); 
        myShader.useShader();

        // ImGui window for uniform values
        ImGui::Begin("Uniform Values");

        static float numSegments = 4.0f;
        ImGui::SliderFloat("NumSegments", &numSegments, 0.0f, 20.0f);

        static float alpha = 0.5f;
        ImGui::SliderFloat("alpha", &alpha, 0.0f, 1.0f);

        static float extrusion = 0.01f;
        ImGui::SliderFloat("Extrusion Width", &extrusion, 0.01f, 0.1f);

        static bool extrude = false;
        ImGui::Checkbox("Extrude", &extrude);

        if (ImGui::Button("Apply Changes")) {
            int location = glGetUniformLocation(myShader.getProgramID(), "NumSegments");
            glUniform1f(location, numSegments);

            location = glGetUniformLocation(myShader.getProgramID(), "alpha");
            glUniform1f(location, alpha);

            location = glGetUniformLocation(myShader.getProgramID(), "extrude");
            glUniform1i(location, extrude ? 1 : 0);

            location = glGetUniformLocation(myShader.getProgramID(), "extrusionWidth");
            glUniform1f(location, extrusion);
        }

        ImGui::End();

        // Draw patches of four vertices
        int numVertices = vertices.size() / 2.0f;
        for (int i = 0; i <= numVertices - 4; i++) {
            glPatchParameteri(GL_PATCH_VERTICES, 4);
            glDrawArrays(GL_PATCHES, i, 4);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}
