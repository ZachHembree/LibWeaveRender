#include "pch.hpp"
#include "ReplicaInternalD3D11.hpp"
#include "D3D11/Shaders/BuiltInShaders.hpp"
#include "D3D11/ShaderSrc/DefaultShaders.hpp"
#include "ShaderLibGen/ShaderDataSerializers.hpp"
#include <sstream>

using namespace Replica;
using namespace Replica::Effects;
using namespace Replica::D3D11;

static ShaderLibDef DeserializeLibData()
{
	const string_view libData((char*)&LibName[0], std::size(LibName));
	std::stringstream bufStream;
	ShaderLibDef lib;

	bufStream << libData;
	Deserializer libReader(bufStream);
	libReader(lib);

	return lib;
}

static const ShaderLibDef s_DefaultLibDef = DeserializeLibData();

const ShaderLibDef& Replica::Effects::GetBuiltInShaders()
{
	return s_DefaultLibDef;
}