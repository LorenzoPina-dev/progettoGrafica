#ifndef MyShaderClassSelection_H
#define MyShaderClassSelection_H
#define GLM_FORCE_RADIANS
#include "shaderclass.h"
#include "light.h"
#include <iostream>
#include <cstdint>

/**
    Classe che include le funzionalità specifiche legate agli shader da usare
    nell'applicazione. 
    Sono stati overloadati i metodi load_shaders e load_done.
    Sono stati inseriti due metodi pubblici per settare la matrice di trasformazione
    delle coordinate dei vertici. 
*/
class MyShaderClassSelection : public ShaderClass {
public:

    /**
        Setta la matrice di trasformazione nel vertex shader

        @param transform matrice 4x4 di trasformazione  
    */
    void set_model_transform(const glm::mat4 &transform);

    /**
        Setta la matrice di trasformazione di camera completa

        @param transform matrice 4x4 di trasformazione  
    */
    void set_camera_transform(const glm::mat4 &transform);


    void set_projection_transform(const glm::mat4 &projection);
    void set_base_object_id(unsigned int  id);  
    void set_render_mode(int mode);

    void enableFBO();

    void bindTexture();

private:

    /**
        Metodo per il caricamento degli shader

        @return true se il caricamento è andato a buon fine
    */
    virtual bool load_shaders();

    /**
        Metodo che recupera tutte le location delle variabili uniform 
        negli shader caricati.  

        @return se le operazioni post-load sono andate a buon fine
    */
    virtual bool load_done();
    GLuint FBO;
    GLuint TextureSel;
    GLuint depthBuffer;

    GLint _model_transform_location; ///<< Location della variabile Model2World
    
    GLint _camera_transform_location; ///<< Location della variabile World2Camera
    GLint _base_object_id_location;
    GLint _projection_transform_location;
    GLint _render_mode_location;
};
#endif