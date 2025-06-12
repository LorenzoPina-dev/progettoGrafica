#include <ostream>
#define GLM_FORCE_RADIANS
#include <iostream>
#include "transform.h"
#include "../base/glm/glm/gtc/quaternion.hpp"
#include "../base/glm/glm/gtx/string_cast.hpp"

namespace {
    const float g2r = M_PI / 180.0f;
    const float r2g = 180.0f / M_PI;
}

float to_radiant(float deg) {
    return deg * g2r;
}

float to_degree(float rad) {
    return rad * r2g;
}

LocalTransform::LocalTransform() {
    reset();
}

void LocalTransform::update() {
    _combined = T();
}

// ROTATION
void LocalTransform::rotate(float degX, float degY, float degZ) {
    rotate(glm::vec3(degX, degY, degZ));
}

void LocalTransform::rotate(const glm::vec3& angle) {
    glm::quat q = glm::quat_cast(glm::mat3_cast(getQuad(angle)));
    _rotation = q;  // setto direttamente il quaternion
    update();
}
void LocalTransform::rotate(const glm::quat& angle) {
    glm::quat q = glm::quat_cast(glm::mat3_cast(angle));
    _rotation = q;  // setto direttamente il quaternion
    update();
}

void LocalTransform::addRotate(float degX, float degY, float degZ) {
    addRotate(glm::vec3(degX, degY, degZ));
}

void LocalTransform::addRotate(const glm::vec3& angle) {
	 glm::quat delta = glm::quat_cast( glm::mat3_cast(getQuad(angle)));
    _rotation = delta * _rotation;  // moltiplicazione quaternion
    update();
}

glm::quat LocalTransform::getRotate() const {
    return _rotation;
}
glm::vec3 LocalTransform::getRotateVec() const {
    return glm::eulerAngles(_rotation);
}

// TRANSLATION
void LocalTransform::translate(float x, float y, float z) {
    translate(glm::vec3(x, y, z));
}

void LocalTransform::translate(const glm::vec3& offset) {
    _translation = offset;
    update();
}

void LocalTransform::addTranslate(float x, float y, float z) {
    addTranslate(glm::vec3(x, y, z));
}

void LocalTransform::addTranslate(const glm::vec3& offset) {
    _translation += offset;
    update();
}

glm::vec3 LocalTransform::getTranslate() const {
    return _translation;
}

// SCALING
void LocalTransform::scale(float sx, float sy, float sz) {
    scale(glm::vec3(sx, sy, sz));
}

void LocalTransform::scale(float sc) {
    scale(glm::vec3(sc));
}

void LocalTransform::scale(const glm::vec3& factor) {
    _scaling = glm::max(factor, glm::vec3(0.1f));
    update();
}

void LocalTransform::addScale(float sx, float sy, float sz) {
    addScale(glm::vec3(sx, sy, sz));
}

void LocalTransform::addScale(float sc) {
    addScale(glm::vec3(sc));
}

void LocalTransform::addScale(const glm::vec3& factor) {
    _scaling = glm::max(_scaling + factor, glm::vec3(0.0f));
    update();
}

glm::vec3 LocalTransform::getScale() const {
    return _scaling;
}

// TRANSFORMATION MATRIX
const glm::mat4 LocalTransform::T() {
    glm::mat3 R = glm::mat3_cast(_rotation);
    glm::mat4 T(1.0f);
    T[0][0] = R[0][0] * _scaling.x; T[0][1] = R[0][1] * _scaling.y; T[0][2] = R[0][2] * _scaling.z;
    T[1][0] = R[1][0] * _scaling.x; T[1][1] = R[1][1] * _scaling.y; T[1][2] = R[1][2] * _scaling.z;
    T[2][0] = R[2][0] * _scaling.x; T[2][1] = R[2][1] * _scaling.y; T[2][2] = R[2][2] * _scaling.z;
    T[3][0] = _translation.x;
    T[3][1] = _translation.y;
    T[3][2] = _translation.z;
    return T;
}

// UTILITIES
glm::quat LocalTransform::getQuad(const glm::vec3& rot) {
    glm::quat x = glm::angleAxis(to_radiant(rot.x), glm::vec3(1, 0, 0));
    glm::quat y = glm::angleAxis(to_radiant(rot.y), glm::vec3(0, 1, 0));
    glm::quat z = glm::angleAxis(to_radiant(rot.z), glm::vec3(0, 0, 1));
    return x * y * z;
}

void LocalTransform::reset() {
    _scaling = glm::vec3(1.0f);
    _translation = glm::vec3(0.0f);
    _rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);  // identità (w=1)
    _combined = glm::mat4(1.0f);
}

glm::mat3 LocalTransform::rotation3(float deg, const glm::vec3& axis) {
    glm::vec3 n = glm::normalize(axis);
    glm::mat3 out(1.0f);
    float t = to_radiant(deg);
    float s = sin(t), c = cos(t), c1 = 1.0f - c;

    out[0][0] = n.x * n.x + (1 - n.x * n.x) * c;
    out[0][1] = n.x * n.y * c1 + n.z * s;
    out[0][2] = n.x * n.z * c1 - n.y * s;

    out[1][0] = n.x * n.y * c1 - n.z * s;
    out[1][1] = n.y * n.y + (1 - n.y * n.y) * c;
    out[1][2] = n.y * n.z * c1 + n.x * s;

    out[2][0] = n.x * n.z * c1 + n.y * s;
    out[2][1] = n.y * n.z * c1 - n.x * s;
    out[2][2] = n.z * n.z + (1 - n.z * n.z) * c;

    return out;
}