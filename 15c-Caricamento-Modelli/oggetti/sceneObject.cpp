#include "sceneObject.h"
#include <array>
#include <iterator>
#include <math.h>
#include <ostream>
#include <utility>
#include <vector>
#define GLM_FORCE_RADIANS
#include "../../base/glm/glm/gtc/matrix_transform.hpp"
#include "../../base/glm/glm/gtx/string_cast.hpp"
#include <iostream>
#include <algorithm>

sceneObject::sceneObject(const std::string& name)
    : name(name), initialized(false), editMode(false),
      translationSpeed(0.0f), velocitaAngolare(0.0f), scaleSpeed(0.0f), transform(), selected(false) {
    // mappe inizialmente vuote
}

sceneObject::sceneObject(const std::string& name, Mesh* m)
    : sceneObject(name) {
    this->m = *std::move(m);
}

// dato id del vertice e se va aggiunto o tolto aggiorna le tabelle delle occorenze e in caso non compare più viene deselezionato
void sceneObject::updateVertexSelectionCount(unsigned int index, bool add) {
    if (add) {
        vertexSelectionCount[index]++;
        if (vertexSelectionCount[index] == 1) {
            m.setSelected(index, true);
        }
    } else {
        auto it = vertexSelectionCount.find(index);
        if (it != vertexSelectionCount.end()) {
            if (--(it->second) == 0) {
                m.setSelected(index, false);
                vertexSelectionCount.erase(it);
            }
        }
    }
}

//dato un pair<id vertice partenza, id vertice arrivo e se aggiungerlo o no va a gestire la selezione dei 2 vertici aggiornando le occorrenze di edge e vertici
void sceneObject::updateEdgeSelectionCount(const std::pair<unsigned int, unsigned int>& edge, bool add) {
    if (add) {
        edgeSelectionCount[edge]++;
        if (edgeSelectionCount[edge] == 1) {
            updateVertexSelectionCount(edge.first,true);
            updateVertexSelectionCount(edge.second,true);
        }
    } else {
        auto it = edgeSelectionCount.find(edge);
        if (it != edgeSelectionCount.end()) {
            if (--(it->second) == 0) {
                updateVertexSelectionCount(edge.first,false);
                updateVertexSelectionCount(edge.second,false);
                edgeSelectionCount.erase(it);
            }
        }
    }
}


//dato un faceId e se aggiungerlo o no va a gestire la selezione dei 3 edge aggiornando le occorrenze di faccia e edge
void sceneObject::updateFaceSelectionCount(unsigned int faceId, bool add) {
    if (add) {
        faceSelectionCount[faceId]++;
        if (faceSelectionCount[faceId] == 1) {
            std::array<unsigned int, 3> face=m.getFaceVertexIndices(faceId);
            updateVertexSelectionCount(face[0],true);
            updateVertexSelectionCount(face[1],true);
            updateVertexSelectionCount(face[2],true);
        }
    } else {
        auto it = faceSelectionCount.find(faceId);
        if (it != faceSelectionCount.end()) {
            if (--(it->second) == 0) {
                std::array<unsigned int, 3> face=m.getFaceVertexIndices(faceId);
                updateVertexSelectionCount(face[0],false);
                updateVertexSelectionCount(face[1],false);
                updateVertexSelectionCount(face[2],false);
                faceSelectionCount.erase(it);
            }
        }
    }
}

// metodi per eseguire un toggle sulla selezione di tutte le possibili strutture (vertex, edge e face)
void sceneObject::toggleVertexSelection(unsigned int index) {
    bool isSelected = vertexSelectionCount.count(index) > 0;
    updateVertexSelectionCount(index, !isSelected);
}
void sceneObject::toggleEdgeSelection(unsigned int index1, unsigned int index2) {
    auto edge = std::make_pair(index1, index2);
    std::cout<<"coppia: "<<index1<<" , "<<index2<<std::endl;
    bool isSelected = edgeSelectionCount.count(edge) > 0;
    updateEdgeSelectionCount(edge, !isSelected);
}

void sceneObject::toggleFaceSelection(unsigned int index1, unsigned int index2, unsigned int index3) {
    unsigned int faceId = m.verifica_faccia(index1, index2, index3);
    std::cout<<"faccia premuta: "<<faceId<<std::endl;
    bool isSelected = faceSelectionCount.count(faceId) > 0;
    updateFaceSelectionCount(faceId, !isSelected);
}

// metodi per modificare lo stato selezione di tutte le possibili strutture (vertex, edge e face)
void sceneObject::setVertexSelection(unsigned int index, bool valore) {
    bool isSelected = vertexSelectionCount.count(index) > 0;
    if (valore != isSelected) {
        updateVertexSelectionCount(index, valore);
    }
}

void sceneObject::setEdgeSelection(unsigned int index1, unsigned int index2, bool valore) {
    auto edge = std::make_pair(index1, index2);
    std::cout<<"coppia: "<<index1<<" , "<<index2<<std::endl;
    bool isSelected = edgeSelectionCount.count(edge) > 0;
    if (valore != isSelected) {
        updateEdgeSelectionCount(edge, valore);
    }
}

void sceneObject::setFaceSelection(unsigned int index1, unsigned int index2, unsigned int index3, bool valore) {
    unsigned int faceId = m.verifica_faccia(index1, index2, index3);
    bool isSelected = faceSelectionCount.count(faceId) > 0;
    if (valore != isSelected) {
        updateFaceSelectionCount(faceId, valore);
    }
}

// Funzione che ritorna la lista attuale dei vertici selezionati (chiave della mappa)
std::vector<unsigned int> sceneObject::getCurrentlySelectedVertices() const {
    std::vector<unsigned int> result;
    result.reserve(vertexSelectionCount.size());
    for (const auto& v: vertexSelectionCount) {
        if (v.second > 0) result.push_back(v.first);
    }
    return result;
}

//metodo per deselezionare tutti i vertici e pulire le mappe contenendo le occorrenze 
void sceneObject::clearSelection() {
    for (auto& v : faceSelectionCount) {
        std::array<unsigned int, 3> idVertexs=m.getFaceVertexIndices(v.first);
        for(unsigned int id:idVertexs)
            m.setSelected(id, false);
    }
    for (auto& v : edgeSelectionCount) {
        m.setSelected(v.first.first, false);
        m.setSelected(v.first.second, false);
    }
    for (auto& v : vertexSelectionCount) {
        m.setSelected(v.first, false);
    }
    vertexSelectionCount.clear();
    edgeSelectionCount.clear();
    faceSelectionCount.clear();
}


//funzione per effettuare traslazioni automatiche, dato il deltaTime permette di aggiornare rotazione, traslazione e scale automatico
void sceneObject::update(float deltaTime) {
    if (editMode && !vertexSelectionCount.empty()) {

        //setup per rotazioni automatiche anche in edit mode ma scelta di non inserirlo in quanto troppo confusionario

        /*LocalTransform t;

        if (glm::length2(translationSpeed) > 0.0f)
            t.addTranslate(translationSpeed * deltaTime);

        if (glm::length2(velocitaAngolare) > 0.0f)
            t.addRotate(velocitaAngolare * deltaTime);

        if (glm::length2(scaleSpeed) > 0.0f)
            t.addScale(scaleSpeed * deltaTime);

        if (glm::length2(t.getTranslate()) > 0.0f || glm::length2(t.getRotate()) > 0.0f || glm::length2(t.getScale()) > 0.0f) {
            glm::mat4 T_obj = transform.T();
            glm::mat4 T_obj_inv = glm::inverse(T_obj);
            glm::mat4 T_local = t.T();

            // Trasformazione totale nello spazio locale dell'oggetto
            glm::mat4 finalTransform = T_obj * T_local * T_obj_inv;

            //m.addTransformationToVertices(getCurrentlySelectedVertices(), finalTransform);
            //m.saveState();
        }*/

    } else {
        if (glm::length2(translationSpeed) > 0.0f)
            transform.addTranslate(translationSpeed * deltaTime);
        if (glm::length2(velocitaAngolare) > 0.0f)
            transform.addRotate(velocitaAngolare * deltaTime);
        if (glm::length2(scaleSpeed) > 0.0f)
            transform.addScale(scaleSpeed * deltaTime);
    }
}



void sceneObject::setSelectionMode(SelectionMode mode){
    this->currentMode=mode;
}


void sceneObject::setEditMode(bool enable) {
    editMode = enable;
}


bool sceneObject::isEditMode() const {
    return editMode;
}

//gestione di traslazione
void sceneObject::setTranslation(const glm::vec3& pos,const glm::mat3 &axes) {
    glm::vec3 translationSistema=axes*pos;
    if (editMode && !vertexSelectionCount.empty()) {
        
        //glm::vec3 diff =  transform.getTranslate()-pos ;
        LocalTransform t;
        t.translate(translationSistema);
        std::vector<unsigned int> selezionati=getCurrentlySelectedVertices();
        glm::vec3 centroid=m.getCentroid(selezionati);
        m.applyTransformationToVertices(selezionati, t.T(),centroid);
    } else {
        transform.translate(posizioneInizialeOggetto+translationSistema);
    }
}

void sceneObject::addTranslation(const glm::vec3& pos,const glm::mat3 &axes) {
    glm::vec3 translationSistema=axes*pos*glm::inverse(axes);
    if (editMode && !vertexSelectionCount.empty()) {
        LocalTransform t;
        t.addTranslate(translationSistema);
        std::cout<<"trasformazione locale"<<std::endl;
        std::vector<unsigned int> selezionati=getCurrentlySelectedVertices();
        glm::vec3 centroid=m.getCentroid(selezionati);
        m.addTransformationToVertices(selezionati, t.T(),centroid);
    } else {
        transform.addTranslate(translationSistema);
    }
}

glm::vec3 sceneObject::getTranslation() const
{
    return transform.getTranslate();
}


//gestione rotazione

void sceneObject::setRotation(const glm::vec3& angles,const glm::mat3 &axes) {
    glm::vec3 rotationSistema=axes*angles*glm::inverse(axes);
    if (editMode && !vertexSelectionCount.empty()) {
        LocalTransform t;
        t.rotate(rotationSistema);
        
        std::vector<unsigned int> selezionati=getCurrentlySelectedVertices();
        glm::vec3 centroid=m.getCentroid(selezionati);
        m.applyTransformationToVertices(selezionati, t.T(),centroid);
    } else {
        
        transform.rotate(transform.getQuad(rotationSistema)*rotazioneInizialeOggetto);
    }
}

void sceneObject::addRotation(const glm::vec3& angles,const glm::mat3 &axes) {
    glm::vec3 rotationSistema=axes*angles*glm::inverse(axes);
    if (editMode && !vertexSelectionCount.empty()) {
        LocalTransform t;
        t.addRotate(rotationSistema);
        
        std::vector<unsigned int> selezionati=getCurrentlySelectedVertices();
        glm::vec3 centroid=m.getCentroid(selezionati);
        m.addTransformationToVertices(selezionati, t.T(),centroid);
    } else {
        transform.addRotate(rotationSistema);
    }
}

void sceneObject::addRotation(float angleX, float angleY, float angleZ,const glm::mat3 &axes) {
    addRotation(glm::vec3(angleX, angleY, angleZ),axes);
}

glm::vec3 sceneObject::getRotation() const
{
    return transform.getRotateVec();
}

//gestione scale
void sceneObject::setScale(const glm::vec3& scale,const glm::mat3 &axes) {
    glm::vec3 scaleSistema=axes*scale*glm::inverse(axes);
    if (editMode && !vertexSelectionCount.empty()) {
        LocalTransform t;
        t.scale(scaleSistema);

        std::vector<unsigned int> selezionati=getCurrentlySelectedVertices();
        glm::vec3 centroid=m.getCentroid(selezionati);
        m.applyTransformationToVertices(selezionati, t.T(),centroid);
    } else {
        transform.scale(scaleInizialeOggetto + glm::vec3(1)+ scaleSistema);/*+axes*s);*/
    }
}

void sceneObject::addScale(const glm::vec3& scale,const glm::mat3 &axes) {
    glm::vec3 scaleSistema=axes*scale*glm::inverse(axes);
    if (editMode && !vertexSelectionCount.empty()) {
        LocalTransform t;
        t.addScale(scaleSistema);
        
        std::vector<unsigned int> selezionati=getCurrentlySelectedVertices();
        glm::vec3 centroid=m.getCentroid(selezionati);
        m.addTransformationToVertices(selezionati, t.T(),centroid);
    } else {
        transform.addScale( scaleSistema);
    }
}

glm::vec3 sceneObject::getScale() const
{
    return transform.getScale();
}




//gestione trasformazioni automatiche

void sceneObject::setRotationSpeed(const glm::vec3 &v)
{
    this->velocitaAngolare = v;
}

void sceneObject::addRotationSpeed(const glm::vec3 &v)
{
    this->velocitaAngolare += v;
}

void sceneObject::redRotationSpeed(const glm::vec3 &v)
{
    this->velocitaAngolare -= v;
}

glm::vec3 sceneObject::getRotationSpeed() const
{
    return velocitaAngolare;
}

void sceneObject::setTranslationSpeed(const glm::vec3 &speed)
{
    translationSpeed = speed;
    //std::cout<<"spped"<<translationSpeed.x<<" "<<translationSpeed.y<<std::endl;
}

void sceneObject::addTranslationSpeed(const glm::vec3 &speed)
{
    translationSpeed += speed;
    //std::cout<<"spped"<<translationSpeed.x<<" "<<translationSpeed.y<<std::endl;
}
void sceneObject::redTranslationSpeed(const glm::vec3 &speed)
{
    translationSpeed -= speed;
    //std::cout<<"spped"<<translationSpeed.x<<" "<<translationSpeed.y<<std::endl;
}

void sceneObject::setScaleSpeed(const glm::vec3& scale){
    scaleSpeed=scale;
}



LocalTransform& sceneObject::getTransform() {
    return transform;
}


bool sceneObject::getSelected() const
{
    return selected;
}

void sceneObject::setSelected(bool selected)
{
    this->selected= selected;
}

Mesh* sceneObject::getMesh(){
    return &m;
}

const std::string& sceneObject::getName() const {
    return name;
}
void sceneObject::reset(){
    transform.reset();
    setScaleSpeed(glm::vec3(0,0,0));
    setTranslationSpeed(glm::vec3(0,0,0));
    setRotationSpeed(glm::vec3(0,0,0));
    selected=false;
    std::cout<<to_string()<<std::endl;
}

std::string sceneObject::to_string(){
    std::string out="";
    out+="SceneObject: " + getName() + "\n";
    out+= "  Position: " + glm::to_string(getTranslation()) + "\n";
    out+= "  Rotation: " + glm::to_string(getRotation()) + "\n";
    out+= "  Scale: "    + glm::to_string(getScale()) + "\n";
    out+= "  Rotation Speed: " + glm::to_string(getRotationSpeed()) + "\n";
    return out;
}
void sceneObject::saveState(){
    posizioneInizialeOggetto=transform.getTranslate();
    scaleInizialeOggetto=transform.getScale();
    rotazioneInizialeOggetto=transform.getRotate();
    m.saveState();
}

glm::mat3 sceneObject::getAssiIniziale(){
    LocalTransform temp;
    temp.rotate(rotazioneInizialeOggetto);
    glm::mat3 rotMat = glm::mat3(temp.T());
    rotMat[0] = glm::normalize(rotMat[0]);
    rotMat[1] = glm::normalize(rotMat[1]);
    rotMat[2] = glm::normalize(rotMat[2]);
    return rotMat;
}

std::ostream& operator<<(std::ostream& os, const sceneObject& obj) {
    os << "SceneObject: " << obj.getName() << "\n";
    os << "  Position: " << glm::to_string(obj.getTranslation()) << "\n";
    os << "  Rotation: " << glm::to_string(obj.getRotation()) << "\n";
    os << "  Scale: "    << glm::to_string(obj.getScale()) << "\n";
    os << "  Rotation Speed: " << glm::to_string(obj.getRotationSpeed()) << "\n";
    return os;
}

void sceneObject::gestioneSelezione(glm::uvec4 id, SelectionMode mode){

    unsigned int  idObject= id.r;
    unsigned int   idVertex=id.g;
    unsigned int   idEdge=id.b;
    unsigned int   idFace=id.a;
    int maxSize=getMesh()->getVertices().size();

    if(idFace<=maxSize&&idFace>0 && mode==SelectionMode::FACE)
        toggleFaceSelection(idVertex-1,idEdge-1,idFace-1);
    else if(idEdge<=maxSize&&idEdge>0 && mode==SelectionMode::EDGE)
        toggleEdgeSelection(idVertex-1, idEdge-1);
    else if(idVertex<=maxSize&&idVertex>0 && mode==SelectionMode::VERTEX) 
        toggleVertexSelection(idVertex-1);
}
