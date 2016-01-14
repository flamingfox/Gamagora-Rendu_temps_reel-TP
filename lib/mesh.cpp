#include "mesh.h"


/* -------------------------------------------- */
/* -------------------------------------------- */
/* -------------------------------------------- */
/*            Fonctions utilitaires             */
/* -------------------------------------------- */
/* -------------------------------------------- */
/* -------------------------------------------- */

void Mesh::merge(const Mesh &delta)
{
	if(&delta != this){
		int taille = vertex.size();
		vertex.reserve(taille+delta.nbvertex());

		for(size_t i=0; i< delta.vertex.size(); i++){
			vertex.push_back(delta.vertex[i]);
		}

		face.reserve(this->nbface()+delta.nbface());
		for(size_t i=0; i< delta.face.size(); i++){
			face.push_back(delta.face[i] + taille );
		}
	}
}

/* -------------------------------------------- */
/* -------------------------------------------- */
/* -------------------------------------------- */
/*            Fonctions accessors               */
/* -------------------------------------------- */
/* -------------------------------------------- */
/* -------------------------------------------- */

std::vector<Vector3D> Mesh::getvertex() const{
	return vertex;
}

std::vector<int> Mesh::getface() const{
    return face;
}

void Mesh::setVertex(std::vector<Vector3D> vertex)
{
    this->vertex.clear();

    for(Vector3D vec : vertex){
        this->vertex.push_back(vec);
    }
}

void Mesh::setFace(std::vector<int> faces)
{
    this->face.clear();

    for(int face : faces){
        this->face.push_back(face);
    }
}

void Mesh::addVertex(const Vector3D& _vertex)
{
    vertex.push_back(_vertex);
}

void Mesh::addFace(const unsigned int& refPointFace1, const unsigned int& refPointFace2, const unsigned int& refPointFace3)
{
    face.push_back(refPointFace1);
    face.push_back(refPointFace2);
    face.push_back(refPointFace3);
}

size_t Mesh::nbvertex() const
{
	return this->vertex.size();
}

size_t Mesh::nbface() const
{
	return this->face.size();
}


void Mesh::translation(const float x, const float y, const float z)
{
	for(Vector3D& p: vertex){
		p.x+=x;
		p.y+=y;
		p.z+=z;
	}
}

void Mesh::rescale(float scale)
{
	for(Vector3D& p: vertex){
		p *= scale;
	}
}

void Mesh::rotation(const Vector3D T){
    rotation(T.x, T.y, T.z);
}

void Mesh::rotation(const float rX, const float rY, const float rZ){

    for(size_t i=0; i<vertex.size(); i++){
        if(rX != 0)
            vertex[i].rotateAboutAxis(rX, Vector3D(1,0,0) );
		if(rY != 0)
            vertex[i].rotateAboutAxis(rY, Vector3D(0,1,0) );
		if(rZ != 0)
            vertex[i].rotateAboutAxis(rZ, Vector3D(0,0,1) );
    }
}
