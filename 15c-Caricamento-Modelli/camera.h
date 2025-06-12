#ifndef CAMERA_H
#define CAMERA_H

#include "../base/glm/glm/glm.hpp"
#include "oggetti/sceneObject.h"

/**
	Classe che incapsula tutta la gestione della camera con movimenti, prospttiva e gestione di fov, near plane e far plane. 
*/class Camera {
public:
	Camera();
	~Camera();

	void reset();
	void update();

	void set_perspective(float FOVDeg, float width, float height, float znear, float zfar);

	// Getter
	const glm::mat4& projection();
	const glm::mat4& camera();
	const glm::mat4& CP();
	const glm::vec3& position() const;
	const glm::vec3& target() const;
	const glm::vec3 rotazione() const;
	const float& distanza() const;
	const float& speed() const;

	// Setter
	void set_speed(float speed);

	// Interazioni Blender-style
	void orbit(float deltaYaw, float deltaPitch);// rotazione
	void dolly(float delta); // zoom avanti/indietro
	void pan(float deltaX, float deltaY); // sposta il target


	void setTarget(glm::vec3 pos);

private:
	glm::mat4 _camera;
	glm::mat4 _projection;
	glm::mat4 _combined;

	glm::vec3 _position;
	glm::vec3 _target;
	glm::vec3 _up;

	float _yaw_deg;
	float _pitch_deg;
	float _distance;
	float _speed;

	void update_camera_transform();
	void update_from_angles();
	glm::mat4 perspective_projection(float FOVDeg, float width, float height, float znear, float zfar);
};


#endif


