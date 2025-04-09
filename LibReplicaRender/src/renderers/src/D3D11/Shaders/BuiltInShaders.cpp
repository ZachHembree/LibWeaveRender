#include "pch.hpp"
#include "D3D11/Shaders/BuiltInShaders.hpp"
#include "D3D11/ShaderSrc/DefaultShaders.hpp"

using namespace Replica;
using namespace Replica::Effects;

static const ShaderLibDef s_DefaultLibDef = GetDeserializedLibDef(s_FX_DefaultShaders);

const ShaderLibDef& Replica::Effects::GetBuiltInShaders()
{
	return s_DefaultLibDef;
}