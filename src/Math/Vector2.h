#ifndef __PBR_VECTOR2_H__
#define __PBR_VECTOR2_H__

#include <PBR.h>

namespace pbr {
namespace math {

    // Forward declaration
    class Vector3;

    class PBR_SHARED Vector2 {
    public:
        float x, y;

        Vector2();
        Vector2(float scalar);
        Vector2(float x, float y);

        // Vector3 projection - drop z coordinate
        // Make it explicit so as to avoid unintentional use
        explicit Vector2(const Vector3& v);

        // Vector math operators
        Vector2  operator* (float scalar) const;
        Vector2& operator*=(float scalar);

        Vector2  operator/ (float scalar) const;
        Vector2& operator/=(float scalar);

        Vector2  operator+ (const Vector2& v) const;
        Vector2& operator+=(const Vector2& v);

        Vector2  operator- (const Vector2& v) const;
        Vector2& operator-=(const Vector2& v);

        Vector2  operator- () const;

        bool operator==(const Vector2& v) const;
        bool operator!=(const Vector2& v) const;

        // Array-like access
        float  operator[](uint32 idx) const;
        float& operator[](uint32 idx);

        // Vector member methods
        float lengthSqr() const;
        float length()    const;

        void  normalize();

        float min() const;
        float max() const;
        unsigned int maxDim() const;
        unsigned int minDim() const;
    };

    // Standard input/ouput
    PBR_SHARED std::istream& operator>>(std::istream& is, Vector2& v);
    PBR_SHARED std::ostream& operator<<(std::ostream& os, const Vector2& v);

    PBR_SHARED Vector2 operator*(float scalar, const Vector2& v);

    PBR_SHARED Vector2 abs(const Vector2& v);
    PBR_SHARED Vector2 pow(const Vector2& v, float exp);
    PBR_SHARED Vector2 normalize(const Vector2& v);
    PBR_SHARED Vector2 min(const Vector2& v1, const Vector2& v2);
    PBR_SHARED Vector2 max(const Vector2& v1, const Vector2& v2);

    PBR_SHARED float distance(const Vector2& v1, const Vector2& v2);
    PBR_SHARED float dot(const Vector2& v1, const Vector2& v2);
    PBR_SHARED float absDot(const Vector2& v1, const Vector2& v2);
}
}

#endif