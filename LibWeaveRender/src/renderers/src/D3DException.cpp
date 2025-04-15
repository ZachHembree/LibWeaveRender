#include "pch.hpp"
#include <sstream>
#include "../src/renderers/include/D3DException.hpp"

using namespace std;
using namespace Weave;
using namespace Weave::D3D11;

string_view D3DException::GetType() const noexcept
{
	return "D3D Exception";
}
