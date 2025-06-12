#include "alfaObject.h"
#include "../common.h"
 
AlfaObject::AlfaObject(const std::string& nome) : sceneObject(nome) {
    traslazioneInit=glm::vec3(0);
    rotazioneInit=glm::vec3(0);
    scaleInit=glm::vec3(1);
}

//override per applicare le trasformazioni
void AlfaObject::init(void) {
    m.load_mesh(nomeFile,flags);

    //trasformazioni per centrare l'oggetto
    transform.translate(traslazioneInit);
    transform.scale(scaleInit);
    transform.rotate(rotazioneInit);

    initialized = true;
}

//render con alfa blend
void AlfaObject::render(){
    if(!initialized) init();

    glEnable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    m.render();
    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
}

void AlfaObject::setTraslazioneInit(glm::vec3 traslazione){
    this->traslazioneInit=traslazione;
}

void AlfaObject::setRotazioneInit(glm::vec3 rotazione){
    this->rotazioneInit=rotazione;
}

void AlfaObject::setScaleInit(glm::vec3 scale){
    this->scaleInit=scale;
}

void AlfaObject::setModel(std::string nomeFile, unsigned int flags){
    this->nomeFile=nomeFile;
    this->flags=flags;
}