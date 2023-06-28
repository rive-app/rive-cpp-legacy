#ifdef RIVE_RENDERER_TESS
#include "viewer/tess/bitmap_decoder.hpp"
#include <cstring>

Bitmap::Bitmap(uint32_t width,
               uint32_t height,
               PixelFormat pixelFormat,
               std::unique_ptr<const uint8_t[]> bytes) :
    m_Width(width), m_Height(height), m_PixelFormat(pixelFormat), m_Bytes(std::move(bytes))
{}

Bitmap::Bitmap(uint32_t width, uint32_t height, PixelFormat pixelFormat, const uint8_t* bytes) :
    Bitmap(width, height, pixelFormat, std::unique_ptr<const uint8_t[]>(bytes))
{}

size_t Bitmap::bytesPerPixel(PixelFormat format) const
{
    switch (format)
    {
        case PixelFormat::R:
            return 1;
        case PixelFormat::RGB:
            return 3;
        case PixelFormat::RGBA:
            return 4;
    }
}

size_t Bitmap::byteSize(PixelFormat format) const
{
    return m_Width * m_Height * bytesPerPixel(format);
}

size_t Bitmap::byteSize() const { return byteSize(m_PixelFormat); }

std::unique_ptr<Bitmap> DecodePng(rive::Span<const uint8_t> bytes);
std::unique_ptr<Bitmap> DecodeJpeg(rive::Span<const uint8_t> bytes) { return nullptr; }
std::unique_ptr<Bitmap> DecodeWebP(rive::Span<const uint8_t> bytes) { return nullptr; }

using BitmapDecoder = std::unique_ptr<Bitmap> (*)(rive::Span<const uint8_t> bytes);
struct ImageFormat
{
    const char* name;
    uint8_t fingerprintSize;
    uint8_t fingerprint[4];
    BitmapDecoder decodeImage;
};

std::unique_ptr<Bitmap> Bitmap::decode(rive::Span<const uint8_t> bytes)
{
    static ImageFormat decoders[] = {
        {
            "png", 4,
            {0x89, 0x50, 0x4E, 0x47},
            DecodePng,
        },
        {
            "jpeg", 3,
            {0xFF, 0xD8, 0xFF},
            DecodeJpeg,
        },
        {
            "webp", 3,
            {0x52, 0x49, 0x46},
            DecodeWebP,
        },
    };

    for (auto recognizer : decoders)
    {
        auto& fingerprint = recognizer.fingerprint;

        // Immediately discard decoders with fingerprints that are longer than
        // the file buffer.
        if (recognizer.fingerprintSize > bytes.size())
        {
            continue;
        }

        // If the fingerprint doesn't match, discrd this decoder. These are all bytes so .size() is
        // fine here.
        if (std::memcmp(recognizer.fingerprint, bytes.data(), recognizer.fingerprintSize) != 0)
        {
            continue;
        }

        auto bitmap = recognizer.decodeImage(bytes);
        if (!bitmap)
        {
            fprintf(stderr, "Bitmap::decode - failed to decode a %s.\n", recognizer.name);
        }
        return bitmap;
    }
    return nullptr;
}

void Bitmap::pixelFormat(PixelFormat format)
{
    if (format == m_PixelFormat)
    {
        return;
    }
    auto nextByteSize = byteSize(format);
    auto nextBytes = std::unique_ptr<uint8_t[]>(new uint8_t[nextByteSize]);

    auto fromBytesPerPixel = bytesPerPixel(m_PixelFormat);
    auto toBytesPerPixel = bytesPerPixel(format);
    int writeIndex = 0;
    int readIndex = 0;
    for (int i = 0; i < m_Width * m_Height; i++)
    {
        for (int j = 0; j < toBytesPerPixel; j++)
        {
            nextBytes[writeIndex++] = j < fromBytesPerPixel ? m_Bytes[readIndex++] : 255;
        }
    }

    m_Bytes = std::move(nextBytes);
    m_PixelFormat = format;
}
#endif
