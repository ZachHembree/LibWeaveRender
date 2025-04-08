#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Shaders/BuiltInShaders.hpp"
#include "D3D11/ShaderSrc/DefaultShaders.hpp"
#include "ReplicaEffects/ShaderDataSerialization.hpp"
#include <sstream>

using namespace Replica;
using namespace Replica::Effects;
using namespace Replica::D3D11;

static const ShaderLibDef s_DefaultLibDef = GetDeserializedLibDef(s_FX_DefaultShaders);

const ShaderLibDef& Replica::Effects::GetBuiltInShaders()
{
	return s_DefaultLibDef;
}