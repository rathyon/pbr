#include <Perspective.h>

#include <Transform.h>

using namespace pbr;

Perspective::Perspective() {

}

Perspective::Perspective(int32 width, int32 height, const Vec3& eye,
                         const Vec3& at, const Vec3& up, float n, float f, float fov) 
    : Camera(width, height, eye, at, up, n, f), _fov(fov) {

    _projMatrix = perspective(fov, aspect(), n, f);
}

float Perspective::fov() const {
    return _fov;
}