
#include "mesh.h"
#include "../base/assimp/assimp/postprocess.h" // Assimp post processing flags
#include "../base/assimp/assimp/scene.h"       // Assimp output data structure
#include "../base/assimp/assimp/Importer.hpp"
#include <iostream>
#include <iterator>
#include <ostream>
#include <set>
#include <vector>
#include <cstring>


std::ostream &operator<<(std::ostream &os, const Vertex &v) {
    os<<"["<<v.position.x<<", "<<v.position.y<<", "<<v.position.z<<"] ";
    os<<"("<<v.normal.x<<", "<<v.normal.y<<", "<<v.normal.z<<") ";
    os<<"<"<<v.textcoord.x<<", "<<v.textcoord.y<<"> ";

    return os;
}


Mesh::Mesh(): _VAO(-1), _VBO(-1), _IBO(-1), _num_indices(0) {
    verticiIniziali=std::unordered_map<unsigned int, glm::vec3>();
    normaliIniziali=std::unordered_map<unsigned int, glm::vec3>();
    saveState();
}


Mesh::~Mesh() {
}



bool Mesh::load_mesh(const std::string& Filename, unsigned int flags)
{
    
    bool Ret = false;
    Assimp::Importer Importer;

    std::cout << "Loading '" << Filename << "'" << std::endl;
    const aiScene* pScene = Importer.ReadFile(Filename.c_str(), flags |aiProcess_JoinIdenticalVertices);//aiProcess_Triangulate | aiProcess_GenSmoothNormals);// | aiProcess_FlipUVs);

    
    std::string Filepath = get_file_path(Filename);
    if (pScene) {
        Ret = init_from_scene(pScene, Filepath);
    }
    else {
        std::cout<<"Error loading "<<Filename<<" : "<<Importer.GetErrorString()<<std::endl;
    }

    return Ret;
}

bool Mesh::init_from_scene(const aiScene* pScene, const std::string& Filepath) {  

    // Copiamo i dati dal formato Assimp agli array di vertici e indici
    //
    // NOTA: CONSIDERIAMO SOLO *UNA* MESH E LA RELATIVA TEXTURE COLORE 

    bool Ret = true;

    const aiMesh* paiMesh = pScene->mMeshes[0]; // consideriamo solo una mesh (mesh 0)

    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    for (unsigned int i = 0 ; i < paiMesh->mNumVertices ; i++) {
        const aiVector3D* pPos      = &(paiMesh->mVertices[i]);
        const aiVector3D* pNormal   = &(paiMesh->mNormals[i]);
        const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

        Vertex v(glm::vec3(pPos->x, pPos->y, pPos->z),
                 glm::vec3(pNormal->x, pNormal->y, pNormal->z),
                 glm::vec2(pTexCoord->x, pTexCoord->y));

        Vertices.push_back(v);
    }

    for (unsigned int i = 0 ; i < paiMesh->mNumFaces ; i++) {
        const aiFace& Face = paiMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
        vertIndexToFacePositions[Face.mIndices[0]].push_back(i);
        vertIndexToFacePositions[Face.mIndices[1]].push_back(i);
        vertIndexToFacePositions[Face.mIndices[2]].push_back(i);
    }

    //2000->[faccia 2,4,7]


    _num_indices = Indices.size(); 

    // Creiamo e bindiamo gli oggetti OpenGL

    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);

    glGenBuffers(1, &_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), &Vertices[0], GL_DYNAMIC_DRAW);

    glGenBuffers(1, &_IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.size(), &Indices[0], GL_DYNAMIC_DRAW);

    glVertexAttribPointer(ATTRIB_POSITIONS, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(struct Vertex, position));
    glVertexAttribPointer(ATTRIB_NORMALS, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(struct Vertex, normal));
    glVertexAttribPointer(ATTRIB_COLOR_TEXTURE_COORDS, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(struct Vertex, textcoord));
    //glEnableVertexAttribArray(3);
    glVertexAttribPointer(SELECTED, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(struct Vertex, selezionato));

    glBindVertexArray(0);

    // Carichiamo la texture
    // Consideriamo solo un materiale che ha una texture diffusiva
    for (unsigned int i = 1 ; i < pScene->mNumMaterials ; i++) {

        const aiMaterial* pMaterial = pScene->mMaterials[i];
        
        aiString materialName;

        pMaterial->Get(AI_MATKEY_NAME, materialName);

        //std::cout<< i << " " << materialName.data << std::endl;

        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString Path;

            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
                std::string data = Path.data;

                std::string FullPath = Filepath + "/" + data;

                if (!_texture.load(FullPath.c_str())) {
                    std::cout<<"  Error loading texture '"<<FullPath<<"'"<<std::endl;
                }
                else {
                    std::cout<<"  Loaded texture '"<<FullPath<<"'"<<std::endl;
                    break;
                }
            }
        }
    }

    if (!_texture.is_valid()) {
        Ret = _texture.load("white.png");
        std::cout<<"  Loaded blank texture."<<std::endl;
    }
    
    //updateSelectedSSBO();
    saveState();
    return Ret;
}

void Mesh::applyTransformationToVertices(const std::vector<unsigned int>& indices, 
                                         const glm::mat4& transform,
                                         const glm::vec3& origin,
                                         const glm::mat3& localAxes) {
    
    glm::vec3 normal = getCentroidNormal(indices); // centro dei vertici selezionati
    // Helper robusto
    glm::vec3 helper = (fabs(normal.z) < 0.999f) ? glm::vec3(0, 0, 1) : glm::vec3(0, 1, 0);
    // Tangente e bitangente ortonormali
    glm::vec3 tangent   = glm::normalize(glm::cross(helper, normal));
    glm::vec3 bitangent = glm::normalize(glm::cross(normal, tangent));
    // Costruzione della matrice 4x4 (local frame)
    glm::mat4 localFrame(1.0f);
    localFrame[0] = glm::vec4(tangent,   0.0f);  // X
    localFrame[1] = glm::vec4(bitangent, 0.0f);  // Y
    localFrame[2] = glm::vec4(normal,    0.0f);  // Z
    localFrame[3] = glm::vec4(origin,  1.0f);  // Traslazione
    glm::mat4 localFrameInv = glm::inverse(localFrame);

    glm::mat4 T =  localFrame* transform *localFrameInv ; //conversione in spazio locale
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(T)));
    for (unsigned int idx : indices) {
        if (idx >= Vertices.size()) continue;    
        glm::vec4 pos = glm::vec4(verticiIniziali[idx], 1.0f);
        pos = T * pos;
        Vertices[idx].position = glm::vec3(pos);
        // Per la normale
        glm::vec3 norm = glm::normalize(normalMatrix * normaliIniziali[idx]);
        Vertices[idx].normal = norm;
    }
    _modified = true;
}


void Mesh::addTransformationToVertices(const std::vector<unsigned int>& indices, 
                                       const glm::mat4& transform,
                                       const glm::vec3& origin,
                                       const glm::mat3& localAxes) 
{
    glm::vec3 normal = getCentroidNormal(indices); // centro dei vertici selezionati
    // Helper robusto
    glm::vec3 helper = (fabs(normal.z) < 0.999f) ? glm::vec3(0, 0, 1) : glm::vec3(0, 1, 0);
    // Tangente e bitangente ortonormali
    glm::vec3 tangent   = glm::normalize(glm::cross(helper, normal));
    glm::vec3 bitangent = glm::normalize(glm::cross(normal, tangent));
    // Costruzione della matrice 4x4 (local frame)
    glm::mat4 localFrame(1.0f);
    localFrame[0] = glm::vec4(tangent,   0.0f);  // X
    localFrame[1] = glm::vec4(bitangent, 0.0f);  // Y
    localFrame[2] = glm::vec4(normal,    0.0f);  // Z
    localFrame[3] = glm::vec4(origin,  1.0f);  // Traslazione
    
    glm::mat4 localFrameInv = glm::inverse(localFrame);
    glm::mat4 T = localFrame  * transform * localFrameInv ;
    
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));
    
    for (unsigned int idx : indices) {
        if (idx >= Vertices.size()) continue;
        glm::vec4 pos = glm::vec4(Vertices[idx].position, 1.0f);
        pos = transform * pos;
        Vertices[idx].position = glm::vec3(pos);

        // Per la normale
        glm::vec3 norm = glm::normalize(normalMatrix * Vertices[idx].normal);
        Vertices[idx].normal = norm;
    }
    _modified = true;
}

glm::vec3 Mesh::getCentroid(const std::vector<unsigned int>& indices){
    glm::vec3 centroid(0.0f);
    for (unsigned int idx : indices) {
        if (idx >= Vertices.size()) continue;
        centroid += verticiIniziali[idx];
    }
    centroid /= static_cast<float>(indices.size());
    return centroid;
}
glm::vec3 Mesh::getCentroidNormal(const std::vector<unsigned int>& indices){
    glm::vec3 centroid(0.0f);
    for (unsigned int idx : indices) {
        if (idx >= Vertices.size()) continue;
        centroid += normaliIniziali[idx];
    }
    centroid = glm::normalize(centroid);
    return centroid;
}

std::string Mesh::get_file_path(const std::string &Filename) const {
    std::string::size_type SlashIndex = Filename.find_last_of("/");
    std::string Dir;

    if (SlashIndex == std::string::npos) {
        Dir = ".";
    }
    else if (SlashIndex == 0) {
        Dir = "/";
    }
    else {
        Dir = Filename.substr(0, SlashIndex);
    }  
    
    return Dir;  
}

std::vector<Vertex> Mesh::getVertices()
{
    return Vertices;
}

std::vector<unsigned int> Mesh::getIndices()
{
    return Indices;
}
void Mesh::saveState(){
    for (unsigned int idx=0;idx < Vertices.size();idx++)
    {
        verticiIniziali[idx] = Vertices[idx].position;
        normaliIniziali[idx] = Vertices[idx].normal;
    }   

}
unsigned int  Mesh::verifica_faccia(unsigned int i1, unsigned int i2, unsigned int i3) const{
    //estraggo i 3 array di facce in cui compaiono i vertici
    //intersezzione tra set
    const auto& f1 = vertIndexToFacePositions.at(i1);
    const auto& f2 = vertIndexToFacePositions.at(i2);
    const auto& f3 = vertIndexToFacePositions.at(i3);

    std::set<size_t> set1(f1.begin(), f1.end());
    std::set<size_t> set2(f2.begin(), f2.end());
    std::set<size_t> set3(f3.begin(), f3.end());

    for (auto faceID : set1) {
        if (set2.count(faceID) > 0 && set3.count(faceID) > 0) {
            return faceID;
        }
    }
    return false;
}
unsigned int  Mesh::verifica_edge(unsigned int i1, unsigned int i2) const {
    const auto& f1 = vertIndexToFacePositions.at(i1);
    const auto& f2 = vertIndexToFacePositions.at(i2);

    std::set<size_t> set1(f1.begin(), f1.end());
    std::set<size_t> set2(f2.begin(), f2.end());

    for (auto faceID : set1) {
        if (set2.count(faceID) > 0) {
            // i1 e i2 condividono almeno una faccia => potenzialmente sono connessi da un edge
            return true;
        }
    }
    return false;
}

std::array<unsigned int, 3> Mesh::getFaceVertexIndices(unsigned int faceID) const{
    /*
        indices creano triangoli prendendo triplette di conseguenza avendo id della faccia 
        faccia*3 è il primo vertice e poi +1 e +2 sono gli altri 2
    */

    if (faceID * 3 + 2 >= Indices.size()) {
        throw std::out_of_range("Face ID out of range");
    }
    return {
        Indices[faceID * 3],
        Indices[faceID * 3 + 1],
        Indices[faceID * 3 + 2]
    };
}


void Mesh::setSelected(unsigned int id, bool valore) {
    if (id >= Vertices.size()) return;
    Vertices[id].selezionato = valore;
    _modified = true;
    std::cout << "modificato " << id << " in : " << Vertices[id].selezionato << " " << _modified << std::endl;
}

void Mesh::clearSelected(const std::vector<unsigned int>& indices) {
    for (unsigned int i : indices)
        setSelected(i, false);
}

void Mesh::toggleSelected(unsigned int id) {
    if (id >= Vertices.size()) return;
    setSelected(id, !Vertices[id].selezionato);
}




// Nel render bind SSBO (in realtà si può bindare una volta per frame, ma così siamo sicuri)
void Mesh::render() {
    glBindVertexArray(_VAO);

    _texture.bind(TEXTURE_COLOR);

    glEnableVertexAttribArray(ATTRIB_POSITIONS);
    glEnableVertexAttribArray(ATTRIB_NORMALS);
    glEnableVertexAttribArray(ATTRIB_COLOR_TEXTURE_COORDS);
    glEnableVertexAttribArray(SELECTED);

    if (_modified) {
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), nullptr, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * Vertices.size(), Vertices.data());
        _modified = false;
    }

    glDrawElements(GL_TRIANGLES, _num_indices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}