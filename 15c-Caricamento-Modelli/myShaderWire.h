// myShaderWire.h
#pragma once

#include "myshaderclass.h"
#include "utilities.h"
#include "common.h"
#include "../base/glm/glm/gtc/type_ptr.hpp"
class MyShaderWire : public ShaderClass {
public:
    bool load_shaders();
    bool load_done();

    void set_model_transform(const glm::mat4 &transform);
    void set_camera_transform(const glm::mat4 &transform);
    void set_projection_transform(const glm::mat4 &projection);
    void set_render_mode(int mode);
    void set_camera_position(const glm::vec3 &pos);

private:
    GLint _model_transform_location;
    GLint _camera_transform_location;
    GLint _projection_transform_location;
    GLint _render_mode_location;
    GLint _camera_position_location;
};