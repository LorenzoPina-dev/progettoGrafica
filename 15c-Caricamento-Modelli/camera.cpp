#include "camera.h"
#include <cassert>
#include <cmath>
#define GLM_FORCE_RADIANS
#include "../base/glm/glm/gtc/matrix_transform.hpp"
#include "camera.h"
#include <algorithm>



Camera::~Camera() {}
Camera::Camera() {
	reset();
}

void Camera::reset() {
	_target    = glm::vec3(0.0f, 0.0f, 0.0f);
	_distance  = 40.0f;
	_yaw_deg   = 90.0f;
	_pitch_deg = 0.0f;
	_up        = glm::vec3(0.0f, 1.0f, 0.0f);
	_speed     = 0.1f;

	update_from_angles();
	update();
}

void Camera::update() {
	update_camera_transform();
	_combined = _projection * _camera;
}

void Camera::update_camera_transform() {
	_camera = glm::lookAt(_position, _target, _up);
}

void Camera::update_from_angles() {
	// Calcola posizione orbitale a partire da yaw/pitch/distanza
	//Camera orbitale intorno a _target
	float yawRad = glm::radians(_yaw_deg);
	float pitchRad = glm::radians(_pitch_deg);

	//mantiene la distanza settata
	glm::vec3 offset;
	offset.x = _distance * cos(pitchRad) * cos(yawRad);
	offset.y = _distance * sin(pitchRad);
	offset.z = _distance * cos(pitchRad) * sin(yawRad);

	//utilizza _target come partenza su cui applicare offset
	_position = _target + offset;
	update_camera_transform();
}

void Camera::orbit(float deltaYaw, float deltaPitch) {
	_yaw_deg   += deltaYaw;
	_pitch_deg += deltaPitch;

	_pitch_deg = glm::clamp(_pitch_deg, -89.0f, 89.0f);
	update_from_angles();
	update();
}


void Camera::dolly(float delta) {
	_distance *= (1.0f - delta * 0.1f);
	_distance = glm::clamp(_distance, 0.1f, 100.0f);
	update_from_angles();
}

void Camera::pan(float deltaX, float deltaY) {
	glm::vec3 viewDir = glm::normalize(_target - _position);
	glm::vec3 right   = glm::normalize(glm::cross(viewDir, _up));
	glm::vec3 upMove  = glm::normalize(_up);

	_target += -right * deltaX * _speed + upMove * deltaY * _speed;
	update_from_angles();
}

void Camera::set_perspective(float FOVDeg, float width, float height, float znear, float zfar) {
	_projection = perspective_projection(FOVDeg, width, height, znear, zfar);
	update();
}

const glm::mat4& Camera::projection() { return _projection; }
const glm::mat4& Camera::camera()     { return _camera; }
const glm::mat4& Camera::CP()         { return _combined; }
const glm::vec3& Camera::position() const { return _position; }
const glm::vec3& Camera::target() const { return _target; }
const glm::vec3 Camera::rotazione() const{ return glm::vec3(_yaw_deg,_pitch_deg,0); }
const float& Camera::distanza() const{ return _distance; }
const float& Camera::speed() const { return _speed; }
void Camera::set_speed(float speed) { _speed = speed; }

glm::mat4 Camera::perspective_projection(float FOVDeg, float width, float height, float znear, float zfar) {
	return glm::perspective(glm::radians(FOVDeg), width / height, znear, zfar);
}


void Camera::setTarget(glm::vec3 pos){
	_target=pos;
}