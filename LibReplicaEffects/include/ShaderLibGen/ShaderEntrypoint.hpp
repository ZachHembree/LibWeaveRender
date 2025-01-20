#pragma once
#include "ShaderParser/SymbolEnums.hpp"

namespace Replica::Effects
{
	struct ShaderEntrypoint
	{
		string name;
		ShadeStages stage;
		int symbolID;
	};
}