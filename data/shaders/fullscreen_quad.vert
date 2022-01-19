#version 450
#extension GL_ARB_separate_shader_objects : enable

void main() {
    float x = float((gl_VertexIndex & 1) << 2) - 1.0;
    float y = float((gl_VertexIndex & 2) << 1) - 1.0;
    gl_Position = vec4(y, x, 0.0, 1.0);
}