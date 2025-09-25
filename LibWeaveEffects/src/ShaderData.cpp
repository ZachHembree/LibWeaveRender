#include "pch.hpp"
#include "WeaveUtils/Compression.hpp"
#include "WeaveEffects/ShaderData.hpp"
#include "WeaveEffects/ShaderDataSerialization.hpp"

using namespace Weave;
using namespace Weave::Effects;

ShaderLibDef Weave::Effects::GetDeserializedLibDef(string_view libData)
{
	static thread_local std::stringstream bufStream;
	static thread_local ZLibArchive zipBuffer;
	static thread_local Vector<byte> libBuffer;

	// Deserialize zip wrapper
	{
		bufStream.clear();
		bufStream.str({});
		bufStream << libData;

		Deserializer zipReader(bufStream);
		zipReader(zipBuffer);
	}

	// Unzip shader data
	DecompressBytes(zipBuffer, libBuffer);

	// Copy unzipped data to ss
	bufStream.clear();
	bufStream.str({});
	bufStream.write(reinterpret_cast<char*>(libBuffer.GetData()), libBuffer.GetLength());

	// Deserialize decompressed data
	ShaderLibDef lib;
	Deserializer libReader(bufStream);
	libReader(lib);

	return lib;
}
