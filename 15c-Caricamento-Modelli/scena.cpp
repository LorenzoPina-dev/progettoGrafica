#include "scena.h"
#include "../base/freeglut/include/GL/freeglut.h"
#include "../base/glew/include/GL/glew.h" // prima di freeglut
#include "../base/glm/glm/glm.hpp"
#include "../base/glm/glm/gtc/matrix_transform.hpp"
#include <algorithm>
#include <chrono>

#include "../base/glm/glm/gtc/type_ptr.hpp" // Per glm::value_ptr
#include <cstdint>
#include <iostream>
#include <iterator>
#include <ostream>

void scena::renderOverlay(int x, int y, std::string messaggio) {

  glDisable(GL_DEPTH_TEST);
  glUseProgram(0);

  // Salva attributi e matrici
  glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glColor3f(1.0f, 1.0f, 1.0f);

  glRasterPos2f(x, y);

  int lineSpacing = 14; // Altezza del font circa per GLUT_BITMAP_HELVETICA_12
  int startX = x;
  int startY = y;

  for (char c : messaggio) {
    if (c == '\n') {
      startY -= lineSpacing;
      glRasterPos2f(startX, startY);
    } else {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
  }

  // Ripristina matrici e attributi
  glPopMatrix();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glPopAttrib();

  // Riabilita depth test e shader
  glEnable(GL_DEPTH_TEST);
  myshaders.enable();
}

void scena::createScene() {
  reset();

  myshaders.init();
  myshaders.enable();
  myshaderWire.init();
  myshaderWire.enable();
  selection_shader.init();
  selection_shader.enable();
}
void scena::set(assi assi) {
  if (assi == asseOperazione)
    toggleAsse();
  this->asseOperazione = assi;
}
void scena::set(SistemaRiferimento sistemaRiferimento) {
  this->riferimento = sistemaRiferimento;
}
void scena::set(Trasformazione trasformazione) { this->t = trasformazione; }
void scena::set(SelectionMode selectMode) { mode = selectMode; }
void scena::addObject(sceneObject *o) { objects.push_back(o); }

void scena::removeObject(int o) { objects.erase(objects.cbegin() + o); }

void scena::removeObject(sceneObject *o) {
  int pos = std::distance(objects.begin(),
                          std::find(objects.begin(), objects.end(), o));
  objects.erase(objects.cbegin() + pos);
}

void scena::render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  myshaders.enable();
  myshaders.set_selected(false);

  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE); // Attiva scrittura z-buffer
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  myshaders.set_projection_transform(camera.projection());
  myshaders.set_camera_transform(camera.camera());
  myshaders.set_camera_position(camera.position());
  myshaders.set_ambient_light(ambient_light);
  myshaders.set_directional_light(directional_light);
  myshaders.set_diffusive_light(diffusive_light);
  myshaders.set_specular_light(specular_light);

  for (sceneObject *o : objects) {
    myshaders.set_model_transform(o->getTransform().T());

    if (selectedObject == nullptr || !selectedObject->isEditMode())
      myshaders.set_selected(o->getSelected());
    o->render();
  }
  if (selectedObject != nullptr && selectedObject->isEditMode()) {

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-2.0f, -2.0f);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    myshaderWire.enable();
    myshaderWire.set_projection_transform(camera.projection());
    myshaderWire.set_camera_transform(camera.camera());
    myshaderWire.set_camera_position(camera.position());

    glEnable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Cambia la modalità di wireframe: 0=vertici, 1=spigoli, 2=facce
    myshaderWire.set_render_mode(mode); // <-- scegli qui dinamicamente se vuoi

    myshaderWire.set_model_transform(selectedObject->getTransform().T());
    // o->render();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    selectedObject->render(); // Nuovo metodo (vedi punto 4)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    selectedObject->render();
    glPointSize(5.0f); // Margine d’errore nel click
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    selectedObject->render();
    glDepthMask(GL_TRUE);
    glDisable(GL_POLYGON_OFFSET_FILL);

    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
  }
  // Ora disegna il testo in 2DF
  std::stringstream ss;

  const char *RiferimentoNames[] = {"globale", "locale", "camera"};
  const char *TrasformazioneNames[] = {"none", "spostamento", "rotazione",
                                       "scaling"};
  const char *AssiNames[] = {"f", "xy", "xz", "yz", "x", "y", "z"};
  const char *SelectNames[] = {"all", "vertex", "edge", "face"};

  glm::vec3 posCam = camera.position();
  glm::vec3 rotCam = camera.rotazione();
  glm::vec3 targetCam = camera.target();
  float distCam = camera.distanza();
  ss << "Camera: " << std::endl
     << "Pos: (" << posCam.x << "," << posCam.y << "," << posCam.z << ")"
     << std::endl
     << "Target: (" << targetCam.x << ", " << targetCam.y << ", " << targetCam.z
     << ")" << std::endl
     << "Yaw:    " << rotCam.x << " deg" << std::endl
     << "Pitch:  " << rotCam.y << " deg" << std::endl
     << "Dist:   " << distCam << std::endl
     << "Fov:   " << fov << std::endl
     << "NearPlane:   " << near_plane << std::endl
     << "FarPlane:   " << far_plane << std::endl
     << std::endl
     << "Riferimento:   " << RiferimentoNames[riferimento] << std::endl
     << "Trasformazione:   " << TrasformazioneNames[t] << std::endl;
  if (t != Trasformazione::none)
    ss << "Asse:   " << AssiNames[asseOperazione] << std::endl << std::endl;

  bool editMode =
      selectedObject == nullptr ? false : selectedObject->isEditMode();
  ss << "Edit:   " << (editMode ? " edit mode" : " object mode") << std::endl;
  if (editMode)
    ss << "SelectMode:   " << SelectNames[mode] << std::endl << std::endl;
  else
    ss << std::endl;

  // Disabilita profondità e testi shader
  if (selectedObject != nullptr) {
    glm::vec3 pos = selectedObject->getTranslation();
    glm::vec3 rot = selectedObject->getRotation();
    glm::vec3 scala = selectedObject->getScale();
    ss << "Selezionato:   " << selectedObject->getName() << std::endl
       << "Pos:   (" << pos.x << ", " << pos.y << ", " << pos.z << ")"
       << std::endl
       << "Rot:   (" << rot.x << ", " << rot.y << ", " << rot.z << ")"
       << std::endl
       << "Sc:   (" << scala.x << ", " << scala.y << ", " << scala.z << ")"
       << std::endl
       << std::endl;
  }
  renderOverlay(20, this->WINDOW_HEIGHT - 20, ss.str());
  glutSwapBuffers();
}
void scena::renderPicking() {
  selection_shader.enableFBO();

  GLuint clearColor[4] = {0, 0, 0, 0};
  glClearBufferuiv(GL_COLOR, 0, clearColor);
  glClear(GL_DEPTH_BUFFER_BIT);

  selection_shader.enable();
  selection_shader.set_projection_transform(camera.projection());
  selection_shader.set_camera_transform(camera.camera());
  selection_shader.set_render_mode(SelectionMode::FACE);
  for (int i = 0; i < objects.size(); i++) {
    sceneObject *obj = objects[i];
    selection_shader.set_model_transform(obj->getTransform().T());
    selection_shader.set_base_object_id(i);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    obj->render();
  }
  if (selectedObject != nullptr && mode != SelectionMode::ALL) {
    selection_shader.set_render_mode(mode);
    selection_shader.set_model_transform(selectedObject->getTransform().T());
    selection_shader.set_base_object_id(
        std::distance(objects.begin(), std::find(objects.begin(), objects.end(),
                                                 selectedObject)));
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0f, -1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    selectedObject->render();
  }
  glDisable(GL_POLYGON_OFFSET_FILL);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void scena::init() {

  glutInitContextVersion(4, 6);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glewExperimental = GL_TRUE;

  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Informatica Grafica");

  glutSetCursor(GLUT_CURSOR_LEFT_ARROW);

  glutWarpPointer(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

  // Must be done after glut is initialized!
  GLenum res = glewInit();
  if (res != GLEW_OK) {
    std::cerr << "Error : " << glewGetErrorString(res) << std::endl;
    exit(1);
  }

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}
void scena::update() {
  auto now = std::chrono::high_resolution_clock::now();
  float deltaTime =
      std::chrono::duration<float, std::milli>(now - lastFrameTime).count();
  lastFrameTime = now;
  for (sceneObject *o : objects)
    o->update(deltaTime / 1000);
  camera.update(/*deltaTime / 1000*/);
}

void scena::toggleAsse() {
  std::cout << "toggle Asse: " << riferimento << std::endl;
  riferimento = (SistemaRiferimento)((riferimento + 1) % 3);
}
void scena::toggleOverlay() { mostraOverlay = !mostraOverlay; }

void scena::toggleSelect() {

  if (mode == SelectionMode::ALL) {
    if (selectedObject == nullptr)
      setSelection(0);
    mode = SelectionMode::VERTEX;
    for (sceneObject *obj : objects)
      obj->setEditMode(true);
  } else {
    mode = SelectionMode::ALL;
    for (sceneObject *obj : objects)
      obj->setEditMode(false);
  }
}

void scena::setSelection(int id) {
  selectedObject = objects[id];
  camera.setTarget(selectedObject->getTranslation());
}

void scena::reset() {
  camera.reset();
  fov = 30.0f;
  near_plane = 0.1f;
  far_plane = 100;
  camera.set_perspective(fov, WINDOW_WIDTH, WINDOW_HEIGHT, near_plane,
                         far_plane);
  for (sceneObject *o : objects) {
    o->reset();
  }

  ambient_light = AmbientLight(glm::vec3(1, 1, 1), 0.2);
  directional_light =
      DirectionalLight(glm::vec3(1, 1, 1), glm::vec3(0, 0, -1)); // 0.5
  diffusive_light = DiffusiveLight(0.5);                         // 0.5
  specular_light = SpecularLight(0.5, 30);
}

void scena::setFov(float fov) {
  if (fov < 5)
    fov = 5;
  if (fov > 120)
    fov = 120;
  camera.set_perspective(fov, WINDOW_WIDTH, WINDOW_HEIGHT, 0.1, 100);
  this->fov = fov;
}
void scena::setFar(float farPlane) {
  if (farPlane < near_plane)
    farPlane = near_plane + 1;
  camera.set_perspective(fov, WINDOW_WIDTH, WINDOW_HEIGHT, near_plane,
                         farPlane);
  this->far_plane = farPlane;
}
void scena::setNear(float nearPlane) {
  if (nearPlane > far_plane)
    nearPlane = far_plane - 1;
  camera.set_perspective(fov, WINDOW_WIDTH, WINDOW_HEIGHT, nearPlane,
                         far_plane);
  this->near_plane = nearPlane;
}

void scena::addFov(float fov) { setFov(this->fov + fov); }
void scena::addNear(float nearPlane) { setNear(this->near_plane + nearPlane); }
void scena::addFar(float farPlane) { setFar(this->far_plane + farPlane); }

float scena::updateSpeedBasedOnFOV() {
  float minFOV = 0.0f;
  float maxFOV = 120.0f;
  float baseSpeed = 0.0f;
  float maxSpeed = 24.0f;
  float speedFactor = (fov - minFOV) / (maxFOV - minFOV);
  // = glm::clamp(speedFactor, 0.0f, 1.0f);
  return baseSpeed + speedFactor * (maxSpeed - baseSpeed);
}

void scena::gestioneOggetti(int xClick, int yClick, int x, int y) {
  float scala = updateSpeedBasedOnFOV();
  if (selectedObject == nullptr)
    return;

  glm::mat4 viewMatrix = camera.camera(); // view matrix
  glm::mat4 invView = glm::inverse(viewMatrix);
  /*glm::vec3 right = glm::normalize(glm::vec3(invView[0]));  // X cam
  glm::vec3 up    = glm::normalize(glm::vec3(invView[1]));  // Y cam
  glm::vec3 forward = -glm::normalize(glm::vec3(invView[2])); // Z cam (verso
  avanti)*/
  glm::vec3 forward = glm::normalize(glm::vec3(invView[2]));
  glm::vec3 right = glm::normalize(
      glm::cross(glm::vec3(0, 1, 0), forward)); // up globale è (0,1,0)
  glm::vec3 up = glm::cross(forward, right);

  glm::mat3 assiLocali = glm::mat3(selectedObject->getAssiIniziale());
  glm::mat3 assiCamera = glm::mat3(glm::mat3(right, up, forward));

  if (t == Trasformazione::spostamento) {
    float sensibilita = 0.005f;
    float deltaX = (x - xClick) * sensibilita * scala;
    float deltaY = (yClick - y) * sensibilita * scala;

    if (riferimento == SistemaRiferimento::LOCALE)
      selectedObject->setTranslation(
          gestioneTransform(glm::vec3(0.0f), glm::vec2(deltaX, deltaY)),
          assiLocali);
    else if (riferimento == SistemaRiferimento::CAMERA)
      selectedObject->setTranslation(
          gestioneTransform(glm::vec3(0.0f), glm::vec2(deltaX, deltaY)),
          assiCamera);
    else
      selectedObject->setTranslation(
          gestioneTransform(glm::vec3(0.0f), glm::vec2(deltaX, deltaY)));
  } else if (t == Trasformazione::rotazione) {

    float sensibilita = 0.025f / 2;

    float deltaX = (x - xClick) * sensibilita * scala;
    float deltaY = (y - yClick) * sensibilita * scala;
    glm::vec3 pos =
        gestioneTransform(glm::vec3(0.0f), glm::vec2(deltaX, deltaY));
    pos = glm::vec3(pos.y, pos.x, pos.z);

    if (riferimento == SistemaRiferimento::LOCALE)
      selectedObject->setRotation(pos, assiLocali);
    else if (riferimento == SistemaRiferimento::CAMERA)
      selectedObject->setRotation(pos, assiCamera);
    else
      selectedObject->setRotation(pos);

  } else if (t == Trasformazione::scaling) {
    float sensitivita = 0.25;
    float delta = ((x - xClick) + (yClick - y)) * sensitivita * 0.01f * scala;
    glm::vec3 newScale = glm::vec3(1.0f);

    if (riferimento == SistemaRiferimento::LOCALE)
      selectedObject->setScale(
          gestioneTransform(glm::vec3(1.0f), glm::vec2(delta)), assiLocali);
    else if (riferimento == SistemaRiferimento::CAMERA)
      selectedObject->setScale(
          gestioneTransform(glm::vec3(1.0f), glm::vec2(delta)), assiCamera);
    else
      selectedObject->setScale(
          gestioneTransform(glm::vec3(1.0f), glm::vec2(delta)));
  }
  glutPostRedisplay();
}

void scena::gestioneCamera(int xClick, int yClick, int x, int y,
                           TrasformazioneCamera trasformazione) {
  float scala = updateSpeedBasedOnFOV();
  float sensitivita = 0.0625;
  float deltaX = (x - xClick) * sensitivita * scala,
        deltay = (y - yClick) * sensitivita * scala;
  if (trasformazione == TrasformazioneCamera::PAN)
    camera.pan(deltaX, deltay);
  else if (trasformazione == TrasformazioneCamera::ZOOM)
    camera.dolly(deltaX);
  else
    camera.orbit(deltaX * 0.3f, deltay * 0.3f);
  glutPostRedisplay();
}

void scena::gestioneSelezione(int x, int y, bool resetSelezione) {

  renderPicking();
  // GLuint id = ReadPixelID(x, y);
  selection_shader.bindTexture();
  glm::uvec4 pixelData;
  int height = glutGet(GLUT_WINDOW_HEIGHT);
  int width = glutGet(GLUT_WINDOW_WIDTH);
  glReadPixels(x, height - 1 - y, // Pixel coordinates (bottom-left origin)
               1, 1,              // Width/height (1x1 pixel)
               GL_RGBA_INTEGER,   // Critical for integer textures
               GL_UNSIGNED_INT,   // Matches texture format
               &pixelData         // Output storage
  );
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  unsigned int idObject = pixelData.r;
  unsigned int idVertex = pixelData.g;
  unsigned int idEdge = pixelData.b;
  unsigned int idFace = pixelData.a;
  std::cout << "Coordinate cliccate: (" << x << ", " << y << ")" << std::endl;
  std::cout << "ID letto: (" << idObject << " , " << idVertex << " , " << idEdge
            << " , " << idFace << ")" << std::endl;

  if (selectedObject != nullptr) {
    selectedObject->setSelected(false);
  }
  if (idObject <= objects.size() && idObject > 0) {
    setSelection(idObject - 1);
    if (resetSelezione) {
      selectedObject->clearSelection();
    }
    selectedObject->gestioneSelezione(pixelData, mode);
  } else if (mode == SelectionMode::ALL)
    selectedObject = nullptr;

  if (selectedObject != nullptr)
    selectedObject->setSelected(true);

  if (selectedObject != nullptr) {
    std::cout << "Oggetto selezionato: " << typeid(*selectedObject).name()
              << std::endl;
  } else {
    std::cout << "Nessun oggetto selezionato (cliccato su sfondo)" << std::endl;
  }
}

void scena::addSpeedObject(bool verticale, bool add, float speed) {
  if (selectedObject == nullptr)
    return;
  glm::vec3 v = selectedObject->getRotationSpeed();

  std::cout << speed << std::endl;
  float delta = speed;
  if (!add)
    delta *= -1;
  if (verticale)
    v.x += delta;
  else
    v.y += delta;
  selectedObject->setRotationSpeed(v);
}

void scena::salvaStato() {
  if (selectedObject == nullptr)
    return;
  selectedObject->saveState();
}

void scena::addIntensityLight(Light tipo, float valore) {
  switch ((int)tipo) {
  case AMBIENT:
    ambient_light.inc(valore);
    break;
  case SPECULARSHINE:
    specular_light.inc_shine(valore);
    break;
  case DIFFUSIVE:
    diffusive_light.inc(valore);
    break;
  case SPECULAR:
    specular_light.inc(valore);
    break;
  }
}
void scena::subIntensityLight(Light tipo, float valore) {
  switch ((int)tipo) {
  case AMBIENT:
    ambient_light.dec(valore);
    break;
  case SPECULARSHINE:
    specular_light.dec_shine(valore);
    break;
  case DIFFUSIVE:
    diffusive_light.dec(valore);
    break;
  case SPECULAR:
    specular_light.dec(valore);
    break;
  }
}

void scena::removeSelected() {
  if (selectedObject == nullptr)
    return;
  removeObject(selectedObject);
  selectedObject = nullptr;
}

glm::vec3 scena::gestioneTransform(glm::vec3 iniziale, glm::vec2 delta) {
  glm::vec3 newPos = iniziale;
  switch (asseOperazione) {
  case assi::x:
    newPos.x += delta.x;
    break;
  case assi::y:
    newPos.y += delta.y;
    break;
  case assi::z:
    newPos.z += delta.x;
    break;
  case assi::xy:
    newPos.x += delta.x;
    newPos.y += delta.y;
    break;
  case assi::xz:
    newPos.x += delta.x;
    newPos.z += delta.y;
    break;
  case assi::yz:
    newPos.z += delta.x;
    newPos.y += delta.y;
    break;
  default:
    newPos += glm::vec3(delta.x, delta.y, 0);
    break;
  }
  return newPos;
}