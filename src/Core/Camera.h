#ifndef __PBR_CAMERA_H__
#define __PBR_CAMERA_H__

#include <SceneObject.h>

namespace pbr {

    struct CameraData {
        Mat4 viewMatrix;
        Mat4 projMatrix;
        Mat4 viewProjMatrix;
        Vec3 viewPos;
    };

    class Camera : public SceneObject {
    public:
        Camera();
        Camera(int32 width, int32 height, const Vec3& eye,
               const Vec3& at, const Vec3& up, float n, float f);

        int32 width()  const;
        int32 height() const;

        float aspect() const;
        float near()   const;
        float far()    const;

        Vec3 right() const;
        Vec3 front() const;
        Vec3 up()    const;

        void lookAt(const Vec3& eye, const Vec3& at, const Vec3& up);
        void lookAt(const Vec3& at);

        const Mat4& viewMatrix() const;
        const Mat4& projMatrix() const;

        Mat4 viewProjMatrix() const;

        void updateOrientation(float dp, float dy);

        void updateViewMatrix();

        // TODO: Rafael
        //Ray traceRay(const Vec2& pixel) const;

    protected:
        Mat4 _projMatrix;

        int32 _width;
        int32 _height;

        float _far;
        float _near;

        float _pitch;
        float _yaw;
    };

}

#endif