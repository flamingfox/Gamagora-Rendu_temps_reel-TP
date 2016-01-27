#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>

#include <math.h>
#include "float.h"

#include "vector3d.h"

#include <algorithm>

class Mesh{

protected:
    std::vector<Vector3D> vertex;
    std::vector<Vector3D> normalsPoints;
    std::vector<int> face;
    std::vector<int> normalIds;

public :

    Mesh(){}
    Mesh(std::vector<Vector3D> listvertex, std::vector<int> listface): vertex(listvertex), face(listface)  {}

    ~Mesh(){}

    void merge(const Mesh &delta);

    std::vector<Vector3D> getvertex() const;
    std::vector<int> getface() const;
    std::vector<Vector3D> getNormals() const {return normalsPoints;}
    std::vector<int> getNormalIds() const {return normalIds;}
    void setVertex(std::vector<Vector3D> vertex);
    void setNormals(std::vector<Vector3D> normals);
    void setFaces(std::vector<int> faces);
    void setNormalIds(std::vector<int> normalIds) { this->normalIds.clear(); this->normalIds = normalIds;}
    void addVertex(const Vector3D& _vertex);
    void addNormal(const Vector3D& _normal);
    void addNormalId(const int& _normalId);
    void addFace(const unsigned int& refPointFace1, const unsigned int& refPointFace2, const unsigned int& refPointFace3);
    size_t nbvertex() const;
    inline size_t nbnormal() const{return normalsPoints.size();}
    inline size_t nbnormalid() const{return normalIds.size();}
    size_t nbface() const;

	void translation(const float x, const float y, const float z);
	void rescale(float scale);

	void rotation(const Vector3D T);
	void rotation(const float rX, const float rY, const float rZ);
	
protected:

};


#endif // MESH_H
