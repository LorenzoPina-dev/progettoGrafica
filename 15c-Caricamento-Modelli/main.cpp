/**
  Questo codice mostra come sfruttare la libreria esterna assimp per impofrtare
  dei modelli 3D e renderizzarli nella scena. Al momento, il codice è in grado
  di renderizzare modelli costituiti da una singola mesh e con una singola
  Texture associata.

  I modelli 3D sono memorizzato nella cartella models (nella root di questo
  codice). Il formato dei file è Wavefront
  (https://en.wikipedia.org/wiki/Wavefront_.obj_file), un semplice file di testo
  con la lista delle coordinate dei vertici, facce e normali dei triangoli, e
  la/le texture associata/e.

  E' stata creata una classe Mesh che gestisce il caricamento e il rendering dei
  modelli. Gli oggetti in scena sono incapsulati in classi che permettono
  l'inizializzazione del modello e il suo rendering.

  Per comodità, sono state istanziate delle variabili globali che
  contengono diversi modelli 3D:

  Modelli singoli:
  'teapot': Una teiera (visualizzabile premendo 't')
  'skull' : Una teschio (visualizzabile premendo 'k')
  'dragon': Una drago (visualizzabile premendo 'g')
  'boot'  : Uno scarpone (visualizzabile premendo 'b')
  'flower': Un fiore (visualizzabile premendo 'f')

  Modello composto
  'marius': Un volto (visualizzabile premendo 'm').
  Questo modello è composto da diverse mesh (6) ciascuna definita in un suo
  file separato. Una volta caricate le singole mesh, queste sono renderizzate
  di seguito all'interno dello stesso ciclo di rendering per avere il volto
  completo. Inoltre, per questo modello, alcune texture hanno delle trasparenze
  Per poterle usare in modo corretto è necessario impostare in modo appropriato
  OpenGL. Vedere la funzione marius.render().
*/

#include <ostream>
#define GLM_FORCE_RADIANS

#include "../base/glew/include/GL/glew.h"
#include "../base/freeglut/include/GL/freeglut.h" // prima di freeglut
#include "../base/glm/glm/glm.hpp"
#include "../base/glm/glm/gtc/matrix_transform.hpp"
#include <chrono>
#include <iostream>
#include <sstream>

#include "camera.h"
#include "transform.h"

#include "myshaderclass.h"

#include "../base/glm/glm/gtc/type_ptr.hpp" // Per glm::value_ptr
#include "myShaderSelect.h"
#include "myShaderWire.h"
#include "oggetti/boot.h"
#include "oggetti/dragon.h"
#include "oggetti/flower.h"
#include "oggetti/marius.h"
#include "oggetti/sceneObject.h"
#include "oggetti/skull.h"
#include "oggetti/teapot.h"
#include "scena.h"
#include "util/GestoreMouse.h"
#include "util/GestoreTastiera.h"
#include <algorithm>
#include <cstdint>
unsigned char MODEL_TO_RENDER = 'k';

/**
  Struttura di comodo dove sono memorizzate tutte le variabili globali
*/


scena s;
GestoreMouse gm;
GestoreTastiera gt;

/**
Prototipi della nostre funzioni di callback.
Sono definite più avanti nel codice.
*/

void MyClose(void);

void init(int argc, char *argv[]) {
  glutInit(&argc, argv);
  s.init();

  //inizializzazione funzioni glut
  glutDisplayFunc([] { s.render(); });
  glutCloseFunc([]{
    std::cout << "Tearing down the system..." << std::endl;
    exit(0);
  });
  glutIdleFunc([] {
    s.update();
    glutPostRedisplay();
  });

  //setup mouse
  gm = GestoreMouse();
  //gestione onUp
  gm.setOnRightUp([](int x, int y) { glutSetCursor(GLUT_CURSOR_LEFT_ARROW); });
  gm.setOnWheelUp([](int x, int y) { s.set(Trasformazione::none); });
  gm.setOnLeftUp([](int x, int y) { glutSetCursor(GLUT_CURSOR_LEFT_ARROW); });

  //gestione onDown
  gm.setOnRightDown([](int x, int y) { glutSetCursor(GLUT_CURSOR_CYCLE); });
  gm.setOnWheelDown([](int x, int y) { s.salvaStato(); });
  gm.setOnLeftDown([](int x, int y){
      s.gestioneSelezione(x, y, !gt.isCtrlPressed());
      glutPostRedisplay(); // chiama la selezione
    });

  //gestione drag
  gm.setMouseRigthDrag([](int xClick, int yClick, int x, int y) {
    TrasformazioneCamera t = TrasformazioneCamera::ROTAZIONE;
    if (gt.isShiftPressed())
      t = TrasformazioneCamera::PAN;
    else if (gt.isCtrlPressed())
      t = TrasformazioneCamera::ZOOM;
    s.gestioneCamera(xClick, yClick, x, y, t);
    gm.resettaPosIniziale(x, y);
  });
  gm.setMouseWheelDrag([](int xClick, int yClick, int x, int y) {
    s.gestioneOggetti(xClick, yClick, x, y);
  });

  gm.inizializza();

  //inizializzazione di tutte le gesture
  gt = GestoreTastiera();
  
  gt.setKeyEvent(' ', [](int x, int y) { s.reset(); });
  gt.setKeyEvent(27, [](int x, int y) { glutDestroyWindow(glutGetWindow()); });

  //gestione trasformazioni
  gt.setKeyEvent('r', [](int x, int y) { s.set(Trasformazione::rotazione); });
  gt.setKeyEvent('g', [](int x, int y) { s.set(Trasformazione::spostamento); });
  gt.setKeyEvent('s', [](int x, int y) { s.set(Trasformazione::scaling); });

  //gestione lock assi
  gt.setKeyEvent('r', [](int x, int y) { s.set(assi::f); });
  gt.setKeyEvent('g', [](int x, int y) { s.set(assi::f); });
  gt.setKeyEvent('s', [](int x, int y) { s.set(assi::f); });
  gt.setKeyEvent('x', [](int x, int y) { s.set(assi::x); });
  gt.setKeyEvent('y', [](int x, int y) { s.set(assi::y); });
  gt.setKeyEvent('z', [](int x, int y) { s.set(assi::z); });
  gt.setKeyEvent('X', [](int x, int y) { s.set(assi::yz); });
  gt.setKeyEvent('Y', [](int x, int y) { s.set(assi::xz); });
  gt.setKeyEvent('Z', [](int x, int y) { s.set(assi::xy); });
  
  //gestione modalità selezione (vertex,edge e face)
  gt.setKeyEvent(9, [](int x, int y) { s.toggleSelect(); });     // tab
  gt.setKeyEvent('1', [](int x, int y) { s.set(SelectionMode::VERTEX); });
  gt.setKeyEvent('2', [](int x, int y) { s.set(SelectionMode::EDGE); });
  gt.setKeyEvent('3', [](int x, int y) { s.set(SelectionMode::FACE); });

  //gestione luci
  gt.setKeyEvent('1', [](int x, int y) { s.subIntensityLight(Light::AMBIENT, 0.01); });
  gt.setKeyEvent('2', [](int x, int y) { s.addIntensityLight(Light::AMBIENT, 0.01); });
  gt.setKeyEvent('3', [](int x, int y) { s.subIntensityLight(Light::DIFFUSIVE, 0.01); });
  gt.setKeyEvent('4', [](int x, int y) { s.addIntensityLight(Light::DIFFUSIVE, 0.01); });
  gt.setKeyEvent('5', [](int x, int y) { s.subIntensityLight(Light::SPECULAR, 0.01); });
  gt.setKeyEvent('6', [](int x, int y) { s.addIntensityLight(Light::SPECULAR, 0.01); });
  gt.setKeyEvent('7', [](int x, int y) { s.subIntensityLight(Light::SPECULARSHINE, 1); });
  gt.setKeyEvent('8', [](int x, int y) { s.addIntensityLight(Light::SPECULARSHINE, 1); });

  //gestione camera
  gt.setKeyEvent('+', [](int x, int y) { s.addFov(5); });
  gt.setKeyEvent('-', [](int x, int y) { s.addFov(-5); });
  gt.setKeyEvent('$', [](int x, int y) { s.addFar(0.5); });
  gt.setKeyEvent('%', [](int x, int y) { s.addFar(-0.5); });
  gt.setKeyEvent('!', [](int x, int y) { s.addNear(0.5); });
  gt.setKeyEvent('"', [](int x, int y) { s.addNear(-0.5); });

  //gestione oggetti scena
  gt.setKeyEvent('S', [](int x, int y) { s.addObject(new Skull()); });
  gt.setKeyEvent('T', [](int x, int y) { s.addObject(new Teapot()); });
  gt.setKeyEvent('F', [](int x, int y) { s.addObject(new Flower()); });
  gt.setKeyEvent('D', [](int x, int y) { s.addObject(new Dragon()); });
  gt.setKeyEvent('B', [](int x, int y) { s.addObject(new Boot()); });
  gt.setKeyEvent('M', [](int x, int y) { new Marius(s);});
  gt.setKeyEvent(127, [](int x, int y) { s.removeSelected(); }); // canc

  //gestione rotazioni automatiche
  gt.setKeyEvent(GLUT_KEY_UP, [](int x, int y) { s.addSpeedObject(true, false); }, true); //riduzione speed verticale
  gt.setKeyEvent(GLUT_KEY_DOWN, [](int x, int y) { s.addSpeedObject(true, true); }, true); //aumento speed verticale
  gt.setKeyEvent(GLUT_KEY_LEFT, [](int x, int y) { s.addSpeedObject(false, true); },true); //aumento spped orizzontale
  gt.setKeyEvent(GLUT_KEY_RIGHT, [](int x, int y) { s.addSpeedObject(false, false); },true);//riduzione spped orizzontale

  gt.inizializza();

}

void create_scene() {
  s.createScene();
  //mesh di default
  s.addObject(new Teapot());
}


int main(int argc, char *argv[]) {
  init(argc, argv);
  create_scene();
  glutMainLoop();
  return 0;
}