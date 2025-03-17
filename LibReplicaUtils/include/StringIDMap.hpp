#pragma once
#include "ReplicaUtils.hpp"
#include "Serialization.hpp"
#include <unordered_map>
#include <limits>

namespace Replica
{
    /// <summary>
    /// Serializable string ID mapping data
    /// </summary>
    struct StringIDMapDef
    {
        static constexpr uint INVALID_ID = std::numeric_limits<uint>::max();

        MAKE_NO_COPY(StringIDMapDef)

        StringIDMapDef();

        StringIDMapDef(StringIDMapDef&& other) noexcept;

        StringIDMapDef& operator=(StringIDMapDef&& other) noexcept;

        /// <summary>
        /// Alternating starting indices + string length. ID == index / 2
        /// </summary>
        UniqueArray<uint> substrings;

        /// <summary>
        /// Concatenated strings stored in the order they were created
        /// </summary>
        UniqueArray<char> stringData;
    };

    template<class Archive>
    void serialize(Archive& ar, StringIDMapDef& def)
    {
        ar(def.substrings, def.stringData);
    }

    /// <summary>
    /// Read-only interface for string ID lookup
    /// </summary>
    class StringIDMap
    {
    public:
        MAKE_NO_COPY(StringIDMap)

            static constexpr uint INVALID_ID = StringIDMapDef::INVALID_ID;

        StringIDMap(StringIDMapDef&& def) noexcept;

        explicit StringIDMap(const StringIDMapDef& def);

        /// <summary>
        /// Returns true if the string exists in the map and retrieves its ID
        /// </summary>
        bool TryGetStringID(std::string_view str, uint& id) const;

        /// <summary>
        /// Returns the string_view corresponding to the given ID
        /// </summary>
        std::string_view GetString(uint id) const;

        /// <summary>
        /// Returns the number of unique strings in the map
        /// </summary>
        uint GetStringCount() const;

    private:
        // Concatenated text buffer
        UniqueArray<char> stringBuf;
        // Substrings pointing to text buffer and ID -> String map
        UniqueArray<std::string_view> substrings;
        // String -> ID map
        std::unordered_map<std::string_view, uint> idMap;

        void InitMapData(const StringIDMapDef& def);
    };

}