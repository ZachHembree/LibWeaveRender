#pragma once
#include "WeaveUtils/GlobalUtils.hpp"
#include "WeaveUtils/DynamicCollections.hpp"
#include "WeaveUtils/StringSpan.hpp"
#include <unordered_map>

namespace Weave
{
    class StringIDBuilder;

    /// <summary>
    /// Serializable string ID mapping data
    /// </summary>
    struct StringIDMapDef
    {
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
    /// Abstract read-only interface for retrieving unique strings and IDs
    /// </summary>
    class IStringIDMap
    {
    public:
        /// <summary>
        /// Returns true if the string exists in the map and retrieves its ID
        /// </summary>
        virtual bool TryGetStringID(std::string_view str, uint& id) const = 0;

        /// <summary>
        /// Returns the string_view corresponding to the given ID
        /// </summary>
        virtual const StringSpan GetString(uint id) const = 0;

        /// <summary>
        /// Returns the number of unique strings in the map
        /// </summary>
        virtual uint GetStringCount() const = 0;

        /// <summary>
        /// Returns a read-only view to a serializable definition of the string ID map
        /// </summary>
        virtual StringIDMapDef::Handle GetDefinition() const = 0;

        /// <summary>
        /// Returns true if map represents a set of aliases in a StringIDBuilder
        /// </summary>
        virtual bool GetIsAlias() const { return false; }

        /// <summary>
        /// Returns the ID equivalent to the given local ID in the parent StringIDBuilder if this 
        /// map is an alias.
        /// </summary>
        virtual uint GetAliasedID(uint localID) const { return localID; }

        /// <summary>
        /// Returns true if the given ID is shared between a StringIDBuilder and a StringIDMapAlias
        /// </summary>
        static bool GetIsShared(uint id);

        virtual ~IStringIDMap() = default;
    };

    /// <summary>
    /// Read-only map for retrieving unique strings and IDs
    /// </summary>
    class StringIDMap : public IStringIDMap
    {
    public:
        MAKE_NO_COPY(StringIDMap)

        explicit StringIDMap(const StringIDMapDef::Handle& def);

        explicit StringIDMap(StringIDMapDef&& def);

        /// <summary>
        /// Returns true if the string exists in the map and retrieves its ID
        /// </summary>
        bool TryGetStringID(std::string_view str, uint& id) const override;

        /// <summary>
        /// Returns the string_view corresponding to the given ID
        /// </summary>
        const StringSpan GetString(uint id) const override;

        /// <summary>
        /// Returns the number of unique strings in the map
        /// </summary>
        uint GetStringCount() const override;

        /// <summary>
        /// Returns a read-only view to a serializable definition of the string ID map
        /// </summary>
        StringIDMapDef::Handle GetDefinition() const override;

    private:
        std::unique_ptr<StringIDMapDef> pDef;
        // String -> ID map
        mutable std::unordered_map<std::string_view, uint> idMap;

        void InitMapData() const;
    };

    /// <summary>
    /// StringID subset backed by a StringIDBuilder superset
    /// </summary>
    class StringIDMapAlias : public IStringIDMap
    {
    public:
        MAKE_NO_COPY(StringIDMapAlias)

        StringIDMapAlias(const StringIDMapDef::Handle& def, StringIDBuilder& parent);

        const StringIDBuilder& GetParent() const;

        /// <summary>
        /// Returns the equivalent stringID in the parent StringIDBuilder
        /// </summary>
        uint GetAliasedID(uint localID) const override;

        /// <summary>
        /// Always returns true because the StringIDMap is a set of aliases for another map
        /// </summary>
        bool GetIsAlias() const override { return true; }

        /// <summary>
        /// Returns true if the string exists in the map and retrieves its ID
        /// </summary>
        bool TryGetStringID(std::string_view str, uint& id) const override;

        /// <summary>
        /// Returns the string_view corresponding to the given ID
        /// </summary>
        const StringSpan GetString(uint id) const override;

        /// <summary>
        /// Returns the number of unique strings in the map
        /// </summary>
        uint GetStringCount() const override;

        /// <summary>
        /// Returns a read-only view to a serializable definition of the string ID map
        /// </summary>
        StringIDMapDef::Handle GetDefinition() const override;

    private:
        const StringIDBuilder* pParent;
        UniqueArray<uint> idAliases;
    };
}