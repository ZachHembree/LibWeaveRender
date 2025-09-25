#pragma once
#include "WeaveUtils/Int.hpp"
#include "WeaveUtils/TextUtils.hpp"
#include "WeaveUtils/DynamicCollections.hpp"
#include "WeaveUtils/Serialization.hpp"

namespace Weave
{
    /// <summary>
    /// Serializable struct containing a compressed zlib deflate archive
    /// </summary>
    struct ZLibArchive
    {
        /// <summary>
        /// Compression level used by ZLib. -1 == default. 9 == smallest. 1 == fastest.
        /// Must be specified before compressing.
        /// </summary>
        byte compressionLevel;

        /// <summary>
        /// Checksum computed against original bytestream for validation
        /// </summary>
        uint originalCRC32;

        /// <summary>
        /// Bytestream size before compression
        /// </summary>
        uint originalSizeBytes;

        /// <summary>
        /// Compressed bytestream
        /// </summary>
        Vector<byte> data;

        /// <summary>
        /// Returns compressed bytestream as a string_view
        /// </summary>
        string_view GetDataAsString() const;

        float GetCompressionRatio() const;
    };

    template <class Archive>
    inline void serialize(Archive& ar, ZLibArchive& def)
    {
        ar(def.compressionLevel, def.originalCRC32, def.originalSizeBytes, def.data);
    }

    /// <summary>
    /// Computes a CRC32 for a narrow string
    /// </summary>
    uint GetCRC32(string_view data);

    /// <summary>
    /// Computes a CRC32 for a byte (unsigned char) array
    /// </summary>
    uint GetCRC32(const IDynamicArray<byte>& data);

    /// <summary>
    /// Compresses the given input byte array into a deflate archive with the compression level specified in ZLibArchive
    /// </summary>
    void CompressBytes(const IDynamicArray<byte>& input, ZLibArchive& output);

    /// <summary>
    /// Compresses the given input string into a deflate archive with the compression level specified in ZLibArchive
    /// </summary>
    void CompressBytes(string_view input, ZLibArchive& output);

    /// <summary>
    /// Decompresses the given deflate archive into the given byte array
    /// </summary>
    void DecompressBytes(const ZLibArchive& input, Vector<byte>& output);
}
