#pragma once
#ifdef WV_CEREAL_XML
#include <cereal/archives/xml.hpp>

namespace Weave
{
	using Deserializer = cereal::XMLInputArchive;
	using Serializer = cereal::XMLOutputArchive;
}
#elif defined(WV_CEREAL_JSON)
#include <cereal/archives/json.hpp>

namespace Weave
{
	using Deserializer = cereal::JSONInputArchive;
	using Serializer = cereal::JSONOutputArchive;
}
#elif defined(WV_CEREAL_PORTABLE_BIN)
#include <cereal/archives/portable_binary.hpp>

namespace Weave
{
	using Deserializer = cereal::PortableBinaryInputArchive;
	using Serializer = cereal::PortableBinaryOutputArchive;
}
#else
#include <cereal/archives/binary.hpp>

namespace Weave
{
	using Deserializer = cereal::BinaryInputArchive;
	using Serializer = cereal::BinaryOutputArchive;
}

#endif
#include <cereal/types/string.hpp>
#include "internal/DynCollectionSerializers.hpp"
