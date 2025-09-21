#pragma once
#include "WeaveUtils/GlobalUtils.hpp"
#include "WeaveUtils/DynamicCollections.hpp"
#include <unordered_map>
#include <limits>

namespace Weave
{
    /// <summary>
    /// Serializable string ID mapping data
    /// </summary>
    struct StringIDMapDef
    {
        static constexpr uint INVALID_ID = -1;

        /// <summary>
        /// Alternating starting indices + string length. ID == index / 2
        /// </summary>
        Vector<uint> substrings;

        /// <summary>
        /// Concatenated strings stored in the order they were created
        /// </summary>
        string stringData;

        /// <summary>
        /// Represents a serializable, non-owning view to the underlying definition data
        /// </summary>
        struct Handle
        {
            const Vector<uint>* pSubstrings;
            const string* pStringData;

            /// <summary>
            /// Returns a deep copy of the definition data
            /// </summary>
            StringIDMapDef GetCopy() const
            {
                return 
                {
                    .substrings = *pSubstrings,
                    .stringData = *pStringData
                };
            }
        };

        /// <summary>
        /// Returns a serializable, non-owning view to the underlying definition data
        /// </summary>
        Handle GetHandle() const
        {
            return 
            {
                .pSubstrings = &substrings,
                .pStringData = &stringData
            };
        }

        void Clear();
    };

    /// <summary>
    /// Read-only interface for string ID lookup
    /// </summary>
    class StringIDMap
    {
    public:
        MAKE_NO_COPY(StringIDMap)

        static constexpr uint INVALID_ID = StringIDMapDef::INVALID_ID;

        explicit StringIDMap(const StringIDMapDef& def);

        explicit StringIDMap(StringIDMapDef&& def);

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
        std::unique_ptr<StringIDMapDef> pDef;
        // String -> ID map
        std::unordered_map<std::string_view, uint> idMap;

        void InitMapData();
    };

}