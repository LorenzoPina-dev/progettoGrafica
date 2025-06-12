#pragma once
#define GLM_FORCE_RADIANS
#include <string>
#include "../transform.h"
#include "../../base/glm/glm/glm.hpp"
#include "../myshaderclass.h"
#include "../mesh.h"
#include <unordered_set>

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1,T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2;
    }
};

class sceneObject {
protected:
    std::string name;
    bool initialized;
    bool editMode;
    SelectionMode currentMode;
    LocalTransform transform;
    Mesh m;

    glm::vec3 translationSpeed;
    glm::vec3 velocitaAngolare;
    glm::vec3 scaleSpeed;
    glm::vec3 posizioneInizialeOggetto;
    glm::quat rotazioneInizialeOggetto;
    glm::vec3 scaleInizialeOggetto;

    bool selected;


public:
    sceneObject(const std::string& name);
    sceneObject(const std::string& name, Mesh* m);

    virtual void init(){
        initialized=true;
    };
    virtual void render(){
        if (!initialized) init();
            m.render();
    };
    
    void update(float deltaTime);
    void reset();

    
    virtual ~sceneObject() = default;

    void saveState();

    // metodi per toggle della selezioni di ogni struttura
    void toggleVertexSelection(unsigned int index);
    void toggleEdgeSelection(unsigned int index1, unsigned int index2);
    void toggleFaceSelection(unsigned int index1, unsigned int index2, unsigned int index3);


    // metodi per set della selezioni di ogni struttura
    void setVertexSelection(unsigned int index, bool valore);
    void setEdgeSelection(unsigned int index1, unsigned int index2, bool valore);
    void setFaceSelection(unsigned int index1, unsigned int index2, unsigned int index3, bool valore);


    //deseleziona tutti i vertici
    void clearSelection();

    //setter per le mod
    // SelectionMode (vertex,edge,face)
    void setSelectionMode(SelectionMode mode);
    // EditMode true o false
    void setEditMode(bool enable);

    //get edit mode
    bool isEditMode() const;


    //metodi per trasformazioni: axes sono i 3 assi su coi viene fatta la trasformazione

    //traslazioni
    void setTranslation(const glm::vec3& pos,const glm::mat3 &axes= glm::mat3(1.0f));
    void addTranslation(const glm::vec3& pos,const glm::mat3 &axes= glm::mat3(1.0f));

    //rotazioni
    void setRotation(const glm::vec3& angles,const glm::mat3 &axes= glm::mat3(1.0f));
    void addRotation(const glm::vec3& angles,const glm::mat3 &axes= glm::mat3(1.0f));
    void addRotation(float angleX, float angleY, float angleZ,const glm::mat3 &axes= glm::mat3(1.0f));

    //scale
    void setScale(const glm::vec3& scale,const glm::mat3 &axes= glm::mat3(1.0f));
    void addScale(const glm::vec3& scale,const glm::mat3 &axes= glm::mat3(1.0f));

    LocalTransform& getTransform();
    glm::vec3 getRotation() const;

    //gestione trasformazioni automatiche:

    //rotazione
    void setRotationSpeed(const glm::vec3 &v);
    void addRotationSpeed(const glm::vec3 &v);
    void redRotationSpeed(const glm::vec3 &v);
    glm::vec3 getRotationSpeed() const;

    //traslazione
    void setTranslationSpeed(const glm::vec3 &speed);
    void addTranslationSpeed(const glm::vec3 &speed);
    void redTranslationSpeed(const glm::vec3 &speed);
    glm::vec3 getTranslation() const;

    //scale
    void setScaleSpeed(const glm::vec3& scale);
    glm::vec3 getScale() const;

    //metodi di get e set per sapere se l'oggetto è selezionato
    bool getSelected() const;
    void setSelected(bool selected);


    Mesh* getMesh();

    const std::string& getName() const;

    std::string to_string();


    //medodo che gestisce la logica dato il uvec4 dalla texture capisce cosa si è cliccato e lo seleziona
    void gestioneSelezione(glm::uvec4 id, SelectionMode mode);

    friend std::ostream& operator<<(std::ostream& os, const sceneObject& obj);

    glm::mat3 getAssiIniziale();

private:
    //mappe che mantengono il nuymero di riferimenti alla data struttura(vertici,lati e facce)
    std::unordered_map<unsigned int, unsigned int> vertexSelectionCount;
    std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int, pair_hash> edgeSelectionCount;
    std::unordered_map<unsigned int, unsigned int> faceSelectionCount;
    
    //metodi intermi che permettono di aggiornare le mappe sopra a seguito di una nuova selezione
    void updateVertexSelectionCount(unsigned int index, bool add);
    void updateEdgeSelectionCount(const std::pair<unsigned int, unsigned int>& edge, bool add);
    void updateFaceSelectionCount(unsigned int faceId, bool add);

    std::vector<unsigned int> getCurrentlySelectedVertices() const;
};
