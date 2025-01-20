#pragma once
#ifdef RPFX_CEREAL_XML
#include <cereal/archives/xml.hpp>

namespace Replica::Effects
{
	using Deserializer = cereal::XMLInputArchive;
	using Serializer = cereal::XMLOutputArchive;
}
#elif defined(RPFX_CEREAL_JSON)
#include <cereal/archives/json.hpp>

namespace Replica::Effects
{
	using Deserializer = cereal::JSONInputArchive;
	using Serializer = cereal::JSONOutputArchive;
}
#elif defined(RPFX_CEREAL_PORTABLE_BIN)
#include <cereal/archives/portable_binary.hpp>

namespace Replica::Effects
{
	using Deserializer = cereal::PortableBinaryInputArchive;
	using Serializer = cereal::PortableBinaryOutputArchive;
}
#else
#include <cereal/include/cereal/archives/binary.hpp>

namespace Replica::Effects
{
	using Deserializer = cereal::BinaryInputArchive;
	using Serializer = cereal::BinaryOutputArchive;
}

#endif
#include <cereal/include/cereal/types/string.hpp>
#include "internal/DynCollectionSerializers.hpp"
