#include <Camera.h>

#include <Transform.h>

using namespace pbr;

Camera::Camera() { }
Camera::Camera(int32 width, int32 height, const Vec3& eye,
               const Vec3& at, const Vec3& up, float n, float f) 
    : _width(width), _height(height), _near(n), _far(f) {
    
    lookAt(eye, at, up);

    Vector3 viewDir = normalize(at - eye);
    _pitch = asinf(-viewDir.y); // -viewDir.y
    _yaw   = atan2f(viewDir.x, -viewDir.z);
}

int32 Camera::width() const {
    return _width;
}

int32 Camera::height() const {
    return _height;
}

float Camera::aspect() const {
    return (float)_width / (float)_height;
}

float Camera::near() const {
    return _near;
}

float Camera::far() const {
    return _far;
}

Vec3 Camera::right() const {
    return Vec3(_objToWorld.m11,
                _objToWorld.m12,
                _objToWorld.m13);
}

Vec3 Camera::front() const {
    return Vec3(_objToWorld.m31, 
                _objToWorld.m32, 
                _objToWorld.m33);
}

Vec3 Camera::up() const {
    return Vec3(_objToWorld.m21,
                _objToWorld.m22,
                _objToWorld.m23);
}

void Camera::lookAt(const Vec3& eye, const Vec3& at, const Vec3& up) {
    _position    = eye;
    _objToWorld  = math::lookAt(eye, at, up);

    Mat4 matOrient = _objToWorld;
    matOrient.m14 = 0; matOrient.m24 = 0; matOrient.m34 = 0; matOrient.m44 = 1;
    _orientation = Quat(matOrient);
}

void Camera::lookAt(const Vec3& at) {
    _objToWorld = math::lookAt(position(), at, normalize(Vec3(0.01, 1.0, 0.0)));
}

const Mat4& Camera::viewMatrix() const {
    return _objToWorld;
}

const Mat4& Camera::projMatrix() const {
    return _projMatrix;
}

Mat4 Camera::viewProjMatrix() const {
    return projMatrix() * viewMatrix();
}

void Camera::updateViewMatrix() {
    Matrix4x4 rotX = rotationAxis(_pitch, Vector3(1, 0, 0));
    Matrix4x4 rotY = rotationAxis(_yaw,   Vector3(0, 1, 0));

    Matrix4x4 orientation = rotX * rotY;

    _objToWorld = orientation * translation(-_position);

    //_objToWorld = translation(-_position) * _orientation.toMatrix();
}

void Camera::updateOrientation(float dpdx, float dydx) {
/*    _pitch += dpdx;
    _yaw   -= dydx;

    // Normalize angles
    _yaw = fmodf(_yaw, 2.0f * PI);
    if (_yaw < 0.0f)
        _yaw += 2.0f * PI;

    if (_pitch > PI / 2.0f)
        _pitch = PI / 2.0f;
    else if (_pitch < -PI / 2.0f)
        _pitch = -PI / 2.0f;

    updateViewMatrix();*/

    _pitch += dpdx;
    _yaw   -= dydx;

    // Normalize angles
    _yaw = fmodf(_yaw, 2.0f * PI);
    if (_yaw < 0.0f)
        _yaw += 2.0f * PI;

    if (_pitch > PI / 2.0f)
        _pitch = PI / 2.0f;
    else if (_pitch < -PI / 2.0f)
        _pitch = -PI / 2.0f;

    //updateViewMatrix();
}