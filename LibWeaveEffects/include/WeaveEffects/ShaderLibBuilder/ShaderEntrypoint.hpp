#pragma once
#include "ShaderParser/SymbolEnums.hpp"

namespace Weave::Effects
{
	struct ShaderEntrypoint
	{
		string name;
		ShadeStages stage;
		int symbolID;
	};
}