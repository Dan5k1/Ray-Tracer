// Transform.cpp: implementation of the Transform class.

#include "Transform.h"

// Helper rotation function.  
mat3 Transform::rotate(const float degrees, const vec3& axis) 
{

    glm::vec3 axisNormalized = glm::normalize(axis);
    float x = axisNormalized[0];
    float y = axisNormalized[1];
    float z = axisNormalized[2];
    float cosTheta = cos(degrees * pi / 180.0);
    float sinTheta = sin(degrees * pi / 180.0);
    glm::mat3 maaT(x * x, x * y, x * z, x * y, y * y, y * z, x * z, y * z, z * z);
    glm::mat3 mA(0, z, -y, -z, 0, x, y, -x, 0); // column major order
    glm::mat3 mR = (cosTheta * glm::mat3(1.0f)) + ((1 - cosTheta) * maaT) + (sinTheta * mA);
    return mR;
}

void Transform::left(float degrees, vec3& eye, vec3& up) 
{
 
    eye = rotate(degrees, up) * eye;
}

void Transform::up(float degrees, vec3& eye, vec3& up) 
{
    up = rotate(degrees, glm::cross(eye, up)) * up;
    eye = rotate(degrees, glm::cross(eye, up)) * eye;
}

mat4 Transform::lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) 
{

    glm::vec3 c = glm::normalize(eye - glm::vec3(0, 0, 0)); // c = normalize(eye - center)
    glm::vec3 b = glm::normalize(up);
    glm::vec3 a = glm::cross(b, c); // a = b x c
    glm::mat4 viewMatrix(a[0], b[0], c[0], 0, a[1], b[1], c[1], 0, a[2], b[2], c[2], 0, -glm::dot(a, eye), -glm::dot(b, eye), -glm::dot(c, eye), 1);
    return viewMatrix;
}

mat4 Transform::perspective(float fovy, float aspect, float zNear, float zFar)
{

    float A = -(zFar + zNear) / (zFar - zNear);
    float B = -(2 * zFar * zNear) / (zFar - zNear);
    float theta = (fovy * pi / 180.0) / 2; // in radians
    float d = 1 / tan(theta);
    glm::mat4 mP(d/aspect, 0, 0, 0, 0, d, 0, 0, 0, 0, A, -1, 0, 0, B, 0);
    return mP;
}

mat4 Transform::scale(const float &sx, const float &sy, const float &sz) 
{
    mat4 ret(sx, 0, 0, 0, 0, sy, 0, 0, 0, 0, sz, 0, 0, 0, 0, 1);
    return ret;
}

mat4 Transform::translate(const float &tx, const float &ty, const float &tz) 
{
    mat4 ret(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, tx, ty, tz, 1);
    return ret;
}

// To normalize the up direction and construct a coordinate frame.   

vec3 Transform::upvector(const vec3 &up, const vec3 & zvec) 
{
    vec3 x = glm::cross(up,zvec); 
    vec3 y = glm::cross(zvec,x); 
    vec3 ret = glm::normalize(y); 
    return ret; 
}


Transform::Transform()
{

}

Transform::~Transform()
{

}
