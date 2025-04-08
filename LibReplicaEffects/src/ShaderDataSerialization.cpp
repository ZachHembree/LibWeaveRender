#include "pch.hpp"
#include "ReplicaEffects/ShaderDataSerialization.hpp"

using namespace Replica;
using namespace Replica::Effects;

ShaderLibDef Replica::Effects::GetDeserializedLibDef(string_view libData)
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
