#version 410 core
layout (location = 0 ) in vec2 VertexPosition;
uniform mat4 MVP;

void main() {
       gl_Position = vec4(VertexPosition, 0.0, 1.0); 
}
