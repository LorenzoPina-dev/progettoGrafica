#version 330 core

layout(location = 0) in vec3 position;

uniform uint objectID;

flat out uint vObjectID;
flat out uint vVertexID;

uniform mat4 Model2World;
uniform mat4 World2Camera;
uniform mat4 ProjectionMatrix;

void main() {
    vObjectID = objectID;
    vVertexID = uint(gl_VertexID);

    gl_Position =  vec4(position, 1.0);
}