#include <PBRMath.h>

namespace pbr {
    namespace math {

        Float max(Float x, Float y) {
            return std::max(x, y);
        }

        Float min(Float x, Float y) {
            return std::min(x, y);
        }

        int32 sign(Float scalar) {
            if (scalar < 0)
                return -1;

            return 1;
        }

        Float radians(Float degrees) {
            return (PI / 180) * degrees;
        }

        Float degrees(Float radians) {
            return (180 / PI) * radians;
        }

        Float log2(Float x) {
            return std::log(x) * INVLOG2;
        }

        Float sqrtSafe(Float x) {
            return std::sqrt(std::max((Float)0.0, x));
        }

        Float acosSafe(Float x) {
            return std::acos(clamp(x, -1.0, 1.0));
        }

        Float erf(Float x) {
            Float a1 = (Float) 0.254829592;
            Float a2 = (Float)-0.284496736;
            Float a3 = (Float) 1.421413741;
            Float a4 = (Float)-1.453152027;
            Float a5 = (Float) 1.061405429;
            Float p = (Float) 0.3275911;

            // Save the sign of x
            Float sign = math::sign(x);
            x = std::abs(x);

            // A&S formula 7.1.26
            Float t = (Float) 1.0 / ((Float) 1.0 + p*x);
            Float y = (Float) 1.0 - (((((a5*t + a4)*t) + a3)*t + a2)*t + a1) * t * std::exp(-x * x);

            return sign * y;
        }

        Float erfInv(Float x) {
            /* ---------------------------------------------------------
            "Approximating the erfinv function" - Mike Giles
            -----------------------------------------------------------*/
            Float w, p;

            w = -std::log(((Float)1 - x) * ((Float)1 + x));

            if (w < (Float)5) {
                w = w - (Float) 2.5;
                p = (Float) 2.81022636e-08;
                p = (Float) 3.43273939e-07 + p*w;
                p = (Float)-3.5233877e-06 + p*w;
                p = (Float)-4.39150654e-06 + p*w;
                p = (Float) 0.00021858087 + p*w;
                p = (Float)-0.00125372503 + p*w;
                p = (Float)-0.00417768164 + p*w;
                p = (Float) 0.246640727 + p*w;
                p = (Float) 1.50140941 + p*w;
            } else {
                w = std::sqrt(w) - (Float)3;
                p = (Float)-0.000200214257;
                p = (Float) 0.000100950558 + p*w;
                p = (Float) 0.00134934322 + p*w;
                p = (Float)-0.00367342844 + p*w;
                p = (Float) 0.00573950773 + p*w;
                p = (Float)-0.0076224613 + p*w;
                p = (Float) 0.00943887047 + p*w;
                p = (Float) 1.00167406 + p*w;
                p = (Float) 2.83297682 + p*w;
            }

            return p*x;
        }

        Float lerp(Float t, Float v1, Float v2) {
            return (1 - t) * v1 + t * v2;
        }

        bool solQuadratic(Float a, Float b, Float c, Float* x0, Float* x1) {
            double disc = b * b - 4 * a * c;
            if (disc < 0)
                return false;

            double rootDisc = std::sqrt(disc);
            double q;
            if (b < 0)
                q = -0.5 * (b - rootDisc);
            else
                q = -0.5 * (b + rootDisc);

            *x0 = q / a;
            *x1 = c / q;

            if (*x0 > *x1)
                std::swap(x0, x1);

            return true;
        }

        bool solSystem2x2(const Matrix2x2& A, const Vector2& b, Float* x0, Float* x1) {
            Float det = A.det();
            if (std::abs(det) < FLOAT_EPSILON)
                return false;

            *x0 = (A.m22 * b[0] - A.m12 * b[1]) / det;
            *x1 = (A.m11 * b[1] - A.m21 * b[0]) / det;

            if (std::isnan(*x0) || std::isnan(*x1))
                return false;

            return true;
        }

        bool newtonRaphson(Float x0, Float* sol, std::function<Float(Float)> f, std::function<Float(Float)> df, uint32 iters) {
            Float xn = x0;
            for (uint32 i = 0; i < iters; ++i) {
                Float deriv = df(xn);
                if (deriv == 0)
                    return false;

                Float c = xn - f(xn) / deriv;
                if (std::abs(c - xn) < FLOAT_EPSILON * std::abs(c)) {
                    *sol = c;
                    break;
                }

                xn = c;
            }

            *sol = xn;
            return true;
        }
    }

}