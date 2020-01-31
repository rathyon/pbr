#include <Texture.h>

using namespace pbr;

TexSampler::TexSampler() : _wrapS(WRAP_REPEAT), _wrapT(WRAP_REPEAT), _wrapR(WRAP_REPEAT), 
                           _min(FILTER_NEAREST), _mag(FILTER_NEAREST), _numSamples(1) { }

TexSampler::TexSampler(TexWrapMode sWrap, TexWrapMode tWrap, 
                       TexFilterMode minFilter, TexFilterMode magFilter)
    : _wrapS(sWrap), _wrapT(tWrap), _wrapR(WRAP_REPEAT), _min(minFilter), _mag(magFilter), _numSamples(1) { }

TexSampler::TexSampler(TexWrapMode sWrap, TexWrapMode tWrap, TexWrapMode rWrap, 
                       TexFilterMode minFilter, TexFilterMode magFilter)
    : _wrapS(sWrap), _wrapT(tWrap), _wrapR(rWrap), _min(minFilter), _mag(magFilter), _numSamples(1) { }

TexWrapMode TexSampler::sWrap() const {
    return _wrapS;
}

TexWrapMode TexSampler::tWrap() const {
    return _wrapT;
}

TexWrapMode TexSampler::rWrap() const {
    return _wrapR;
}

TexFilterMode TexSampler::minFilter() const {
    return _min;
}

TexFilterMode TexSampler::magFilter() const {
    return _mag;
}

void TexSampler::setWrapMode(TexWrapMode sWrap, TexWrapMode tWrap) {
    _wrapS = sWrap;
    _wrapT = tWrap;
}

void TexSampler::setWrapMode(TexWrapMode sWrap, TexWrapMode tWrap, TexWrapMode rWrap) {
    setWrapMode(sWrap, tWrap);
    _wrapR = rWrap;
}

void TexSampler::setFilterMode(TexFilterMode min, TexFilterMode mag) {
    _min = min;
    _mag = mag;
}

void TexSampler::setNumSamples(uint32 numSamples) {
    _numSamples = numSamples;
}

uint32 TexSampler::numSamples() const {
    return _numSamples;
}

Texture::Texture() : _width(1), _height(1), _depth(1), _sampler() { }
Texture::Texture(int32 w, int32 h, int32 d, const TexSampler& sampler, const TexFormat& fmt)
    : _width(w), _height(h), _depth(d), _sampler(sampler), _format(fmt) { }

int32 Texture::width() const {
    return _width;
}

int32 Texture::height() const {
    return _height;
}

int32 Texture::depth() const {
    return _depth;
}

const TexSampler& Texture::sampler() const {
    return _sampler;
}

const TexFormat& Texture::format() const {
    return _format;
}

void Texture::setFormat(const TexFormat& format) {
    _format = format;
}