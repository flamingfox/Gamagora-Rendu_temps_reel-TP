#ifndef OBJMANAGER_H
#define OBJMANAGER_H

#include "vector3d.h"

#include "mesh.h"
#include <string>

#include <iostream>
#include <iomanip>
#include <fstream>

//#include <glm/glm.hpp>

namespace ObjManager
{
    Mesh loadFromOBJ(const Vector3D &center, const char* obj);
	
    void writeToObj(const std::string name, const std::vector<Vector3D>& vertex, const std::vector<unsigned int>& face);
}

#endif // OBJMANAGER_H
