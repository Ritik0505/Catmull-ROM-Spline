#version 410 core

// Specify the input layout as isolines
layout(isolines, equal_spacing, ccw) in;

// Uniform variable for the Model-View-Projection matrix
uniform mat4 u_MVP; // projection * view * model

// Uniform variable for the tessellation parameter alpha
uniform float alpha;

struct Segment
{
    vec2 a;
    vec2 b;
    vec2 c;
    vec2 d;
};


vec2 interpolatePoint(vec2 p0, vec2 p1, vec2 p2, vec2 p3, float t) {
    float t0 = 0.0;
    float t1 = t0 + pow(distance(p0, p1), alpha);
    float t2 = t1 + pow(distance(p1, p2), alpha);
    float t3 = t2 + pow(distance(p2, p3), alpha);

    float t01 = pow(distance(p0, p1), alpha);
    float t12 = pow(distance(p1, p2), alpha);
    float t23 = pow(distance(p2, p3), alpha);

    float tension = 0.5f;
    
    vec2 m1 = (1.0f - tension) * (p2 - p1 + t12 * ((p1 - p0) / t01 - (p2 - p0) / (t01 + t12)));
    vec2 m2 = (1.0f - tension) * (p2 - p1 + t12 * ((p3 - p2) / t23 - (p3 - p1) / (t12 + t23)));

    Segment segment;
    segment.a = 2.0f * (p1 - p2) + m1 + m2;
    segment.b = -3.0f * (p1 - p2) - m1 - m1 - m2;
    segment.c = m1;
    segment.d = p1;

    vec2 point = segment.a * t * t * t + segment.b * t * t + segment.c * t + segment.d;
    return point;
}

void main() {
    // The tessellation u coordinate
    float t = gl_TessCoord.x;

    // The patch vertices (control points)
    vec2 p0 = gl_in[0].gl_Position.xy;
    vec2 p1 = gl_in[1].gl_Position.xy;
    vec2 p2 = gl_in[2].gl_Position.xy;
    vec2 p3 = gl_in[3].gl_Position.xy;

    // Interpolate the point on the curve
    vec2 point = interpolatePoint(p0, p1, p2, p3, t);

    // Transform the interpolated point by MVP matrix
    gl_Position =  vec4(point, 0.0f, 1.0);
}
