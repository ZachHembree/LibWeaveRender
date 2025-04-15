#include "pch.hpp"
#include "WeaveEffects/ShaderData.hpp"
#include "WeaveEffects/ShaderDataSerialization.hpp"

using namespace Weave;
using namespace Weave::Effects;

ShaderLibDef Weave::Effects::GetDeserializedLibDef(string_view libData)
{
	static thread_local std::stringstream bufStream;
	bufStream.clear();
	bufStream.str({});

	ShaderLibDef lib;

	bufStream << libData;
	Deserializer libReader(bufStream);
	libReader(lib);

	return lib;
}
