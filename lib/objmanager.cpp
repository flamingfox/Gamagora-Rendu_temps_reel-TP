#include "objmanager.h"

Mesh ObjManager::loadFromOBJ(const Vector3D &center, const char* obj){

    Mesh retour;

    std::vector<Vector3D> normalsPoints;
    std::vector<int> normalIds;

    FILE* f = fopen(obj, "r");
    while (!feof(f)) {
        char line[255];
        for(int i=0; i<255; i++) line[i]=0;

        fgets(line, 255, f);
        if (line[0]=='v' && line[1]==' ') {
            Vector3D vec;
            sscanf(line, "v %f %f %f\n", &vec[0], &vec[2], &vec[1]);
            //vec[2] = vec[2];
            Vector3D p = vec + center;
            retour.addVertex(p);
        }
        if (line[0]=='v' && line[1]=='n') {
            Vector3D vec;
            sscanf(line, "vn %f %f %f\n", &vec[0], &vec[2], &vec[1]);
            normalsPoints.push_back(vec.normalized());
        }
        if (line[0]=='f') {
            int i0, i1, i2;
            int j0,j1,j2;
            int k0,k1,k2;

            int n = 0;
            //sscanf(line, "f %u/%u/%u %u/%u/%u %u/%u/%u\n", &i0, &j0, &k0, &i1, &j1, &k1, &i2, &j2, &k2 );

            //count of '/' occuration in the line
            for (int i=0; i < sizeof(line); i++){
                if(line[i]=='/')
                    n++;
            }

            if(n==0){
                sscanf(line, "f %u %u %u\n", &i0, &i1, &i2);

                retour.addFace(i0-1, i1-1, i2-1);

            }
            else if(n==3){
                sscanf(line, "f %u/%u %u/%u %u/%u\n", &i0, &k0, &i1, &k1, &i2, &k2 );

                retour.addFace(i0-1, i1-1, i2-1);

            }
            else if(n==6){
                if(sscanf(line, "f %u/%u/%u %u/%u/%u %u/%u/%u\n", &i0, &j0, &k0, &i1, &j1, &k1, &i2, &j2, &k2 ) < 0);
                    sscanf(line, "f %u//%u %u//%u %u//%u\n", &i0, &k0, &i1, &k1, &i2, &k2 );

                retour.addFace(i0-1, i1-1, i2-1);
                normalIds.push_back(k0-1);
                normalIds.push_back(k1-1);
                normalIds.push_back(k2-1);
            }
        }
    }


    if(normalsPoints.empty() || normalIds.empty()){

        for (int i = 0; i < retour.getface().size(); i+=3)
            {
                int ia = retour.getface()[i];
                int ib = retour.getface()[i+1];
                int ic = retour.getface()[i+2];

                Vector3D normal = (retour.getvertex()[ib] - retour.getvertex()[ia]).crossProduct(
                            (retour.getvertex()[ic] - retour.getvertex()[ia])).normalized();

                retour.addNormal(normal); retour.addNormal(normal); retour.addNormal(normal);
                retour.addNormalId(i); retour.addNormalId(i+1); retour.addNormalId(i+2);
            }
    }
    else{
        retour.setNormals(normalsPoints);
        retour.setNormalIds(normalIds);
    }

    fclose(f);

    return retour;
}

void ObjManager::writeToObj(const std::string name, const std::vector<Vector3D> &vertex, const std::vector<unsigned int> &face)
{
    std::ofstream obj;
    obj.open(name.c_str(), std::ios::out);

    obj << "#vertices:\n";

    for(int i = 0 ; i < vertex.size(); i++) {
        obj << "v " << std::setprecision(4) << vertex[i].x << " " << vertex[i].y << " " << vertex[i].z << "\n";
    }

    obj << "#faces:\n";

    for(int i = 0 ; i < face.size(); i += 3){
        obj << "f " << face[i] + 1 << " " << face[i+1] + 1 << " "<< face[i+2] + 1 << "\n";

    }

    obj << "\n";

    obj.close();

    std::cout << "wrote " << name << std::endl;
}
