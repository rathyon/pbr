#ifndef __PBR_MATRIX3X3_H__
#define __PBR_MATRIX3X3_H__

#include <PBR.h>

namespace pbr {
namespace math {

    class Vector3;
    class Matrix4x4;

    class PBR_SHARED Matrix3x3 {
    public:
        union {
            struct {
                float m11, m21, m31;
                float m12, m22, m32;
                float m13, m23, m33;
            };

            float m[3][3]; // Column major storage _m[col][row]
        };

        Matrix3x3();
        explicit Matrix3x3(float scalar);
        Matrix3x3(float m11, float m12, float m13,
                    float m21, float m22, float m23,
                    float m31, float m32, float m33);
        Matrix3x3(const Vector3& col0, const Vector3& col1, const Vector3& col2);
        explicit Matrix3x3(const Matrix4x4& mat);

        Matrix3x3  operator* (float scalar) const;
        Matrix3x3& operator*=(float scalar);

        Vector3    operator* (const Vector3& v)     const;
        Matrix3x3  operator* (const Matrix3x3& mat) const;

        Matrix3x3  operator+ (const Matrix3x3& mat) const;
        Matrix3x3& operator+=(const Matrix3x3& mat);

        Matrix3x3  operator- (const Matrix3x3& mat) const;
        Matrix3x3& operator-=(const Matrix3x3& mat);

        Matrix3x3  operator- () const;

        bool operator==(const Matrix3x3& mat) const;
        bool operator!=(const Matrix3x3& mat) const;

        float  operator()(uint32 i, uint32 j) const;
        float& operator()(uint32 i, uint32 j);

        float trace() const;
        float det()   const;
    };

    // Standard input/ouput
    PBR_SHARED std::istream& operator>>(std::istream& is, Matrix3x3& mat);
    PBR_SHARED std::ostream& operator<<(std::ostream& os, const Matrix3x3& mat);

    PBR_SHARED Vector3   operator*(const Vector3& v, const Matrix3x3& mat);
    PBR_SHARED Matrix3x3 operator*(float scalar, const Matrix3x3& mat);

    PBR_SHARED Matrix3x3 transpose(const Matrix3x3& mat);
    PBR_SHARED Matrix3x3 inverse(const Matrix3x3& mat);
}
}

#endif