#include "pch.hpp"
#include "WeaveUtils/Compression.hpp"
#include "WeaveUtils/Span.hpp"
#include "WeaveEffects/ShaderData.hpp"
#include "WeaveEffects/ShaderDataSerialization.hpp"

using namespace Weave;
using namespace Weave::Effects;

ShaderLibDef Weave::Effects::GetDeserializedLibDef(string_view libData)
{
	static thread_local ZLibArchive archive;

	// Deserialize zip wrapper
	{
		// Interpret input as istream
		std::istrstream inStream(libData.data(), (int)libData.size());
		Deserializer zipReader(inStream);
		zipReader(archive);
	}

	// Unzip shader data
	static thread_local Vector<byte> zipBuffer;
	DecompressBytes(archive, zipBuffer);

	// Interpret zip buffer as char istream
	std::istrstream vecStream(reinterpret_cast<char*>(zipBuffer.GetData()), (int)zipBuffer.GetLength());
	Deserializer libReader(vecStream);
	ShaderLibDef lib;
	// Deserialize decompressed data
	libReader(lib);

	return lib;
}
