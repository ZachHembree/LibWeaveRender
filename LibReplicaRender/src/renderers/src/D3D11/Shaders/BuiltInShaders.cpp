#include "pch.hpp"
#include "D3D11/Shaders/BuiltInShaders.hpp"
#include "D3D11/ShaderSrc/DefaultShaders.hpp"

using namespace Replica;
using namespace Replica::Effects;

ShaderLibDef Replica::Effects::GetBuiltInShaders()
{
	return GetDeserializedLibDef(s_FX_DefaultShaders);
}