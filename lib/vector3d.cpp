#include "vector3d.h"

void Vector3D::set(float a, float b, float c){
    x=a;    y=b;    z=c;
}

/****************************************************************************************/

float Vector3D::dotProduct(const Vector3D& v) const{
    return dot(*this, v);
}

Vector3D Vector3D::crossProduct(const Vector3D& v) const{
    return Orthogonal(*this, v);
}

Vector3D Vector3D::negate() const{
    return -*this;
}

/****************************************************************************************/

float Vector3D::getNorm() const{
    return Norm(*this);
}

float Vector3D::getNorm2() const
{
    return Norm2(*this);
}

/****************************************************************************************/

void Vector3D::normalize(){
    float len=getNorm();
    #ifndef QT_NO_DEBUG
        if(len == 0)
            std::cerr << "erreur: normalized d'un Vector3D à 0" << std::endl;
    #endif
    *this/=len;
}

Vector3D Vector3D::normalized() const{
    float len=getNorm();
    #ifndef QT_NO_DEBUG
        if(len == 0)
            std::cerr << "erreur: normalized d'un Vector3D à 0" << std::endl;
    #endif
    return *this/len;
}

/****************************************************************************************/

// Functions to access Vector3D class components
float& Vector3D::operator[] (int i) {
    if(i == 0)      return x;
    else if(i == 2) return y;
    else            return z;
}
float Vector3D::operator[] (int i) const {
    if(i == 0)      return x;
    else if(i == 2) return y;
    else            return z;
}

/****************************************************************************************/


 Vector3D Vector3D::operator+() const{
    return *this;
}
 Vector3D Vector3D::operator-() const{
    return Vector3D(-x,-y,-z);
}

/****************************************************************************************/

void Vector3D::operator+=(float a){
    x+=a;    y+=a;    z+=a;
}
void Vector3D::operator-=(float a){
    x-=a;    y-=a;    z-=a;
}
void Vector3D::operator*=(float a){
    x*=a;    y*=a;    z*=a;
}
void Vector3D::operator/=(float a){
    #ifndef QT_NO_DEBUG
        if(a == 0)
            std::cerr << "erreur: division d'un Vector3D avec 0" << std::endl;
    #endif
    x/=a;        y/=a;        z/=a;
}

void Vector3D::operator+=(const Vector3D& v){
    x+=v.x;    y+=v.y;    z+=v.z;
}
void Vector3D::operator-=(const Vector3D& v){
    x-=v.x;    y-=v.y;    z-=v.z;
}
void Vector3D::operator*=(const Vector3D& v){
    x*=v.x;    y*=v.y;    z*=v.z;
}
void Vector3D::operator/=(const Vector3D& v){
    #ifndef QT_NO_DEBUG
        if(v.x==0.0 || v.y==0.0 || v.z==0.0)
            std::cerr << "erreur: division d'un Vector3D avec 0" << std::endl;
    #endif
    x/=v.x;        y/=v.y;        z/=v.z;
}

/****************************************************************************************/

int Vector3D::operator==(const Vector3D& v) const{
    return(x==v.x && y==v.y && z==v.z);
}
int Vector3D::operator!=(const Vector3D& v) const{
    return(!(*this==v));
}

int Vector3D::operator<(const Vector3D& v) const{
    return ((x<v.x) && (y<v.y) && (z<v.z));
}
int Vector3D::operator>(const Vector3D& v) const{
    return ((x>v.x) && (y>v.y) && (z>v.z));
}

int Vector3D::operator<=(const Vector3D& v) const{
    return ((x<=v.x) && (y<=v.y) && (z<=v.z));
}
int Vector3D::operator>=(const Vector3D& v) const{
    return ((x>=v.x) && (y>=v.y) && (z>=v.z));
}

/****************************************************************************************/


Vector3D& Vector3D::rotateAboutX(float angle){
    float s = sinf(angle);
    float c = cosf(angle);

    float ny = c * y - s * z;
    float nz = c * z + s * y;

    y = ny;
    z = nz;

    return (*this);
}

Vector3D& Vector3D::rotateAboutY(float angle){
    float s = sinf(angle);
    float c = cosf(angle);

    float nx = c * x + s * z;
    float nz = c * z - s * x;

    x = nx;
    z = nz;

    return (*this);
}

Vector3D& Vector3D::rotateAboutZ(float angle){
    float s = sinf(angle);
    float c = cosf(angle);

    float nx = c * x - s * y;
    float ny = c * y + s * x;

    x = nx;
    y = ny;

    return (*this);
}

Vector3D& Vector3D::rotateAboutAxis(float angle, const Vector3D& axis){
    float s = sinf(angle);
    float c = cosf(angle);
    float k = 1.0F - c;

    float nx = x * (c + k * axis.x * axis.x) + y * (k * axis.x * axis.y - s * axis.z)
            + z * (k * axis.x * axis.z + s * axis.y);
    float ny = x * (k * axis.x * axis.y + s * axis.z) + y * (c + k * axis.y * axis.y)
            + z * (k * axis.y * axis.z - s * axis.x);
    float nz = x * (k * axis.x * axis.z - s * axis.y) + y * (k * axis.y * axis.z + s * axis.x)
            + z * (c + k * axis.z * axis.z);

    x = nx;
    y = ny;
    z = nz;

    return (*this);
}

/****************************************************************************************/

std::ostream& operator<<(std::ostream& out,const Vector3D& v){
    return out << "{" << v.x << " " << v.y << " " << v.z << "}";
}
