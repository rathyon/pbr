#ifndef __PBR_MATRIX4X4_H__
#define __PBR_MATRIX4X4_H__

#include <PBR.h>

namespace pbr {
namespace math {

    class Vector3;
    class Vector4;
    class Matrix3x3;
    class Quat;

    class PBR_SHARED Matrix4x4 {
    public:
        union {
            struct {
                float m11, m21, m31, m41;
                float m12, m22, m32, m42;
                float m13, m23, m33, m43;
                float m14, m24, m34, m44;
            };

            float m[4][4]; // Column major storage _m[col][row]
        };

        Matrix4x4();
        explicit Matrix4x4(float scalar);
        Matrix4x4(float m11, float m12, float m13, float m14,
                  float m21, float m22, float m23, float m24,
                  float m31, float m32, float m33, float m34,
                  float m41, float m42, float m43, float m44);
        Matrix4x4(const Vector4& col0, const Vector4& col1, const Vector4& col2, const Vector4& col3);
        explicit Matrix4x4(const Matrix3x3& mat);
        explicit Matrix4x4(const Quat& quat);

        Matrix4x4  operator* (float scalar) const;
        Matrix4x4& operator*=(float scalar);

        Vector3    operator* (const Vector3& v)     const;
        Vector4    operator* (const Vector4& v)     const;
        Matrix4x4  operator* (const Matrix4x4& mat) const;

        Matrix4x4  operator+ (const Matrix4x4& mat) const;
        Matrix4x4& operator+=(const Matrix4x4& mat);

        Matrix4x4  operator- (const Matrix4x4& mat) const;
        Matrix4x4& operator-=(const Matrix4x4& mat);

        Matrix4x4 operator-() const;

        bool operator==(const Matrix4x4& mat) const;
        bool operator!=(const Matrix4x4& mat) const;

        float  operator()(uint32 i, uint32 j) const;
        float& operator()(uint32 i, uint32 j);

        float trace() const;
        float det()   const;
    };

    // Standard input/ouput
    PBR_SHARED std::istream& operator>>(std::istream& is, Matrix4x4& mat);
    PBR_SHARED std::ostream& operator<<(std::ostream& os, const Matrix4x4& mat);

    PBR_SHARED Vector3   operator*(const Vector3& v, const Matrix4x4& mat);
    PBR_SHARED Matrix4x4 operator*(float scalar, const Matrix4x4& mat);

    PBR_SHARED Matrix4x4 transpose(const Matrix4x4& mat);
    PBR_SHARED Matrix4x4 inverse(const Matrix4x4& mat);
}
}

#endif