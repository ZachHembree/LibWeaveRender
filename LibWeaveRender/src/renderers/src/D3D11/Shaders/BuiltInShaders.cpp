#include "pch.hpp"
#include "D3D11/Shaders/BuiltInShaders.hpp"
#include "D3D11/ShaderSrc/DefaultShaders.hpp"

using namespace Weave;
using namespace Weave::Effects;

ShaderLibDef Weave::Effects::GetBuiltInShaders()
{
	return GetDeserializedLibDef(s_FX_DefaultShaders);
}