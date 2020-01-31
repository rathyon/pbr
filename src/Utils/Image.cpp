#include <Image.h>

#include <sstream>
#include <fstream>
#include <path.h>

#include <lodepng.h>

#include <zlib.h>

/*
#include <ImfRgba.h>
#include <ImfRgbaFile.h>
#include <hdrloader.h>

using namespace Imf;
using namespace Imath;
*/

using namespace filesystem;
using namespace pbr;

uint32 pbr::formatToNumChannels(ImageFormat format) {
    static const uint32 channels[] = {
        0,
        1, 2, 3, 4,       //  8-bit unsigned
        1, 2, 3, 4,       // 16-bit unsigned
        1, 2, 3, 4,       //  8-bit signed
        1, 2, 3, 4,       // 16-bit signed
        1, 2, 3, 4,       // 16-bit float
        1, 2, 3, 4,       // 32-bit float
        1, 2, 3, 4,       // 16-bit unsigned integer
        1, 2, 3, 4,       // 32-bit unsigned integer
        1, 2, 3, 4,       // 16-bit signed integer
        1, 2, 3, 4,       // 32-bit signed integer
        0, 0, 0, 0, 0, 0, // Packed
        1, 1, 2, 1,       // Depth
        0, 0, 0, 0, 0     // Compressed
    };

    return channels[format];
}

uint32 pbr::formatToBytesPerChannel(ImageFormat format) {
    static const uint32 bpc[] = {
        1, //  8-bit unsigned
        2, // 16-bit unsigned
        1, //  8-bit signed
        2, // 16-bit signed
        2, // 16-bit float
        4, // 32-bit float
        2, // 16-bit unsigned integer
        4, // 32-bit unsigned integer
        2, // 16-bit signed integer
        4  // 32-bit signed integer
    };

    return bpc[(format - 1) >> 2];
}

ImageComponent pbr::formatToImgComp(ImageFormat format) {
    static const ImageComponent cmp[] = {
        UNKNOWN,
        UBYTE, UBYTE, UBYTE, UBYTE,       //  8-bit unsigned
        USHORT, USHORT, USHORT, USHORT,   // 16-bit unsigned
        BYTE, BYTE, BYTE, BYTE,           //  8-bit signed
        SHORT, SHORT, SHORT, SHORT,       // 16-bit signed
        HALF, HALF, HALF, HALF,           // 16-bit float
        FLOAT, FLOAT, FLOAT, FLOAT,       // 32-bit float
        USHORT, USHORT, USHORT, USHORT,   // 16-bit unsigned integer
        UINT, UINT, UINT, UINT,           // 32-bit unsigned integer
        SHORT, SHORT, SHORT, SHORT,       // 16-bit signed integer
        INT, INT, INT, INT,               // 32-bit signed integer
        UNKNOWN, UNKNOWN, UNKNOWN,        // Packed
        UNKNOWN, UNKNOWN, UNKNOWN,
        FLOAT, FLOAT, FLOAT, FLOAT,       // Depth
        UNKNOWN, UNKNOWN, UNKNOWN,        // Compressed
        UNKNOWN, UNKNOWN
    };

    return cmp[format];
}

uint32 pbr::mipDimension(uint32 baseDim, uint32 level) {
    uint32 dim = baseDim >> level;
    return (dim == 0) ? 1 : dim;
}

Image::Image() {
    _format = IMGFMT_UNKNOWN;
    _width  = 0;
    _height = 0;
    _depth  = 0;
    _numLevels = 0;

    _data = nullptr;
}

Image::~Image() {
    _format = IMGFMT_UNKNOWN;   
    _width  = 0;
    _height = 0;
    _depth  = 0;
    _numLevels = 0;

    _data.reset();
}

ImageFormat toFormat(uint32 numChannels, uint32 bytesPerChannel) {
    return IMGFMT_UNKNOWN;
}

void Image::init(ImageFormat format, uint32 width, uint32 height, uint32 depth, uint32 levels) {
    _format    = format;
    _width     = width;
    _height    = height;
    _depth     = depth;
    _numLevels = levels;

    _data = std::make_unique<uint8[]>(totalSize());
}

uint32 Image::numLevels() const {
    return _numLevels;
}

bool Image::hasMipMap() const {
    return _numLevels > 1;
}

ImageComponent Image::compType() const {
    return formatToImgComp(_format);
}

uint32 Image::numChannels() const {
    return formatToNumChannels(_format);
}

uint32 Image::size(uint32 level) const {
    uint32 w = mipDimension(_width,  level);
    uint32 h = mipDimension(_height, level);
    uint32 d = mipDimension(_depth,  level);

    uint32 nChannels    = numChannels();
    uint32 bytesChannel = formatToBytesPerChannel(_format);

    return nChannels * bytesChannel * w * h * d;
}

uint32 Image::totalSize() const {
    uint32 sizeBytes = 0;
    for (uint32 i = 0; i < _numLevels; ++i)
        sizeBytes += size(i);

    return sizeBytes;
}

bool Image::loadPNG(const std::string& filePath) {
    static const uint32 toChannels[] = { 1, 0, 3, 0, 2, 0, 4 };

    std::vector<unsigned char> png;
    std::vector<unsigned char> image;

    lodepng::State state;
    unsigned width, height;
    unsigned error = lodepng::load_file(png, filePath);
    if (!error)
        error = lodepng::decode(image, width, height, state, png);

    if (error) {
        // std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
        return false;
    }

    _width  = width;
    _height = height;
    _depth  = 1;

    _numLevels = 1;

    uint32 bitDepth  = state.info_png.color.bitdepth;
    uint32 nChannels = toChannels[state.info_png.color.colortype];

    image.clear();
    state.info_raw.bitdepth = state.info_png.color.bitdepth;
    state.info_raw.colortype = state.info_png.color.colortype;
    error = lodepng::decode(image, width, height, state, png);

    if (error) {
        // std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
        return false;
    }

    _format = IMGFMT_UNKNOWN;
    switch (nChannels) {
        case 1:
            _format = (bitDepth > 8) ? IMGFMT_R16 : IMGFMT_R8;
            break;
        case 2:
            _format = (bitDepth > 8) ? IMGFMT_RG16 : IMGFMT_RG8;
            break;
        case 3:
            _format = (bitDepth > 8) ? IMGFMT_RGB16 : IMGFMT_RGB8;
            break;
        case 4:
            _format = (bitDepth > 8) ? IMGFMT_RGBA16 : IMGFMT_RGBA8;
            break;
    }

    uint32 size = totalSize();
    _data = std::make_unique<uint8[]>(size);

    memcpy(_data.get(), &image[0], size);

    return true;
}

bool Image::savePNG(const std::string& filePath, uint32 lvl) const {
    static const LodePNGColorType typeTbl[] = { LCT_GREY, LCT_GREY_ALPHA, LCT_RGB, LCT_RGBA };

    // Only process integer images
    if (_format >= IMGFMT_R16F && _format <= IMGFMT_RGBA32F)
        return false;

    std::vector<unsigned char> image; // the raw pixels

    lodepng::State state; // optionally customize this one

    uint32 nChannels   = numChannels();
    uint32 bitsChannel = formatToBytesPerChannel(_format) * 8;

    if (nChannels == 0 || bitsChannel == 0)
        return false;

    state.info_raw.colortype = typeTbl[nChannels - 1];
    state.info_raw.bitdepth  = bitsChannel;

    int32 w = mipDimension(_width,  lvl);
    int32 h = mipDimension(_height, lvl);
    unsigned error = lodepng::encode(image, data(lvl), w, h, state);
    if (!error)
        error = lodepng::save_file(image, filePath);

    if (error) {
        // std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
        return false;
    }

    return true;
}

bool Image::loadHDR(const std::string& filePath) {
    /*HDRLoaderResult result;

    if (!HDRLoader::load(filePath.c_str(), result)) {
        std::cerr << "Could not load HDR image '" << filePath << "'.";
        return nullptr;
    }

    init(IMGFMT_RGB32F, result.width, result.height, 1, 1);

    return loadImage((const uint8*)result.cols, 0);*/

    return false;
}

bool Image::loadEXR(const std::string& filePath) {
    return false;
}

bool Image::saveEXR(const std::string& filePath, uint32 lvl) const {
    // Must be a float format
    /*if (_format < IMGFMT_R16F && _format > IMGFMT_RGBA32F)
        return false;

    const float* pixels = (const float*)data(lvl);

    uint32 nPixels = _width * _height;
    uint32 nChannels = numChannels();

    Rgba *hrgba = new Rgba[nPixels];
    for (int i = 0; i < nPixels; ++i) {
        float rgb[3] = { 0, 0, 0 };
        for (uint32 c = 0; c < nChannels; ++c)
            rgb[c] = pixels[nChannels * i + c];

        hrgba[i] = Rgba(rgb[0], rgb[1], rgb[2]);
    }

    // OpenEXR uses inclusive pixel bounds.
    Box2i displayWindow(V2i(0, 0), V2i(_width - 1, _height - 1));
    Box2i dataWindow(V2i(0, 0), V2i(0 + _width - 1, 0 + _height - 1));

    try {
        RgbaOutputFile file(filePath.c_str(), displayWindow, dataWindow,
                            WRITE_RGB);
        file.setFrameBuffer(hrgba - 0 - 0 * _width, 1, _width);
        file.writePixels(_height);
    } catch (const std::exception &exc) {
        return false;
    }

    delete[] hrgba;

    return true;*/

    return false;
}

bool Image::loadIMG(const std::string& filePath) {
    IMGHeader header;

    std::ifstream file(filePath, std::ios::in | std::ios::binary);

    // Read header
    file.read((char*)&header, sizeof(IMGHeader));

    if (!(header.id[0] == 'I' && header.id[1] == 'M' &&
          header.id[2] == 'G' && header.id[3] == ' ')) {
        file.close();
        return false;
    }

    init((ImageFormat)header.fmt, header.width, header.height, header.depth, header.levels);

    uint8* imgData = new uint8[header.totalSize];

    uint8* ptr = imgData;

    for (uint32 lvl = 0; lvl < header.levels; ++lvl) {
        uint32 sz = size(lvl);
        file.read((char*)ptr, sz);
        loadImage(ptr, lvl);
        ptr += sz;
    }

    file.close();

    delete[] imgData;

    if (totalSize() != header.totalSize)
        return false; // Warn

    return true;
}

bool Image::saveIMG(const std::string& filePath) const {
    IMGHeader header;
    header.id[0] = 'I';
    header.id[1] = 'M';
    header.id[2] = 'G';
    header.id[3] = ' ';
    header.fmt    = format();
    header.width  = width();
    header.height = height();
    header.depth  = depth();
    header.levels = numLevels();
    header.totalSize = totalSize();
    header.compSize  = totalSize();

    std::ofstream file(filePath, std::ios::out | std::ios::binary);

    file.write((const char*)&header, sizeof(IMGHeader));

    uint32 numlvls = numLevels();
    for (uint32 lvl = 0; lvl < numlvls; ++lvl)
        file.write((const char*)data(lvl), size(lvl));

    file.close();

    return true;
}

bool Image::loadImage(const std::string& filePath) {
    filesystem::path path(filePath);
    if (!path.exists()) {
        // Log
        return false;
    }

    std::string ext = path.extension();
    if (ext == "png")
        return loadPNG(filePath);
    else if (ext == "exr")
        return loadEXR(filePath);
    else if (ext == "img")
        return loadIMG(filePath);
    else if (ext == "hdr")
        return loadHDR(filePath);

    return false;
}

bool Image::loadImage(ImageFormat format, uint32 width, uint32 height, uint32 depth, const uint8* data, uint32 numLevels) {
    _format = format;
    _width  = width;
    _height = height;
    _depth  = depth;

    _numLevels = numLevels;

    uint32 size = totalSize();

    _data.reset();
    _data = std::make_unique<uint8[]>(size);

    memcpy(_data.get(), data, size);

    return true;
}

bool Image::loadImage(const uint8* data, uint32 lvl) {
    if (_format == IMGFMT_UNKNOWN || _data == nullptr || 
        _width == 0 || lvl >= _numLevels)
        return false;

    uint32 offset = 0;
    for (uint32 l = 0; l < lvl; ++l)
        offset += size(l);

    uint32 sizeLvl = size(lvl);
    uint8* start = (uint8*)_data.get() + offset;
    memcpy((void*)start, data, sizeLvl);

    return true;
}

bool Image::saveImage(const std::string& filePath, uint32 lvl) const {
    if (_data == nullptr || _width == 0 || lvl >= _numLevels)
        return false;

    filesystem::path path(filePath);
    std::string ext = path.extension();

    if (ext == "img")
        return saveIMG(filePath);

    ImageType imgType = type();
    if (imgType == IMGTYPE_2D) {
        if (ext == "png")
            return savePNG(filePath, lvl);
        else if (ext == "exr")
            return saveEXR(filePath, lvl);
    }

    return false;
}

bool Image::saveMipMap(const std::string& filePath) const {
    if (!hasMipMap())
        return false;

    filesystem::path path(filePath);
    std::string ext      = path.extension();
    std::string parent   = path.parent_path().str();
    std::string fileName = path.filenameNoExt();

    if (!parent.empty())
        fileName = "/" + fileName;

    if (ext != "png" && ext != "exr" && ext != "img")
        return false;

    // Save level 0
    saveImage(filePath);

    // Save other mip map levels
    for (uint32 lvl = 1; lvl < _numLevels; ++lvl) {
        std::ostringstream ss;
        ss << parent << fileName << "-" << lvl << "." << ext;
        saveImage(ss.str(), lvl);
        ss.clear();
    }

    return true;
}

bool Image::flipX() {
    int32 w, h;
    uint32 nChan     = numChannels();
    uint32 bytesChan = formatToBytesPerChannel(_format);

    std::unique_ptr<uint8[]> newImg = std::make_unique<uint8[]>(totalSize());

    // Flip each mip map level
    uint32 prevSize = 0;
    uint32 offset = bytesChan * nChan;
    for (uint32 lvl = 0; lvl < _numLevels; ++lvl) {
        w = mipDimension(_width, lvl);
        h = mipDimension(_height, lvl);

        uint8* dst = (uint8*)newImg.get() + prevSize;
        uint8* src = nullptr;

        for (int32 y = 0; y < h; y++) {
            src = ((uint8*)_data.get() + prevSize) + (y + 1) * w * offset;
            for (int32 x = 0; x < w; x++) {
                memcpy(dst, src, offset);

                dst += offset;
                src -= offset;
            }
        }

        prevSize += size(lvl);
    }

    _data.swap(newImg);

    return true;
}

bool Image::flipY() {
    int32 w, h;
    uint32 nChannels    = numChannels();
    uint32 bytesChannel = formatToBytesPerChannel(_format);

    std::unique_ptr<uint8[]> newImg = std::make_unique<uint8[]>(totalSize());

    // Flip each mip map level
    uint32 prevSize = 0;
    for (uint32 lvl = 0; lvl < _numLevels; ++lvl) {
        w = mipDimension(_width,  lvl);
        h = mipDimension(_height, lvl);

        uint32 lineWidth = w * nChannels * bytesChannel;

        uint8* dst = newImg.get() + prevSize;
        uint8* src = (_data.get() + prevSize) + (h - 1) * lineWidth;

        for (int32 y = 0; y < h; y++) {
            memcpy(dst, src, lineWidth);

            dst += lineWidth;
            src -= lineWidth;
        }

        prevSize += size(lvl);
    }

    _data.swap(newImg);

    return true;
}

bool Image::toGrayscale() {
    // Only process 2D images
    if (type() >= IMGTYPE_3D)
        return false;

    uint32 nChan     = numChannels();
    uint32 bytesChan = formatToBytesPerChannel(_format);
    uint32 graySize  = bytesChan * _width * _height;
    for (uint32 l = 1; l < _numLevels; l++)
        graySize += bytesChan * mipDimension(_width, l) * mipDimension(_height, l);

    // Only process integer images
    if ((_format >= IMGFMT_R16F && _format <= IMGFMT_RGBA32F) 
        || nChan < 3 || bytesChan > 2)
        return false;

    if (bytesChan == 2)
        graySize *= 2;

    std::unique_ptr<uint8[]> newImg = std::make_unique<uint8[]>(graySize);

    int32 w, h;
    uint32 prevSize = 0;
    uint32 prevSizeGray = 0;
    for (uint32 lvl = 0; lvl < _numLevels; ++lvl) {
        w = mipDimension(_width,  lvl);
        h = mipDimension(_height, lvl);

        uint32 nPixels = w * h;
        if (_format <= IMGFMT_RGBA8) {
            uint8* src  = _data.get()  + prevSize;
            uint8* dest = newImg.get() + prevSizeGray;

            do {
                *dest++ = (77 * src[0] + 151 * src[1] + 28 * src[2] + 128) >> 8;
                src += nChan;
            } while (--nPixels);
        } else {
            uint16* src  = (uint16*)_data.get()  + (prevSize / 2);
            uint16* dest = (uint16*)newImg.get() + prevSizeGray;

            do {
                *dest++ = (77 * src[0] + 151 * src[1] + 28 * src[2] + 128) >> 8;
                src += nChan;
            } while (--nPixels);
        }

        prevSizeGray += w * h;
        prevSize     += size(lvl);
    }

    if (_format <= IMGFMT_RGBA8)
        _format = IMGFMT_R8;
    else 
        _format = IMGFMT_R16;

    _data.swap(newImg);

    return true;
}

bool Image::toneMap(float exp) {
    uint32 nChan     = numChannels();
    uint32 bytesChan = formatToBytesPerChannel(_format);

    uint32 mappedSize = 3 * _width * _height;
    for (uint32 l = 1; l < _numLevels; l++)
        mappedSize += 3 * mipDimension(_width, l) * mipDimension(_height, l);

    // Only process 32-bit float images
    if ((_format < IMGFMT_R32F && _format > IMGFMT_RGBA32F)
        || nChan < 3)
        return false;

    std::unique_ptr<uint8[]> newImg = std::make_unique<uint8[]>(mappedSize);

    int32 w, h;
    uint32 prevSize = 0;
    uint32 prevSizeMapped = 0;
    for (uint32 lvl = 0; lvl < _numLevels; ++lvl) {
        w = mipDimension(_width, lvl);
        h = mipDimension(_height, lvl);

        uint32 nPixels = w * h;

        uint8* src = _data.get()  + prevSize;
        uint8* dst = newImg.get() + prevSizeMapped;

        float* srcf = (float*)src;
        do {
            float resR = exp * srcf[0] / (exp * srcf[0] + 1.0f);
            *dst++ = math::clamp<uint8>(uint8(255.0f * resR), 0u, 255u);

            float resG = exp * srcf[1] / (exp * srcf[1] + 1.0f);
            *dst++ = math::clamp<uint8>(uint8(255.0f * resG), 0u, 255u);

            float resB = exp * srcf[2] / (exp * srcf[2] + 1.0f);
            *dst++ = math::clamp<uint8>(uint8(255.0f * resB), 0u, 255u);

            srcf += nChan;
        } while (--nPixels);

        prevSizeMapped += 3 * w * h;
        prevSize       += size(lvl);
    }

    _format = IMGFMT_RGB8;

    _data.swap(newImg);

    return true;
}

ImageFormat Image::format() const {
    return _format;
}

int32 Image::width() const {
    return _width;
}

int32 Image::height() const {
    return _height;
}

int32 Image::depth() const {
    return _depth;
}

uint8* Image::data(uint32 lvl) const {
    if (_data == nullptr || lvl >= _numLevels)
        return nullptr;

    // Compute offset to image level
    uint32 offset = 0;
    for (uint32 l = 0; l < lvl; ++l)
        offset += size(l);

    uint8* start = _data.get() + offset;

    return start;
}

ImageType Image::type() const {
    if (_depth  > 1) return IMGTYPE_3D;
    //if (_depth == 0) return IMGTYPE_CUBE;
    if (_height > 1) return IMGTYPE_2D;
    if (_width  > 1) return IMGTYPE_1D;

    return IMGTYPE_UNKNOWN;
}


Cubemap::Cubemap() { }

void Cubemap::init(ImageFormat format, uint32 width, uint32 height, uint32 numLevels) {
    if (width != 0 && height != 0) {
        for (uint32 i = 0; i < 6; i++)
            _faces[i].init(format, width, height, 1, numLevels);
    }
}

bool Cubemap::loadCubemap(const std::string& filePath) {
    filesystem::path path(filePath);
    if (!path.exists()) {
        // Log
        return false;
    }

    std::string ext = path.extension();
    if (ext == "cube")
        return loadCUBE(filePath);

    return false;
}

bool Cubemap::loadCubemap(const std::string paths[6]) {
    for (uint32 i = 0; i < 6; ++i)
        if (!_faces[i].loadImage(paths[i]))
            return false;

    return true;
}

bool Cubemap::loadCubemap(ImageFormat format, uint32 width, uint32 height, const uint8* data, uint32 numLevels) {
    if (data == nullptr || width == 0 || 
        height == 0 || numLevels == 0)
        return false;

    for (uint32 i = 0; i < 6; ++i)
        if (!_faces[i].loadImage(format, width, height, 1, data, numLevels))
            return false;

    return true;
}

bool Cubemap::loadFace(CubemapFace face, const uint8* data, uint32 lvl) {
    if (data == nullptr || width() == 0 ||
        height() == 0 || numLevels() == 0)
        return false;

    return _faces[face].loadImage(data, lvl);
}

bool Cubemap::saveCubemap(const std::string& filePath) {
    static const std::string faceNames[6] = { "xpos", "xneg", "ypos", "yneg", "zpos", "zneg" };

    filesystem::path path(filePath);
    std::string ext      = path.extension();
    std::string parent   = path.parent_path().str();
    std::string fileName = path.filenameNoExt();

    if (!parent.empty())
        fileName = "/" + fileName;

    if (ext == "cube")
        return saveCUBE(filePath);

    for (uint32 f = 0; f < 6; ++f) {
        std::ostringstream ss;
        ss << parent << fileName << "-" << faceNames[f] << "." << ext;
        if (!_faces[f].saveImage(ss.str()))
            return false;
        ss.clear();
    }

    return true;
}

uint8* Cubemap::data(CubemapFace face, uint32 lvl) const {
    return _faces[face].data(lvl);
}

ImageType Cubemap::type() const {
    return IMGTYPE_CUBE;
}

ImageFormat Cubemap::format() const {
    return _faces[0].format();
}

ImageComponent Cubemap::compType() const {
    return _faces[0].compType();
}

uint32 Cubemap::numChannels() const {
    return _faces[0].numChannels();
}

int32 Cubemap::width() const {
    return _faces[0].width();
}

int32 Cubemap::height() const {
    return _faces[0].height();
}

const Image* Cubemap::faces() const {
    return &_faces[0];
}

const Image* Cubemap::face(CubemapFace face) const {
    return &_faces[face];
}

Image* Cubemap::face(CubemapFace face) {
    return &_faces[face];
}

bool Cubemap::loadCUBE(const std::string& filePath) {
    CUBEHeader header;

    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    
    // Read header
    file.read((char*)&header, sizeof(CUBEHeader));

    // Validate file signature
    if (!(header.id[0] == 'C' && header.id[1] == 'U' &&
          header.id[2] == 'B' && header.id[3] == 'E')) {
        file.close();
        return false;
    }

    uint8* imgData = new uint8[header.totalSize];

    // Check if file is compressed
    if (header.totalSize != header.compSize) {
        uint8* srcBuffer = new uint8[header.compSize];
        file.read((char*)srcBuffer, header.compSize);
        file.close();

        uLong dstLen = header.totalSize;
        uLong srcLen = header.compSize;
        int result = uncompress(imgData, &dstLen, srcBuffer, srcLen);

        if (result != Z_OK) {
            delete[] srcBuffer;
            delete[] imgData;
            return false;
        }

        delete[] srcBuffer;
    } else {
        // Else read it directly from the file
        file.read((char*)imgData, header.totalSize);
        file.close();
    }

    // Initialize the image
    init((ImageFormat)header.fmt, header.width, header.height, header.levels);

    uint8* ptr = imgData;
    for (uint32 f = 0; f < 6; ++f) {
        for (uint32 lvl = 0; lvl < header.levels; ++lvl) {            
            uint32 size = _faces[f].size(lvl);
            //file.read((char*)ptr, size);
            _faces[f].loadImage(ptr, lvl);
            ptr += size;
        }
    }

    //file.close();

    delete[] imgData;

    if (totalSize() != header.totalSize)
        return false; // Warn

    return true;
}

bool Cubemap::saveCUBE(const std::string& filePath) const {
    CUBEHeader header;
    header.id[0] = 'C';
    header.id[1] = 'U';
    header.id[2] = 'B';
    header.id[3] = 'E';
    header.fmt    = format();
    header.width  = width();
    header.height = height();
    header.levels = numLevels();
    header.totalSize = totalSize();
    header.compSize  = header.totalSize;

    // Copy image data into contiguous buffer
    uint8* imgData = new uint8[header.totalSize];
    uint8* auxPtr  = imgData;
    for (uint32 f = 0; f < 6; ++f) {
        for (uint32 lvl = 0; lvl < header.levels; ++lvl) {
            size_t size = _faces[f].size(lvl);
            memcpy(auxPtr, (const char*)_faces[f].data(lvl), size);
            auxPtr += size;
        }
    }

    // Compress buffer with zlib
    uLong  dstLen    = compressBound(header.totalSize);
    uint8* dstBuffer = new uint8[dstLen];
    int result = compress(dstBuffer, &dstLen, imgData, header.totalSize);

    if (result != Z_OK) {
        delete[] dstBuffer;
        delete[] imgData;
        return false;
    }

    // Store the compressed size
    header.compSize = dstLen;

    // Write to file
    std::ofstream file(filePath, std::ios::out | std::ios::binary);
    file.write((const char*)&header, sizeof(CUBEHeader));
    file.write((const char*)dstBuffer, dstLen);
    file.close();

    // Release memory
    delete[] imgData;
    delete[] dstBuffer;

    return true;
}

uint32 Cubemap::size(CubemapFace face, uint32 lvl) const {
    return _faces[face].size(lvl);
}

uint32 Cubemap::totalSize() const {
    uint32 size = 0;
    for (uint32 f = 0; f < 6; ++f)
        size += _faces[f].totalSize();
    return size;
}


bool Cubemap::hasMipMap() const {
    return _faces[0].hasMipMap();
}

uint32 Cubemap::numLevels() const {
    return _faces[0].numLevels();
}