#ifndef __PBR_LIGHT_H__
#define __PBR_LIGHT_H__

#include <SceneObject.h>
#include <Spectrum.h>

namespace pbr {

    class Shape;

    // Used to pass the type id to shaders
    enum LightType : uint8 {
        LIGHTYPE_POINT  = 0,
        LIGHTYPE_SPOT   = 1,
        LIGHTYPE_DIR    = 2,
        LIGHTYPE_QUAD   = 3,
        LIGHTYPE_SPHERE = 4
    };

    // Light data for shader blocks
    // CARE: data is properly aligned, do not change
    /*struct LightData {
        bool      state;      // On/off flag
        LightType type;       // Light type
        uint8     aux[2];      
        Vec3      position;   // Light position / direction for dir lights
        float     auxA;        
        Color     emission;   // Non normalized emission (already multiplied by intensity)
        float     auxB;       
        Vec3      sideU;      
        float     auxC;
        Vec3      sideV;
    }; // 64 Bytes*/

    struct LightData {
        Vec3      position;   // Light position / direction for dir lights
        float     auxA;
        Color     emission;   // Non normalized emission (already multiplied by intensity)
        int       type;
        bool      state;      // On/off flag
    }; // 

    class PBR_SHARED Light : public SceneObject {
    public:
        Light();
        Light(const Color& emission, float intensity);
        Light(const Vec3& position, const Color& emission, float intensity);
        Light(const Mat4& lightToWorld, const Color& emission, float intensity);

        bool  isOn()        const;
        bool  castShadows() const;
        float intensity()   const;
        Color emission()    const;

        virtual void toData(LightData& data) const = 0;
        virtual sref<Shape> shape() const;

    protected:
        bool  _on;
        bool  _shadows;
        float _intensity;
        Color _emission;  // Normalized emission
    };

}

#endif