#include "pch.hpp"
#include <zlib/zlib.h>
#include "WeaveUtils/Compression.hpp"
#include "WeaveUtils/Span.hpp"

using namespace Weave;

static constexpr uint s_ZLibChunkSize = 16384;

uint Weave::GetCRC32(string_view data)
{
	uLong crc = crc32(0L, nullptr, 0);
	crc = crc32(crc, reinterpret_cast<const byte*>(data.data()), (uint)data.size());
	return (uint)crc;
}

uint Weave::GetCRC32(const IDynamicArray<byte>& data)
{
    uLong crc = crc32(0L, nullptr, 0);
    crc = crc32(crc, data.GetData(), (uint)data.GetLength());
    return (uint)crc;
}

void Weave::CompressBytes(const IDynamicArray<byte>& input, ZLibArchive& output)
{
    if (input.IsEmpty())
    {
        output.data.Clear();
        return;
    }

    // Initialize zlib stream
    z_stream zlibStream;
    zlibStream.zalloc = Z_NULL;
    zlibStream.zfree = Z_NULL;
    zlibStream.opaque = Z_NULL;

    WV_CHECK_MSG(output.compressionLevel >= -1 && output.compressionLevel <= 9,
        "Invalid compression level: {}", output.compressionLevel);

    // Initialize deflate with default compression level
    WV_CHECK_MSG(deflateInit(&zlibStream, output.compressionLevel) == Z_OK,
        "Deflate initialization failed: {}", (zlibStream.msg != nullptr) ? zlibStream.msg : "unknown error");

    // Set input data
    zlibStream.avail_in = (uint)input.GetLength();
    zlibStream.next_in = const_cast<byte*>(input.GetData());

    // Prepare output buffer
    output.originalSizeBytes = (uint)input.GetLength();
    output.originalCRC32 = GetCRC32(input);

    output.data.Clear();
    output.data.Reserve(input.GetLength() + 1024);

    size_t outputPos = 0;
    int result;

    do
    {
        output.data.Resize(outputPos + s_ZLibChunkSize);
        zlibStream.next_out = &output.data[outputPos];
        zlibStream.avail_out = s_ZLibChunkSize;

        result = deflate(&zlibStream, Z_FINISH);

        if (result == Z_STREAM_ERROR)
        {
            deflateEnd(&zlibStream);
            WV_THROW("Deflate failed: {}", (zlibStream.msg != nullptr) ? zlibStream.msg : "unknown error");
        }

        outputPos += s_ZLibChunkSize - zlibStream.avail_out;

    } while (zlibStream.avail_out == 0);

    // Trim output to exact size
    output.data.Resize(outputPos);

    // Clean up
    deflateEnd(&zlibStream);

    WV_CHECK_MSG(result == Z_STREAM_END, "Deflate did not complete: {}",
        (zlibStream.msg != nullptr) ? zlibStream.msg : "unknown error");
}

void Weave::CompressBytes(string_view input, ZLibArchive& output)
{
    Span<byte> dataSpan(const_cast<byte*>(reinterpret_cast<const byte*>(input.data())), input.size());
    CompressBytes(dataSpan, output);
}

void Weave::DecompressBytes(const ZLibArchive& input, Vector<byte>& output)
{
    if (input.data.IsEmpty())
    {
        output.Clear();
        return;
    }

    // Initialize zlib stream
    z_stream zlibStream;
    zlibStream.zalloc = Z_NULL;
    zlibStream.zfree = Z_NULL;
    zlibStream.opaque = Z_NULL;
    zlibStream.avail_in = (uint)input.data.GetLength();
    zlibStream.next_in = const_cast<byte*>(input.data.GetData());

    // Initialize inflate
    WV_CHECK_MSG(inflateInit(&zlibStream) == Z_OK,
        "Decompression initialization failed: {}", (zlibStream.msg != nullptr) ? zlibStream.msg : "unknown error");

    // Prepare output buffer
    output.Clear();
    output.Reserve(input.originalSizeBytes);

    size_t outputPos = 0;
    int result;

    do
    {
        output.Resize(outputPos + s_ZLibChunkSize);
        zlibStream.next_out = &output[outputPos];
        zlibStream.avail_out = s_ZLibChunkSize;

        result = inflate(&zlibStream, Z_NO_FLUSH);

        if (result != Z_OK && result != Z_STREAM_END)
        {
            inflateEnd(&zlibStream);
            WV_THROW("Decompression failed: {}", (zlibStream.msg != nullptr) ? zlibStream.msg : "unknown error");
        }

        outputPos += s_ZLibChunkSize - zlibStream.avail_out;

    } while (zlibStream.avail_out == 0);

    // Trim output to exact size
    output.Resize(outputPos);
    // Clean up
    inflateEnd(&zlibStream);

    WV_CHECK_MSG(result == Z_STREAM_END, "Decompression did not complete: {}",
        (zlibStream.msg != nullptr) ? zlibStream.msg : "unknown error");

    // Calculate checksum on decompressed archive and validate
    const uint crc = GetCRC32(output);
    WV_CHECK_MSG(crc == input.originalCRC32, "CRC32 mismatch: expected {:08x}, got {:08x}",
        input.originalCRC32, crc);
}

string_view Weave::ZLibArchive::GetDataAsString() const
{
    return string_view(reinterpret_cast<const char*>(data.GetData()), data.GetLength());
}

float Weave::ZLibArchive::GetCompressionRatio() const
{
    return (data.GetLength() / (float)originalSizeBytes);
}
