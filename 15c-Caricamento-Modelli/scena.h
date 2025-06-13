#pragma once
#define GLM_FORCE_RADIANS
#include "../base/glm/glm/gtc/type_ptr.hpp" // Per glm::value_ptr
#include "camera.h"
#include "myShaderSelect.h"
#include "myShaderWire.h"
#include "myshaderclass.h"
#include "oggetti/sceneObject.h"
#include "util/GestoreMouse.h"
#include "util/GestoreTastiera.h"
#include <chrono>
#include <iostream>
#include <sstream>
#include <vector>

class scena {
private:
  int WINDOW_WIDTH = 1024; // Larghezza della finestra
  int WINDOW_HEIGHT = 768; // Altezza della finestra

  Camera camera;

  AmbientLight ambient_light;
  DirectionalLight directional_light;
  DiffusiveLight diffusive_light;
  SpecularLight specular_light;
  
  MyShaderClass myshaders;
  MyShaderWire myshaderWire;
  MyShaderClassSelection selection_shader;

  std::vector<sceneObject *> objects;
  std::chrono::high_resolution_clock::time_point lastFrameTime;

  SelectionMode mode = SelectionMode::ALL;
  SistemaRiferimento riferimento = GLOBALE;
  assi asseOperazione;

  float fov = 30.0f;
  float near_plane=0.1f;
  float far_plane=100;

  bool mostraOverlay;

  Trasformazione t;
  sceneObject *selectedObject = nullptr;

  float updateSpeedBasedOnFOV();
  glm::vec3 gestioneTransform(glm::vec3 iniziale, glm::vec2 delta);
public:
  //funzioni di gestione generica della scena
  void init();
  void createScene();  
  void update();
  void reset();
  void salvaStato();

  //gestione modalità selezione, assi sistemi di riferimento e trasformazioni
  void set(assi asse);
  void set(SelectionMode selectMode);
  void set(SistemaRiferimento sistemaRiferimento);
  void set(Trasformazione trasformazione);
  void toggleOverlay();
  void toggleAsse();
  void toggleSelect();
  
  //gestione oggetti in scena
  void addObject(sceneObject *o);
  void removeObject(sceneObject *o);
  void removeObject(int o);

  //gestione selezione
  void gestioneSelezione(int x, int y, bool resetSelezione);
  void setSelection(int id);
  void removeSelected();

  //render
  void renderOverlay(int x, int y, std::string messaggio);
  void render();
  void renderPicking();

  //settaggi camera
  void setFov(float fov);
  void setNear(float nearPlane);
  void setFar(float farPlane);
  void addFov(float fov);
  void addNear(float nearPlane);
  void addFar(float farPlane);

  //gestori che muovono oggetti o camera in base al mouse
  void gestioneOggetti(int xClick, int yClick, int x, int y);
  void gestioneCamera(int xClick, int yClick, int x, int y,
                      TrasformazioneCamera trasformazione);
  void addSpeedObject(bool verticale, bool add, float speed = 45);

  //gestione luci
  void addIntensityLight(Light tipo, float valore);
  void subIntensityLight(Light tipo, float valore);
};
