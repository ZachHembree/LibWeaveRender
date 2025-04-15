#pragma once
#include <unordered_map>
#include "WeaveUtils/Utils.hpp"
#include "WeaveUtils/StringIDMap.hpp"
#include "WeaveUtils/StringSpan.hpp"

namespace Weave 
{
    /// <summary>
    /// Builds a map of uint IDs corresponding to unique strings
    /// </summary>
    class StringIDBuilder 
    {
    public:
        MAKE_NO_COPY(StringIDBuilder)

        static constexpr uint INVALID_ID = StringIDMapDef::INVALID_ID;

        StringIDBuilder();

        StringIDBuilder(StringIDBuilder&& other) noexcept;

        StringIDBuilder& operator=(StringIDBuilder&& other) noexcept;

        /// <summary>
        /// Returns the ID corresponding to the given string. Adds a copy of
        /// the string to the map if it hasn't been added previously.
        /// </summary>
        uint GetOrAddStringID(string_view str);

        /// <summary>
        /// Returns true if the string exists in the map and retrieves its ID
        /// </summary>
        bool TryGetStringID(string_view str, uint& id) const;

        /// <summary>
        /// Returns the string corresponding to the given ID
        /// </summary>
        string_view GetString(uint id) const;

        /// <summary>
        /// Returns the total number of strings mapped
        /// </summary>
        uint GetStringCount() const;

        /// <summary>
        /// Returns serializable handle to string ID map data
        /// </summary>
        StringIDMapDef::Handle GetDefinition() const;

        /// <summary>
        /// Clears all strings from the builder
        /// </summary>
        void Clear();

    private:
        string stringData;
        // String storage; ID -> string
        UniqueVector<uint> substrings;

        mutable string textBuf;
        // string -> ID map
        std::unordered_map<StringSpan, uint> idMap;

        static StringSpan GetTmpStringSpan(string_view str, string& buf);

        static void RemoveTmpStringSpan(const StringSpan& str, string& buf);
    };
}