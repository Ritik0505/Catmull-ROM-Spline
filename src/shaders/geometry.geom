
#version 410 core
layout (lines) in;
layout(line_strip, max_vertices = 8) out;

uniform bool extrude; 
uniform float extrusionWidth;

void main() {

    if (!extrude) {
        // If extrusion is disabled, simply pass through the input vertices
        for (int i = 0; i < 2; ++i) {
            gl_Position = gl_in[i].gl_Position;
            EmitVertex();
        }
        EndPrimitive();
        return; // Exit the shader
    }

    // Direction vectors for the isoline
    vec2 tangent = normalize(gl_in[1].gl_Position.xy - gl_in[0].gl_Position.xy);
    vec2 normal = vec2(-tangent.y, tangent.x);

    // Vertices for the quad
    vec4 v0 = gl_in[0].gl_Position + vec4(normal * extrusionWidth, 0.0, 0.0);
    vec4 v1 = gl_in[0].gl_Position - vec4(normal * extrusionWidth, 0.0, 0.0);
    vec4 v2 = gl_in[1].gl_Position + vec4(normal * extrusionWidth, 0.0, 0.0);
    vec4 v3 = gl_in[1].gl_Position - vec4(normal * extrusionWidth, 0.0, 0.0);

    // Emit vertices for the wireframe quad
    gl_Position = v0;
    EmitVertex();

    gl_Position = v1;
    EmitVertex();

    gl_Position = v2;
    EmitVertex();

    gl_Position = v3;
    EmitVertex();

    // End the first triangle
    EndPrimitive();

    // Start the second triangle
    gl_Position = v1;
    EmitVertex();

    gl_Position = v3;
    EmitVertex();

    gl_Position = v2;
    EmitVertex();

    gl_Position = v0;
    EmitVertex();

    // End the second triangle
    EndPrimitive();
    EndPrimitive();
}
