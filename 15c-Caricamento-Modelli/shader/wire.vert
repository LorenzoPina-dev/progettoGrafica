#version 330 core

layout(location = 0) in vec3 position; // Solo posizione, niente vertexID necessario
layout(location = 3) in float selectedFlag; // 1.0 se selezionato, 0.0 altrimenti
/*layout(std430, binding = 3) buffer SelectedBuffer {
    float selected[];
};*/


flat out float vSelected; // flat = non interpolato

uniform mat4 Model2World;
uniform mat4 World2Camera;
uniform mat4 ProjectionMatrix;

void main() {
    gl_Position = vec4(position, 1.0);
    vSelected=selectedFlag;//selected[gl_VertexID];
}
