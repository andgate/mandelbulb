#version 400

uniform mat4 projViewMatrix;

in vec4 position;
in vec3 color;

out vec3 Color;

// Simple passthrough vertex shader... Nothing fancy here.
void main()
{
    Color = color;
    gl_Position = projViewMatrix * position;
}