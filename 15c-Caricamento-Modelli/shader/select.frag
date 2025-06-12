
#version 330 core
layout(location = 0) out uvec4 FragColor;

flat in uvec4 FinalId;

void main() {
    FragColor = FinalId;
}
