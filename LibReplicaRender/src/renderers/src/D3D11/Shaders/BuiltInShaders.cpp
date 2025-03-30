#include "pch.hpp"
#include "ReplicaInternalD3D11.hpp"
#include "D3D11/ShaderSrc/CS_TexCopy2D.hpp"
#include "D3D11/ShaderSrc/CS_TexCopySamp2D.hpp"
#include "D3D11/ShaderSrc/CS_TexCopyScaledSamp2D.hpp"
#include "D3D11/ShaderSrc/PS_Default.hpp"
#include "D3D11/ShaderSrc/PS_Flat3D.hpp"
#include "D3D11/ShaderSrc/PS_Textured.hpp"
#include "D3D11/ShaderSrc/VS_3D.hpp"
#include "D3D11/ShaderSrc/VS_Default.hpp"
#include "D3D11/ShaderSrc/VS_PosTextured.hpp"
#include "D3D11/ShaderSrc/VS_Textured2D.hpp"
#include "D3D11/ShaderSrc/VS_Textured3D.hpp"
#include "D3D11/Shaders/BuiltInShaders.hpp"

using namespace Replica;
using namespace Replica::Effects;
using namespace Replica::D3D11;

const ShaderLibDef s_BuiltInShaders = {};

const ShaderLibDef& Replica::Effects::GetBuiltInShaders()
{
	return s_BuiltInShaders;
}