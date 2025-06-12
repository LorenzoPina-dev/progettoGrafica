// myShaderWire.cpp
#include "myShaderWire.h"
#include "../base/glm/glm/gtc/type_ptr.hpp"
#include <iostream>

bool MyShaderWire::load_shaders() {
    return add_shader(GL_VERTEX_SHADER,   "shader/wire.vert") &&
           add_shader(GL_GEOMETRY_SHADER, "shader/wire.geom") &&
           add_shader(GL_FRAGMENT_SHADER, "shader/wire.frag");
}

bool MyShaderWire::load_done() {
    _model_transform_location = get_uniform_location("Model2World");
    _camera_transform_location = get_uniform_location("World2Camera");
    _projection_transform_location = get_uniform_location("ProjectionMatrix");
    _render_mode_location = get_uniform_location("renderMode");

    return (_model_transform_location != INVALID_UNIFORM_LOCATION) &&
           (_camera_transform_location != INVALID_UNIFORM_LOCATION) &&
           (_projection_transform_location != INVALID_UNIFORM_LOCATION) &&
           (_render_mode_location != INVALID_UNIFORM_LOCATION)&&
           (_camera_position_location != INVALID_UNIFORM_LOCATION);
}

void MyShaderWire::set_model_transform(const glm::mat4 &transform) {
    glUniformMatrix4fv(_model_transform_location, 1, GL_FALSE, glm::value_ptr(transform));
}

void MyShaderWire::set_camera_transform(const glm::mat4 &transform) {
    glUniformMatrix4fv(_camera_transform_location, 1, GL_FALSE, glm::value_ptr(transform));
}

void MyShaderWire::set_projection_transform(const glm::mat4 &projection) {
    glUniformMatrix4fv(_projection_transform_location, 1, GL_FALSE, glm::value_ptr(projection));
}

void MyShaderWire::set_render_mode(int mode) {
    //std::cout<<"modalita scelta: "<<mode<<std::endl;
    glUniform1i(_render_mode_location, mode);
}


void MyShaderWire::set_camera_position(const glm::vec3 &pos) {
  glUniform3fv(_camera_position_location, 1, const_cast<float *>(&pos[0]));
}