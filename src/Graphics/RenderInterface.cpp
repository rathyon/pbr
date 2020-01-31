#pragma warning(disable : 4018)

#include <RenderInterface.h>

#include <Geometry.h>
#include <Utils.h>

#include <Image.h>
#include <Texture.h>
#include <Resources.h>

#include <Renderer.h>

using namespace pbr;
using namespace pbr::math;

const GLenum OGLShaderTypes[] = {
    GL_VERTEX_SHADER,
    GL_FRAGMENT_SHADER,
    GL_GEOMETRY_SHADER,
    GL_COMPUTE_SHADER
};

const GLenum OGLBufferTargets[] = {
    GL_ARRAY_BUFFER,
    GL_ELEMENT_ARRAY_BUFFER,
    GL_UNIFORM_BUFFER
};

const GLenum OGLAttrTypes[] = {
    GL_BYTE,
    GL_SHORT,
    GL_UNSIGNED_INT,
    GL_FLOAT
};

const GLenum OGLBufferUsage[] = {
    GL_STATIC_DRAW,
    GL_STREAM_DRAW,
    GL_DYNAMIC_DRAW
};


const GLenum OGLTexTargets[] = {
    GL_TEXTURE_1D,
    GL_TEXTURE_2D,
    GL_TEXTURE_3D,
    GL_TEXTURE_CUBE_MAP,
    GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

const GLenum OGLTexSizedFormats[] = {
    0,

    // Unsigned formats
    GL_R8,
    GL_RG8,
    GL_RGB8,
    GL_RGBA8,

    GL_R16,
    GL_RG16,
    GL_RGB16,
    GL_RGBA16,

    // Signed IMGFMTs
    GL_R8I,
    GL_RG8I,
    GL_RGB8I,
    GL_RGBA8I,

    GL_R16I,
    GL_RG16I,
    GL_RGB16I,
    GL_RGBA16I,

    // Float IMGFMTs
    GL_R16F,
    GL_RG16F,
    GL_RGB16F,
    GL_RGBA16F,

    GL_R32F,
    GL_RG32F,
    GL_RGB32F,
    GL_RGBA32F,

    // Signed integer IMGFMTs
    GL_R16I,
    GL_RG16I,
    GL_RGB16I,
    GL_RGBA16I,

    GL_R32I,
    GL_RG32I,
    GL_RGB32I,
    GL_RGBA32I,

    // Unsigned integer IMGFMTs
    GL_R16UI,
    GL_RG16UI,
    GL_RGB16UI,
    GL_RGBA16UI,

    GL_R32UI,
    GL_RG32UI,
    GL_RGB32UI,
    GL_RGBA32UI,

    // Packed formats
    0,
    0,
    0,
    0,
    0,
    0,

    // Depth IMGFMTs
    GL_DEPTH_COMPONENT16,
    GL_DEPTH_COMPONENT24,
    GL_DEPTH24_STENCIL8,
    GL_DEPTH_COMPONENT32,

    // Compressed IMGFMTs
    0,
    0,
    0,
    0,
    0,
    0
};

const GLenum OGLTexPixelFormats[] = {
    0,

    // Unsigned IMGFMTs
    GL_RED,
    GL_RG,
    GL_RGB,
    GL_RGBA,

    GL_RED,
    GL_RG,
    GL_RGB,
    GL_RGBA,

    // Signed IMGFMTs
    GL_RED,
    GL_RG,
    GL_RGB,
    GL_RGBA,

    GL_RED,
    GL_RG,
    GL_RGB,
    GL_RGBA,

    // Float IMGFMTs
    GL_RED,
    GL_RG,
    GL_RGB,
    GL_RGBA,

    GL_RED,
    GL_RG,
    GL_RGB,
    GL_RGBA,

    // Signed integer IMGFMTs
    GL_RED,
    GL_RG,
    GL_RGB,
    GL_RGBA,

    GL_RED,
    GL_RG,
    GL_RGB,
    GL_RGBA,

    // Unsigned integer IMGFMTs
    GL_RED,
    GL_RG,
    GL_RGB,
    GL_RGBA,

    GL_RED,
    GL_RG,
    GL_RGB,
    GL_RGBA,

    // Packed IMGFMTs
    0,
    0,
    0,
    0,
    0,
    0,

    // Depth IMGFMTs
    GL_DEPTH_COMPONENT,
    GL_DEPTH_COMPONENT,
    GL_DEPTH_STENCIL,
    GL_DEPTH_COMPONENT,

    // Compressed IMGFMTs
    0,
    0,
    0,
    0,
    0,
    0
};

const GLenum OGLTexPixelTypes[] = {
    GL_UNSIGNED_BYTE,
    GL_BYTE,
    GL_UNSIGNED_SHORT,
    GL_SHORT,
    GL_UNSIGNED_INT,
    GL_INT,
    GL_FLOAT,
    GL_UNSIGNED_BYTE_3_3_2,
    GL_UNSIGNED_BYTE_2_3_3_REV,
    GL_UNSIGNED_SHORT_5_6_5,
    GL_UNSIGNED_SHORT_5_6_5_REV,
    GL_UNSIGNED_SHORT_4_4_4_4,
    GL_UNSIGNED_SHORT_4_4_4_4_REV,
    GL_UNSIGNED_SHORT_5_5_5_1,
    GL_UNSIGNED_SHORT_1_5_5_5_REV,
    GL_UNSIGNED_INT_8_8_8_8,
    GL_UNSIGNED_INT_8_8_8_8_REV,
    GL_UNSIGNED_INT_10_10_10_2,
    GL_UNSIGNED_INT_2_10_10_10_REV
};

const GLenum OGLTexFilters[] = { 
    GL_NEAREST,
    GL_LINEAR,
    GL_NEAREST_MIPMAP_NEAREST,
    GL_LINEAR_MIPMAP_NEAREST,
    GL_NEAREST_MIPMAP_LINEAR,
    GL_LINEAR_MIPMAP_LINEAR 
};

const GLenum OGLTexWrapping[] = { 
    GL_REPEAT,
    GL_MIRRORED_REPEAT,
    GL_CLAMP_TO_EDGE,
    GL_CLAMP_TO_BORDER
};

using namespace pbr;

RenderInterface::RenderInterface() {

}

RenderInterface::~RenderInterface() {   

}

RenderInterface& RenderInterface::get() {
    static RenderInterface _inst;
    return _inst;
}

void RenderInterface::initialize() {
    _programs.push_back({ 0 });
    _currProgram = 0;
    
    // Load BRDF precomputation
    TexSampler brdfSampler;
    Image brdf;
    brdf.loadImage("PBR/brdf.img");
    RRID brdfId = createTexture(brdf, brdfSampler);
    Resource.addTexture("brdf", RHI.getTexture(brdfId));
        
    // Load standard engine shaders
    ShaderSource fsCommon(FRAGMENT_SHADER, "common.fs");

    // Load unreal shader
    ShaderSource vsUnreal(VERTEX_SHADER, "unreal.vs");
    ShaderSource fsUnreal(FRAGMENT_SHADER, "unreal.fs");

    sref<Shader> unrealProg = make_sref<Shader>("unreal");
    unrealProg->addShader(vsUnreal);
    unrealProg->addShader(fsUnreal);
    unrealProg->addShader(fsCommon);
    unrealProg->link();
    Resource.addShader("unreal", unrealProg);

    RHI.useProgram(unrealProg->id());
    RHI.setSampler("irradianceTex", 6);
    RHI.setSampler("ggxTex",        7);
    RHI.setSampler("brdfTex",       8);
    RHI.setBufferBlock("cameraBlock",   CAMERA_BUFFER_IDX);
    RHI.setBufferBlock("rendererBlock", RENDERER_BUFFER_IDX);
    RHI.setBufferBlock("lightBlock",    LIGHTS_BUFFER_IDX);
    RHI.useProgram(0);

    // Load environment shader
    ShaderSource vsSkybox(VERTEX_SHADER,   "skybox.vs");
    ShaderSource fsSkybox(FRAGMENT_SHADER, "skybox.fs");
    sref<Shader> skyProg = make_sref<Shader>("skybox");
    skyProg->addShader(vsSkybox);
    skyProg->addShader(fsSkybox);
    skyProg->addShader(fsCommon);
    skyProg->link();

    Resource.addShader("skybox", skyProg);

    RHI.useProgram(skyProg->id());
    skyProg->registerUniform("envMap");
    RHI.setSampler("envMap", 5);
    RHI.setBufferBlock("rendererBlock", RENDERER_BUFFER_IDX);
    RHI.setBufferBlock("cameraBlock",   CAMERA_BUFFER_IDX);
    RHI.useProgram(0);

    // Set BRDF precomputation
    glActiveTexture(GL_TEXTURE8);
    RHI.bindTexture(brdfId);
}

RRID RenderInterface::uploadGeometry(const sref<Geometry>& geo) {
    auto verts   = geo->vertices();
    auto indices = geo->indices();

    // Create vertex array for the geometry
    RRID resId = createVertexArray();

    RHIVertArray& vertArray = _vertArrays[resId];
    glBindVertexArray(vertArray.id);

    // Create VBOs for vertex data and indices
    RRID vboIds[2] = { 0, 0 };
    vboIds[0] = createBuffer(BUFFER_VERTEX, BufferUsage::STATIC, sizeof(Vertex) * verts.size(), &verts[0]);

    BufferLayoutEntry entries[] = { { 0, 3, ATTRIB_FLOAT, sizeof(Vertex), offsetof(Vertex, position) },
                                    { 1, 3, ATTRIB_FLOAT, sizeof(Vertex), offsetof(Vertex, normal) },
                                    { 2, 2, ATTRIB_FLOAT, sizeof(Vertex), offsetof(Vertex, uv) },
                                    { 3, 3, ATTRIB_FLOAT, sizeof(Vertex), offsetof(Vertex, tangent) } };

    BufferLayout layout = { 4, &entries[0] };
    setBufferLayout(vboIds[0], layout);

    if (indices.size() > 0)
        vboIds[1] = createBuffer(BUFFER_INDEX, BufferUsage::STATIC, sizeof(uint32) * indices.size(), &indices[0]);

    // Associate created VBOs with the VAO
    vertArray.buffers.push_back(vboIds[0]);
    vertArray.buffers.push_back(vboIds[1]);

    vertArray.numVertices = (GLsizei)verts.size();
    vertArray.numIndices  = (GLsizei)indices.size();

    glBindVertexArray(0);

    // Associate RRID of the VAO with the geometry
    geo->setRRID(resId);

    return resId;
}

void RenderInterface::drawGeometry(RRID id) {
    if (id < 0 || id >= _vertArrays.size())
        return; // Error

    RHIVertArray& vao = _vertArrays[id];
    if (vao.id == 0)
        return; // Error

    glBindVertexArray(vao.id);

    glDrawArrays(GL_TRIANGLES, 0, vao.numVertices);

    glBindVertexArray(0);
}

RRID RenderInterface::createVertexArray() {
    RHIVertArray vertArray;

    glGenVertexArrays(1, &vertArray.id);

    RRID resId = _vertArrays.size();
    _vertArrays.push_back(vertArray);

    return resId;
}

bool RenderInterface::deleteVertexArray(RRID id) {
    if (id < 0)
        return false; // Error

    RHIVertArray vao = _vertArrays[id];
    if (vao.id != 0) {
        glDeleteVertexArrays(1, &vao.id);
        vao.id = 0;
        vao.buffers.clear();
        vao.numIndices = 0;
        return true;
    }

    return false;
}

RRID RenderInterface::createBuffer(BufferType type, BufferUsage usage, size_t size, void* data) {
    RHIBuffer buffer;
    buffer.target = OGLBufferTargets[type];

    glGenBuffers(1, &buffer.id);
    glBindBuffer(buffer.target, buffer.id);
    glBufferData(buffer.target, size, data, OGLBufferUsage[usage]);
    glBindBuffer(buffer.target, 0);

    RRID resId = _buffers.size();
    _buffers.push_back(buffer);

    return resId;
}

void RenderInterface::bindBufferBase(RRID id, uint32 index) {
    if (id < 0 || id >= _buffers.size())
        return; // Error

    RHIBuffer buffer = _buffers[id];

    glBindBuffer(buffer.target, buffer.id);
    glBindBufferBase(buffer.target, index, buffer.id);
    glBindBuffer(buffer.target, 0);
}

void RenderInterface::setBufferLayout(RRID id, uint32 idx, AttribType type, uint32 numElems, uint32 stride, size_t offset) {
    if (id < 0 || id >= _buffers.size())
        return; // Error

    RHIBuffer buffer = _buffers[id];

    if (buffer.id == 0 || buffer.target != BUFFER_VERTEX)
        return; // Error

    glBindBuffer(buffer.target, buffer.id);
    glEnableVertexAttribArray(idx);
    glVertexAttribPointer(idx, numElems, OGLAttrTypes[type], GL_FALSE, (GLsizei)stride, (const void*)offset);
    glBindBuffer(buffer.target, 0);
}

void RenderInterface::setBufferLayout(RRID id, const BufferLayout& layout) {
    if (id < 0 || id >= _buffers.size())
        return; // Error

    RHIBuffer buffer = _buffers[id];
    if (buffer.id == 0 || buffer.target != OGLBufferTargets[BUFFER_VERTEX])
        return; // Error

    glBindBuffer(buffer.target, buffer.id);

    for (uint32 i = 0; i < layout.numEntries; ++i) {
        const BufferLayoutEntry& entry = layout.entries[i];

        glEnableVertexAttribArray(entry.index);
        glVertexAttribPointer(entry.index, entry.numElems, OGLAttrTypes[entry.type], GL_FALSE, (GLsizei)entry.stride, (const void*)entry.offset);
    }

    glBindBuffer(buffer.target, 0);
}

bool RenderInterface::updateBuffer(RRID id, size_t size, void* data) {
    if (id < 0 || id >= _buffers.size())
        return false; // Error

    RHIBuffer buffer = _buffers[id];
    if (buffer.id == 0)
        return false; // Error

    glBindBuffer(buffer.target, buffer.id);
    GLvoid* p = glMapBuffer(buffer.target, GL_WRITE_ONLY);
    memcpy(p, data, size);
    glUnmapBuffer(buffer.target);
    glBindBuffer(buffer.target, 0);

    return true;
}

bool RenderInterface::deleteBuffer(RRID id) {
    if (id < 0 || id >= _buffers.size())
        return false; // Error

    RHIBuffer buffer = _buffers[id];
    if (buffer.id != 0) {
        glDeleteBuffers(1, &buffer.id);
        buffer.id = 0;
        return true;
    }

    return false;
}

uint32 RenderInterface::compileShader(const ShaderSource& source) {
    // Create shader id
    GLuint id = glCreateShader(OGLShaderTypes[source.type()]);
    if (id == 0)
        std::cerr << "Could not create shader: " + source.name();

    // Set shader source and compile
    const char* c_str = source.source().c_str();
    glShaderSource(id, 1, &c_str, 0);
    glCompileShader(id);

    // Check if shader compiled
    GLint result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_TRUE)
        return id;

    // Check shader log for the error and print it
    GLint logLen;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLen);

    char* log = new char[logLen];
    glGetShaderInfoLog(id, logLen, &logLen, log);

    std::string message(log);
    delete[] log;

    std::cerr << "Shader " << source.name() << " compilation log:\n" << message;

    // Cleanup the shader
    glDeleteShader(id);

    return id;
}

bool RenderInterface::deleteShader(const ShaderSource& source) {
    uint32 id = source.id();
    if (id != 0) {
        glDeleteShader(id);
        return true;
    }

    return false;
}

RRID RenderInterface::linkProgram(const Shader& shader) {
    // Create program
    GLuint id = glCreateProgram();
    if (id == 0) {
        std::string message = getProgramError(shader);
        Utils::throwError("Could not create program " + shader.name());
    }

    // Attach shaders
    for (GLuint sid : shader.shaders()) {
        glAttachShader(id, sid);

        // Check attachment error
        checkOpenGLError("Could not attach shader (" + std::to_string(sid) +
                         ") to program (" + std::to_string(id) + ") " + shader.name() + ".");
    }

    glLinkProgram(id);

    GLint res;
    glGetProgramiv(id, GL_LINK_STATUS, &res);
    if (res != GL_TRUE) {
        // Check program log for the error and print it
        std::string message = getProgramError(shader);
        Utils::throwError(message);

        // Detach shaders
        for (GLuint sid : shader.shaders())
            glDetachShader(id, sid);

        // Delete the program
        glDeleteProgram(id);

        return false;
    }

    // Detach shaders after successful linking
    for (GLuint sid : shader.shaders())
        glDetachShader(id, sid);

    RRID rrid = _programs.size();
    _programs.push_back({ id });

    return rrid;
}

std::string RenderInterface::getProgramError(const Shader& shader) {
    GLint logLen;
    glGetProgramiv(shader.id(), GL_INFO_LOG_LENGTH, &logLen);

    char* log = new char[logLen];
    glGetProgramInfoLog(shader.id(), logLen, &logLen, log);

    std::string strLog(log);
    delete[] log;

    return strLog;
}

void RenderInterface::useProgram(RRID id) {
    glUseProgram(_programs[id].id);
    _currProgram = id;
}

void RenderInterface::setFloat(const std::string& name, float val) {
    GLuint id = _programs[_currProgram].id;
    GLint loc = glGetUniformLocation(id, name.c_str());
    glUniform1f(loc, val);
}

void RenderInterface::setVector3(const std::string& name, const Vec3& vec) {
    GLuint id = _programs[_currProgram].id;
    GLint loc = glGetUniformLocation(id, name.c_str());
    glUniform3fv(loc, 1, (const GLfloat*)&vec);
}

void RenderInterface::setVector4(const std::string& name, const Vec4& vec) {
    GLuint id = _programs[_currProgram].id;
    GLint loc = glGetUniformLocation(id, name.c_str());
    glUniform4fv(loc, 1, (const GLfloat*)&vec);
}

void RenderInterface::setMatrix3(const std::string& name, const Mat3& mat) {
    GLuint id = _programs[_currProgram].id;
    GLint loc = glGetUniformLocation(id, name.c_str());
    glUniformMatrix3fv(loc, 1, GL_FALSE, (const GLfloat*)&mat);
}

void RenderInterface::setMatrix4(const std::string& name, const Mat4& mat) {
    GLuint id = _programs[_currProgram].id;
    GLint loc = glGetUniformLocation(id, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)&mat);
}

void RenderInterface::setSampler(const std::string& name, uint32 id) {
    GLuint pid = _programs[_currProgram].id;
    GLint loc  = glGetUniformLocation(pid, name.c_str());
    glUniform1i(loc, id);
}

void RenderInterface::setFloat(int32 loc, float val) {
    glUniform1f(loc, val);
}

void RenderInterface::setVector3(int32 loc, const Vec3& vec) {
    glUniform3fv(loc, 1, (const GLfloat*)&vec);
}

void RenderInterface::setVector4(int32 loc, const Vec4& vec) {
    glUniform4fv(loc, 1, (const GLfloat*)&vec);
}

void RenderInterface::setMatrix3(int32 loc, const Mat3& mat) {
    glUniformMatrix3fv(loc, 1, GL_FALSE, (const GLfloat*)&mat);
}

void RenderInterface::setMatrix4(int32 loc, const Mat4& mat) {
    glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)&mat);
}

void RenderInterface::setBufferBlock(const std::string& name, uint32 binding) {
    GLuint id = _programs[_currProgram].id;
    GLint idx = glGetUniformBlockIndex(id, name.c_str());
    glUniformBlockBinding(id, idx, binding);
}

int32 RenderInterface::uniformLocation(RRID id, const std::string& name) {
    GLuint pid = _programs[id].id;
    return glGetUniformLocation(pid, name.c_str());
}
 
uint32 RenderInterface::uniformBlockLocation(RRID id, const std::string& name) {
    GLuint pid = _programs[id].id;
    return glGetUniformBlockIndex(pid, name.c_str());
}

void RenderInterface::checkOpenGLError(const std::string& error) {
    if (isOpenGLError()) {
        std::cerr << error << std::endl;
        std::cin.get();
        exit(EXIT_FAILURE);
    }
}

bool RenderInterface::isOpenGLError() {
    bool isError = false;
    GLenum errCode;
    const GLubyte *errString;
    while ((errCode = glGetError()) != GL_NO_ERROR) {
        isError = true;
        errString = gluErrorString(errCode);
        std::cerr << "OpenGL ERROR [" << errString << "]." << std::endl;
    }
    return isError;
}

RRID RenderInterface::createTexture(const Image& img, const TexSampler& sampler) {
    GLuint id = 0;
    GLenum target = OGLTexTargets[img.type()];

    RRID resId = _textures.size();

    glGenTextures(1, &id);
    glBindTexture(target, id);

    GLenum pType  = OGLTexPixelTypes[img.compType()];
    GLenum oglFmt = OGLTexPixelFormats[img.format()];

    // Upload all levels
    ImageType type = img.type();
    for (uint32 lvl = 0; lvl < img.numLevels(); ++lvl) {
        uint32 w = mipDimension(img.width(), lvl);
        uint32 h = mipDimension(img.height(), lvl);
        uint32 d = mipDimension(img.depth(), lvl);

        if (type == IMGTYPE_2D)
            glTexImage2D(target, lvl, oglFmt, w, h, 0, oglFmt, pType, img.data(lvl));
        else if (type == IMGTYPE_1D)
            glTexImage1D(target, lvl, oglFmt, w, 0, oglFmt, pType, img.data(lvl));
        else if (type == IMGTYPE_3D)
            glTexImage3D(target, lvl, oglFmt, w, h, d, 0, oglFmt, pType, img.data(lvl));
    }

    glTexParameteri(target, GL_TEXTURE_WRAP_S,     OGLTexWrapping[sampler.sWrap()]);
    glTexParameteri(target, GL_TEXTURE_WRAP_T,     OGLTexWrapping[sampler.tWrap()]);
    glTexParameteri(target, GL_TEXTURE_WRAP_R,     OGLTexWrapping[sampler.rWrap()]);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, OGLTexFilters[sampler.minFilter()]);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, OGLTexFilters[sampler.magFilter()]);

    // Unbind texture
    glBindTexture(target, 0);

    TexFormat fmt;
    fmt.imgFmt  = img.format();
    fmt.imgType = img.type();
    fmt.levels  = img.numLevels();
    fmt.pType   = img.compType();

    sref<Texture> tex = make_sref<GPUTexture>(resId, img.width(), img.height(), img.depth(), sampler, fmt);

    _textures.push_back({ id, target, oglFmt, oglFmt, pType, tex });

    return resId;
}

// Create texture
RRID RenderInterface::createTexture(ImageType type, ImageFormat fmt, uint32 width, uint32 height, uint32 depth, const TexSampler& sampler) {
    GLuint id = 0;
    GLenum target = OGLTexTargets[type];

    RRID resId = _textures.size();

    if (type == IMGTYPE_2D) {
        depth = 1;
    } else if (type == IMGTYPE_1D) {
        height = depth = 1;
    }

    if (type == IMGTYPE_2D && sampler.numSamples() > 0)
        target = GL_TEXTURE_2D_MULTISAMPLE;

    glGenTextures(1, &id);
    glBindTexture(target, id);

    GLenum intFormat = OGLTexSizedFormats[fmt];
    if (type == IMGTYPE_2D && sampler.numSamples() > 0)
        glTexImage2DMultisample(target, sampler.numSamples(), intFormat, width, height, GL_TRUE);

    // Set the sampler
    glTexParameteri(target, GL_TEXTURE_WRAP_S,     OGLTexWrapping[sampler.sWrap()]);
    glTexParameteri(target, GL_TEXTURE_WRAP_T,     OGLTexWrapping[sampler.tWrap()]);
    glTexParameteri(target, GL_TEXTURE_WRAP_R,     OGLTexWrapping[sampler.rWrap()]);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, OGLTexFilters[sampler.minFilter()]);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, OGLTexFilters[sampler.magFilter()]);

    // Unbind texture
    glBindTexture(target, 0);

    TexFormat texFmt;
    texFmt.imgFmt  = fmt;
    texFmt.imgType = type;
    texFmt.pType   = formatToImgComp(fmt);
    texFmt.levels  = 1;

    sref<Texture> tex = make_sref<GPUTexture>(resId, width, height, depth, sampler, texFmt);

    _textures.push_back({ id, target, intFormat, OGLTexPixelFormats[fmt], OGLTexPixelTypes[texFmt.pType], tex });

    return resId;
}

RRID RenderInterface::createCubemap(const Cubemap& cube, const TexSampler& sampler) {
    GLuint id = 0;
    GLenum target = OGLTexTargets[ImageType::IMGTYPE_CUBE];

    RRID resId = _textures.size();

    GLenum pType  = OGLTexPixelTypes[cube.compType()];
    GLenum oglFmt = OGLTexPixelFormats[cube.format()];

    glGenTextures(1, &id);
    glBindTexture(target, id);
    
    if (cube.numLevels() > 1) {
        glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(target, GL_TEXTURE_MAX_LEVEL,  4);
        //glGenerateMipmap(target);
    }
    
    for (uint32 side = 0; side < 6; side++) {
        const Image* img = cube.face((CubemapFace)side);

        // Upload all levels for the face
        for (uint32 lvl = 0; lvl < img->numLevels(); ++lvl) {
            uint32 w = mipDimension(img->width(),  lvl);
            uint32 h = mipDimension(img->height(), lvl);

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, lvl, oglFmt, w, h, 0, oglFmt, pType, img->data(lvl));
        }
    }

    glTexParameteri(target, GL_TEXTURE_WRAP_S,     OGLTexWrapping[sampler.sWrap()]);
    glTexParameteri(target, GL_TEXTURE_WRAP_T,     OGLTexWrapping[sampler.tWrap()]);
    glTexParameteri(target, GL_TEXTURE_WRAP_R,     OGLTexWrapping[sampler.rWrap()]);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, OGLTexFilters[sampler.minFilter()]);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, OGLTexFilters[sampler.magFilter()]);

    // Unbind texture
    glBindTexture(target, 0);

    TexFormat fmt;
    fmt.imgFmt  = cube.format();
    fmt.imgType = IMGTYPE_CUBE;
    fmt.levels  = cube.numLevels();
    fmt.pType   = cube.compType();

    sref<Texture> tex = make_sref<GPUTexture>(resId, cube.width(), cube.height(), 1, sampler, fmt);

    _textures.push_back({ id, target, oglFmt, oglFmt, pType, tex });

    return resId;
}

sref<Texture> RenderInterface::getTexture(RRID id) {
    if (id < 0 || id >= _textures.size())
        return nullptr; // Error

    RHITexture tex = _textures[id];
    if (tex.id == 0 || tex.tex == nullptr)
        return nullptr; // Error

    return tex.tex;
}

bool RenderInterface::readTexture(RRID id, Image& img) {
    if (id < 0 || id >= _textures.size())
        return false; // Error

    RHITexture tex = _textures[id];
    if (tex.id == 0)
        return false; // Error

    TexFormat fmt = tex.tex->format();
    if (fmt.imgType == IMGTYPE_CUBE)
        return false; // Error

    img.init(fmt.imgFmt, tex.tex->width(), tex.tex->height(), tex.tex->depth(), fmt.levels);

    glBindTexture(tex.target, tex.id);
    for (uint32 lvl = 0; lvl < fmt.levels; ++lvl)
        glGetTexImage(tex.target, 0, tex.format, tex.pType, img.data(lvl));
    glBindTexture(tex.target, 0);

    return true;
}

bool RenderInterface::readCubemap(RRID id, Cubemap& cube) {
    if (id < 0 || id >= _textures.size())
        return false; // Error

    RHITexture tex = _textures[id];
    if (tex.id == 0)
        return false; // Error

    TexFormat fmt = tex.tex->format();
    if (fmt.imgType != IMGTYPE_CUBE)
        return false; // Error

    cube.init(fmt.imgFmt, tex.tex->width(), tex.tex->height(), fmt.levels);

    glBindTexture(tex.target, tex.id);
    for (uint32 f = 0; f < 6; ++f)
        for (uint32 lvl = 0; lvl < fmt.levels; ++lvl)
            glGetTexImage(tex.target, 0, tex.format, tex.pType, cube.data((CubemapFace)f, lvl));
    glBindTexture(tex.target, 0);

    return true;
}

void RenderInterface::generateMipmaps(RRID id) {
    if (id < 0 || id >= _textures.size())
        return; // Error
    
    RHITexture ogltex = _textures[id];
    if (ogltex.id == 0)
        return; // Error

    glBindTexture(ogltex.target, ogltex.id);
    glGenerateMipmap(ogltex.target);
    glBindTexture(ogltex.target, 0);
}

void RenderInterface::setTextureData(RRID id, uint32 level, const void* pixels) {
    if (id < 0 || id >= _textures.size())
        return; // Error

    RHITexture ogltex = _textures[id];
    if (ogltex.id == 0)
        return; // Error

    glBindTexture(ogltex.target, ogltex.id);

    GLsizei width  = ogltex.tex->width();
    GLsizei height = ogltex.tex->height();
    GLsizei depth  = ogltex.tex->depth();

    const TexFormat& fmt = ogltex.tex->format();
    GLenum type = OGLTexPixelTypes[fmt.pType];
    if (fmt.imgType == IMGTYPE_2D)
        glTexImage2D(ogltex.target, level, ogltex.intFormat, width, height, 0, ogltex.format, type, pixels);
    else if (fmt.imgType == IMGTYPE_1D)
        glTexImage1D(ogltex.target, level, ogltex.intFormat, width, 0, ogltex.format, type, pixels);
    else if (fmt.imgType == IMGTYPE_3D)
        glTexImage3D(ogltex.target, level, ogltex.intFormat, width, height, depth, 0, ogltex.format, type, pixels);

    glBindTexture(ogltex.target, 0);
}

bool RenderInterface::deleteTexture(RRID id) {
    if (id < (int64)_textures.size() && id != -1) {
        GLuint oglId = _textures[id].id;
        if (oglId != 0) {
            glDeleteTextures(1, &oglId);
            _textures[id].id = 0;
            return true;
        }
    }

    return false;
}

void RenderInterface::bindTexture(RRID id) {
    if (id < 0 || id >= _textures.size())
        return; // Error

    RHITexture ogltex = _textures[id];
    if (ogltex.id == 0)
        return; // Error

    glBindTexture(ogltex.target, ogltex.id);
}

void RenderInterface::bindTexture(uint32 slot, RRID id) {
    glActiveTexture(GL_TEXTURE0 + slot);
    bindTexture(id);
}

sref<Image> RenderInterface::getImage(int32 x, int32 y, int32 w, int32 h) const {
    sref<Image> img = make_sref<Image>();
    img->init(IMGFMT_RGB8, w, h, 1, 1);
    glReadPixels(x, y, w, h, GL_RGB, GL_UNSIGNED_BYTE, img->data());
    return img;
}