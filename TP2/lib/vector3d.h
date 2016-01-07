#ifndef VECTOR3D_H
#define VECTOR3D_H
#include <cmath>
#include <iostream>

#define XYZ(p)  p.x, p.y, p.z

class Vector3D
{
public:

    float x,y,z;

    Vector3D(): Vector3D(0,0,0)  {}
    Vector3D(float a, float b): Vector3D(a,b,0.0)    {}
    Vector3D(float a, float b, float c):    x(a),   y(b),  z(c)  {}
    Vector3D(const Vector3D& v): x(v.x), y(v.y), z(v.z)  {}
    Vector3D(Vector3D&& v): x(v.x), y(v.y), z(v.z)  {}

    void operator=(const Vector3D& v) {     x=v.x;  y=v.y;  z=v.z;    }
    void operator=(Vector3D&& v) {          x=v.x;  y=v.y;  z=v.z;    }

    void set(float a, float b, float c);

    float dotProduct(const Vector3D& v) const;

    //c'est l'orthogonal
    Vector3D crossProduct(const Vector3D& v) const;

    Vector3D negate() const;

    float getNorm() const;
    float getNorm2() const;

    void normalize();
    Vector3D normalized() const;

    // Functions to access Vec3 class components
    float& operator[] (int i);
    float operator[] (int i) const;

    // Unary operators
    Vector3D operator+() const;
    Vector3D operator-() const;

    // Boolean functions
    int operator==(const Vector3D&) const;
    int operator!=(const Vector3D&) const;
    int operator<(const Vector3D&) const;
    int operator>(const Vector3D&) const;
    int operator<=(const Vector3D&) const;
    int operator>=(const Vector3D&) const;

    // Assignment operators
    void operator+=(float a);
    void operator-=(float a);
    void operator*=(float a);
    void operator/=(float a);

    void operator+=(const Vector3D& v);
    void operator-=(const Vector3D& v);
    void operator*=(const Vector3D& v);
    void operator/=(const Vector3D& v);


    // Binary operators
    friend Vector3D operator+ (const Vector3D&, const Vector3D&);
    friend Vector3D operator+ (const Vector3D&, float);
    friend Vector3D operator+ (float, const Vector3D&);
    friend Vector3D operator- (const Vector3D&, const Vector3D&);
    friend Vector3D operator- (const Vector3D&, float);
    friend Vector3D operator- (float, const Vector3D&);

	friend Vector3D operator^ (const Vector3D&, const Vector3D&);
    friend Vector3D operator* (const Vector3D&, const Vector3D&);
    friend Vector3D operator* (const Vector3D&, float);
    friend Vector3D operator* (float, const Vector3D&);
    friend Vector3D operator/ (const Vector3D&, const Vector3D&);
    friend Vector3D operator/ (const Vector3D&, float);
    friend Vector3D operator/ (float, const Vector3D&);



    /**
     * @brief rotateAboutX
     * @param angle en radiants
     * @return
     */
    Vector3D& rotateAboutX(float angle);

    /**
     * @brief rotateAboutY
     * @param angle en radiants
     * @return
     */
    Vector3D& rotateAboutY(float angle);

    /**
     * @brief rotateAboutZ
     * @param angle en radiants
     * @return
     */
    Vector3D& rotateAboutZ(float angle);
    Vector3D& rotateAboutAxis(float angle, const Vector3D& axis);


    // Norm
    friend float Norm(const Vector3D&);
    friend float Norm2(const Vector3D&);
    friend Vector3D Normalized(const Vector3D&);

    friend float distance(const Vector3D& u, const Vector3D& v);
    friend float distance2(const Vector3D& u, const Vector3D& v);

    friend Vector3D Random3D();

    friend std::ostream& operator<<(std::ostream& out, const Vector3D& v);
};

/****************************************************************************************/
/****************************************************************************************/
/****************************************************************************************/

// Binary operators
inline Vector3D operator+ (const Vector3D& u, const Vector3D& v){
    return Vector3D(u.x+v.x, u.y+v.y, u.z+v.z);
}
inline Vector3D operator+ (const Vector3D& u, float a){
    return Vector3D(u.x+a, u.y+a, u.z+a);
}
inline Vector3D operator+ (float a, const Vector3D& v){
    return v+a;
}

inline Vector3D operator- (const Vector3D& u, const Vector3D& v){
    return Vector3D(u.x-v.x, u.y-v.y, u.z-v.z);
}
inline Vector3D operator- (const Vector3D& u,float a){
    return Vector3D(u.x-a, u.y-a, u.z-a);
}
inline Vector3D operator- (float a, const Vector3D& v){
    return Vector3D(a-v.x, a-v.y, a-v.z);
}

inline Vector3D operator^ (const Vector3D& u, const Vector3D& v){
    return Vector3D(u.y*v.z - u.z*v.y , -(u.x*v.z - u.z*v.x) , u.x*v.y - u.y*v.x);
}
inline Vector3D operator* (const Vector3D& u, const Vector3D& v){
    return Vector3D(u.x*v.x, u.y*v.y, u.z*v.z);
}
inline Vector3D operator* (const Vector3D& u,float a){
    return Vector3D(u.x*a, u.y*a, u.z*a);
}
inline Vector3D operator* (float a, const Vector3D& v){
    return v*a;
}

inline Vector3D operator/ (const Vector3D& u, const Vector3D& v){
    #ifndef QT_NO_DEBUG
        if(v.x==0.0 || v.y==0.0 || v.z==0.0)
            std::cerr << "erreur: division d'un Vector3D avec 0" << std::endl;
    #endif
    return Vector3D(u.x/v.x, u.y/v.y);
}
inline Vector3D operator/ (const Vector3D& u, float a){
    #ifndef QT_NO_DEBUG
        if(a == 0)
            std::cerr << "erreur: division d'un Vector3D avec 0" << std::endl;
    #endif
    return Vector3D(u.x/a, u.y/a, u.z/a);
}
inline Vector3D operator/ (float a, const Vector3D& v){
    #ifndef QT_NO_DEBUG
        if(v.x==0.0 || v.y==0.0 || v.z==0.0)
            std::cerr << "erreur: division d'un Vector3D avec 0" << std::endl;
    #endif
    return Vector3D(a/v.x, a/v.y, a/v.z);
}

/****************************************************************************************/

/*!
\brief Compute the Euclidean norm of a Vector3D.
*/
inline float Norm(const Vector3D& u){
    return sqrt(Norm2(u));
}
/*!
\brief Compute the Euclidean square norm of a Vector3D.
*/
inline float Norm2(const Vector3D& u){
    return u.x*u.x + u.y*u.y + u.z*u.z;
}

/*!
\brief Compute the normalized Vector3D.
*/
inline Vector3D Normalized(const Vector3D& u)
{
    float len = Norm(u);
    #ifdef QT_NO_DEBUG
        if(len == 0)        {
            std::cerr << "erreur: normalisation d'un Vector3D de longueur 0" << std::endl;
        }
    #endif
    return u/len;
}


/****************************************************************************************/

/*!
\brief Return a new Vector3D with coordinates set to the minimum coordinates
of the two argument vs.
*/
inline Vector3D min(const Vector3D& a,const Vector3D& b){
    return Vector3D(a[0]<b[0] ? a[0]:b[0] , a[1]<b[1] ? a[1]:b[1] , a[2]<b[2] ? a[2]:b[2]);
}

/*!
\brief Return a new Vector3D with coordinates set to the maximum coordinates
of the two argument vs.
*/
inline Vector3D max(const Vector3D& a,const Vector3D& b){
    return Vector3D(a[0]>b[0] ? a[0]:b[0] , a[1]>b[1] ? a[1]:b[1] , a[2]>b[2] ? a[2]:b[2]);
}

/****************************************************************************************/

inline float dot(const Vector3D& u, const Vector3D& v){
    return u.x*v.x + u.y*v.y + u.z*v.z;
}
/*!
\brief Returns a new Vector3D orthogonal to the argument Vector3D.
*/
inline Vector3D Orthogonal(const Vector3D& u, const Vector3D& v){
    return Vector3D(u.y*v.z - u.z*v.y , u.x*v.z - u.z*v.x , u.x*v.y - u.y*v.x);
}

/****************************************************************************************/

inline float distance(const Vector3D& u, const Vector3D& v){
    return (v-u).getNorm();
}
inline float distance2(const Vector3D& u, const Vector3D& v){
    return (v-u).getNorm2();
}

/****************************************************************************************/


//! Generates a random Vector3D with precision 1e-3 within [0,1] interval
inline Vector3D Random3d()
{
    float x=rand()%1001/1000.0;
    float y=rand()%1001/1000.0;
    float z=rand()%1001/1000.0;
    return Vector3D(x,y,z);
}

#endif // VECTOR3D_H
