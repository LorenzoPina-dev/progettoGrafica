#include "myShaderSelect.h"
#include "utilities.h"
#include "common.h"
#include "../base/glm/glm/gtc/type_ptr.hpp"


void MyShaderClassSelection::set_model_transform(const glm::mat4 &transform) {
  glUniformMatrix4fv(_model_transform_location, 1, GL_FALSE, const_cast<float *>(&transform[0][0]));       
}

void MyShaderClassSelection::set_camera_transform(const glm::mat4 &transform) {
  glUniformMatrix4fv(_camera_transform_location, 1, GL_FALSE, const_cast<float *>(&transform[0][0]));       
}


void MyShaderClassSelection::set_projection_transform(const glm::mat4 &projection)
{
   glUniformMatrix4fv(_projection_transform_location, 1, GL_FALSE, glm::value_ptr(projection));

}

void MyShaderClassSelection::set_base_object_id(unsigned int id) {
    glUniform1ui(_base_object_id_location, id);
}

void MyShaderClassSelection::enableFBO()
{
    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);
  glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    //glViewport(0, 0, width, height);
}

void MyShaderClassSelection::set_render_mode(int mode) {
    glUniform1i(_render_mode_location, mode);
    if(mode==0)
      glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    else if(mode==1)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
    else
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
}

void MyShaderClassSelection::bindTexture()
{
  
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    //glBindTexture(GL_TEXTURE_2D, TextureSel);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
}

bool MyShaderClassSelection::load_shaders() {
    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);
    glGenFramebuffers(1,&FBO);
    glBindFramebuffer(GL_FRAMEBUFFER,FBO);
    glGenTextures(1,&TextureSel);
    glBindTexture(GL_TEXTURE_2D,TextureSel);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32UI, width, height, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureSel, 0);
    
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer error: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
}
glBindFramebuffer(GL_FRAMEBUFFER,0);
  return  add_shader(GL_VERTEX_SHADER,"shader/select.vert") &&
           add_shader(GL_GEOMETRY_SHADER, "shader/select.geom") &&
          add_shader(GL_FRAGMENT_SHADER,"shader/select.frag");
}

bool MyShaderClassSelection::load_done() {
  _model_transform_location = get_uniform_location("Model2World");
  _camera_transform_location = get_uniform_location("World2Camera");

  _base_object_id_location     = get_uniform_location("objectID");
 
  _projection_transform_location = get_uniform_location("ProjectionMatrix");
  
    _render_mode_location = get_uniform_location("renderMode");
  // Imposto il sampler con ID 0 per leggere la texture colore
  glUniform1i(get_uniform_location("ColorTextSampler"), TEXTURE_COLOR);

  return  (_model_transform_location != INVALID_UNIFORM_LOCATION) &&
          (_camera_transform_location != INVALID_UNIFORM_LOCATION) &&
          (_base_object_id_location != INVALID_UNIFORM_LOCATION) &&
          (_projection_transform_location != INVALID_UNIFORM_LOCATION) &&
          (_render_mode_location != INVALID_UNIFORM_LOCATION);
}
