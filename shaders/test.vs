#version 110

attribute vec2 vertexIn;

void main() {
    gl_Position = vec4(vertexIn, 0.0, 1.0);
}
