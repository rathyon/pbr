#ifndef __PBR_SHADER_H__
#define __PBR_SHADER_H__

#include <PBR.h>

namespace pbr {

    template<class T>
    using vec = std::vector<T>;

    enum ShaderType {
        VERTEX_SHADER   = 0,
        FRAGMENT_SHADER = 1,
        GEOMETRY_SHADER = 2,
        COMPUTE_SHADER  = 3
    };

    class PBR_SHARED ShaderSource {
    public:
        ShaderSource(ShaderType type, const std::string& filePath);
        ~ShaderSource();

        uint32     id()   const;
        ShaderType type() const;

        const std::string& name()   const;
        const std::string& source() const;       

        bool compile();

    private:
        uint32      _id;
        std::string _name;
        std::string _source;
        ShaderType  _type;
    };

    class PBR_SHARED Shader {
    public:
        Shader(const std::string& name);

        uint32 id() const;
        bool addShader(const ShaderSource& source);
        bool link();

        const std::string& name()    const;
        const vec<uint32>& shaders() const;

        void registerUniform(const std::string& name);
        void registerUniformBlock(const std::string& name);

    private:
        uint32      _id;
        std::string _name;
        vec<uint32> _shaders;
        vec<int32>  _uniforms;
        vec<uint32> _uniformBlocks;
    };



}

#endif