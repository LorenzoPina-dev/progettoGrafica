#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "../base/glm/glm/glm.hpp"


/**
	Funzione che trasforma un angolo da gradi in radianti
	@param deg angolo in gradi
	@return l'angolo in radianti
*/
float to_radiant(float deg);

/**
	Funzione che trasforma un angolo da radianti in gradi
	@param deg angolo in radianti
	@return l'angolo in gradi
*/
float to_degree(float rad);

/**
	Classe che incapsula diverse funzioni per calcolare le matrici di 
	trasformazione geometrica e comporre una unica matrice di 
	trasformazione.
	le chiamate alle funzioni rotate, translate e scale, applicano la trasformazione
	richiesta alla matrice composita corrente: composita = T * composita.
	Chiamate successive alle funzioni concatenano le trasformazioni. 

	le funzioni statiche rotation, rotation3, translation, scaling permettono di 
	calcolare e ottenere in output le matrici di trasformazioni richieste. Possono
	essere usate senza istanziare la classe ma come: LocalTransform::rotation(...)
*/
#pragma once
#include "../base/glm/glm/glm.hpp"
#include "../base/glm/glm/gtc/quaternion.hpp"

class LocalTransform {
public:
    LocalTransform();

    // ROTATION
    void rotate(float degX, float degY, float degZ);
    void rotate(const glm::vec3& angle);
    void rotate(const glm::quat& angle);
    void addRotate(float degX, float degY, float degZ);
    void addRotate(const glm::vec3& angle);
    glm::quat getRotate() const;
    glm::vec3 getRotateVec() const;

    // TRANSLATION
    void translate(float x, float y, float z);
    void translate(const glm::vec3& offset);
    void addTranslate(float x, float y, float z);
    void addTranslate(const glm::vec3& offset);
    glm::vec3 getTranslate() const;

    // SCALING
    void scale(float sx, float sy, float sz);
    void scale(float sc);
    void scale(const glm::vec3& factor);
    void addScale(float sx, float sy, float sz);
    void addScale(float sc);
    void addScale(const glm::vec3& factor);
    glm::vec3 getScale() const;
    glm::quat getQuad(const glm::vec3& rotation);

    const glm::mat4 T(); // Transformation matrix
    void reset();

private:
    void update();
    glm::mat3 rotation3(float deg, const glm::vec3& axis);

    glm::quat _rotation;
    glm::vec3 _translation;
    glm::vec3 _scaling;
    glm::mat4 _combined;
};




#endif