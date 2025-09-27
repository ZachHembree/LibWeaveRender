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
	static thread_local Vector<byte> zipBuffer;

	ShaderLibDef lib;
	DeserializeCompressedStream(libData, archive, zipBuffer, lib);

	return lib;
}
