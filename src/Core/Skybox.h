#ifndef __PBR_SKYBOX_H__
#define __PBR_SKYBOX_H__

#include <PBR.h>
#include <RenderInterface.h>
#include <Geometry.h>

namespace pbr {

    class Skybox {
    public:
        Skybox(const std::string& folder);
        Skybox(RRID cubeProg, RRID cubeTex);

        void initialize();
        void draw() const;

        RRID irradianceTex() const;
        RRID cubeTex() const;
        RRID ggxTex() const;

    private:
        RRID _cubeProg;      
        RRID _geoId;

        // Textures
        RRID _cubeTex;
        RRID _irradianceTex;
        RRID _ggxTex;

        sref<Geometry> _geo;
    };

}

#endif