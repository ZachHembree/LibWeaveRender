#pragma once
#include <optional>
#include <list>
#include <string_view>
#include "ShaderParser/SymbolEnums.hpp"
#include "ShaderParser/ShaderTypeInfo.hpp"

namespace Replica::Effects
{
	class AttribHandle;
	class ScopeBuilder;
	class FuncHandle;
	class VarHandle;
	class SymbolHandle;
	class ScopeHandle;

	using std::string_view;
	using std::optional;
	using IDList = std::list<int>;

	/// <summary>
	/// Wrapper providing an interface to tokens 
	/// </summary>
	class TokenNodeHandle
	{
	public:
		TokenNodeHandle(const ScopeBuilder& sb, int tokenID);

		/// <summary>
		/// Retuns the raw text value of the token, variable name, identifier, literal, etc.
		/// </summary>
		string_view GetValue() const;

		/// <summary>
		/// Returns unique int ID
		/// </summary>
		int GetID() const;

		/// <summary>
		/// Container depth of the token
		/// </summary>
		int GetDepth() const;

		/// <summary>
		/// Returns first LexBlock ID in the source range owned by the token.
		/// Includes child tokens.
		/// </summary>
		int GetBlockStart() const;

		/// <summary>
		/// Returns source range block count
		/// </summary>
		int GetBlockCount() const;

		/// <summary>
		/// Returns token flags
		/// </summary>
		TokenTypes GetFlags() const;

		/// <summary>
		/// Returns child token at the given index
		/// </summary>
		TokenNodeHandle GetChild(int index) const;

		/// <summary>
		/// Returns child token at the given index
		/// </summary>
		TokenNodeHandle operator[](int index) const;

		/// <summary>
		/// Returns the number of child tokens owned
		/// </summary>
		int GetChildCount() const;

		/// <summary>
		/// Returns true if the given token flags are set
		/// </summary>
		bool GetHasFlags(TokenTypes flags) const;

		/// <summary>
		/// Returns true if the token is an identifier for a symbol
		/// </summary>
		bool GetHasSymbol() const;

		/// <summary>
		/// Returns symbol identified by the token
		/// </summary>
		optional<SymbolHandle> GetSymbol() const;

		optional<AttribHandle> GetAsAttribute() const;

		bool operator==(const TokenNodeHandle& other) const;

		bool operator!=(const TokenNodeHandle& other) const;

	protected:
		const int tokenID;
		const ScopeBuilder* pSB;
	};

	class AttribHandle : public TokenNodeHandle
	{
	public:
		AttribHandle(const ScopeBuilder& sb, int id);

		int GetSemanticIndex() const;

	private:
	};

	/// <summary>
	/// Wrapper providing an interface to shader symbols
	/// </summary>
	class SymbolHandle
	{
	public:
		SymbolHandle(const ScopeBuilder& sb, int id);

		/// <summary>
		/// Returns true if the symbol has an identifier token
		/// </summary>
		bool GetHasIdent() const;

		/// <summary>
		/// Returns handle to symbol ident token
		/// </summary>
		TokenNodeHandle GetIdent() const;

		/// <summary>
		/// Returns symbol identifier text
		/// </summary>
		string_view GetName() const;

		/// <summary>
		/// Returns unique int ID
		/// </summary>
		int GetID() const;

		/// <summary>
		/// Returns true if the symbol declares a scope
		/// </summary>
		bool GetIsScope() const;

		/// <summary>
		/// Returns handle to the scope declared by the symbol, if it 
		/// exists
		/// </summary>
		optional<ScopeHandle> GetScope() const;

		/// <summary>
		/// Returns symbol flags
		/// </summary>
		SymbolTypes GetFlags() const;

		/// <summary>
		/// Returns true if the given flags are set in the symbol
		/// </summary>
		bool GetHasFlags(SymbolTypes flags) const;

		/// <summary>
		/// Returns symbol as a function, if applicable
		/// </summary>
		optional<FuncHandle> GetAsFunc() const;

		/// <summary>
		/// Returns symbol as a variable, if applicable
		/// </summary>
		optional<VarHandle> GetAsVar() const;

		bool operator==(const SymbolHandle& other) const;

		bool operator!=(const SymbolHandle& other) const;

	protected:
		int symbolID;
		const ScopeBuilder* pSB;
	};

	/// <summary>
	/// Variable symbol accessor
	/// </summary>
	class VarHandle : public SymbolHandle
	{
	public:
		VarHandle(const ScopeBuilder& sb, int id);

		/// <summary>
		/// Returns the size of the variable
		/// </summary>
		size_t GetSize() const;

		/// <summary>
		/// Returns type data for the variable symbol
		/// </summary>
		const ShaderTypeInfo& GetType() const;

		/// <summary>
		/// Returns type data for the variable symbol
		/// </summary>
		void GetType(ShaderTypeInfo& typeInfo, TokenTypes& typeFlags) const;
	};

	/// <summary>
	/// Function symbol accessor
	/// </summary>
	class FuncHandle : public SymbolHandle
	{
	public:
		FuncHandle(const ScopeBuilder& sb, int id);

		/// <summary>
		/// Returns function signature as a string
		/// </summary>
		string_view GetSignature() const;

		/// <summary>
		/// Returns type data for func return type
		/// </summary>
		const ShaderTypeInfo& GetReturnType() const;

		/// <summary>
		/// Gets type data for func return type
		/// </summary>
		void GetReturnType(ShaderTypeInfo& typeInfo, TokenTypes& typeFlags) const;
	};

	/// <summary>
	/// Wrapper providing access to scope members
	/// </summary>
	class ScopeHandle
	{
	public:
		ScopeHandle(const ScopeBuilder& sb, int id);

		/// <summary>
		/// Returns unique int ID
		/// </summary>
		int GetID() const;

		/// <summary>
		/// Returns first block in source range defining the scope, starting with opening {
		/// </summary>
		int GetBlockStart() const;

		/// <summary>
		/// Returns total number of source blocks contained by the scope
		/// </summary>
		int GetBlockCount() const;

		/// <summary>
		/// Returns symbol declaring the scope
		/// </summary>
		SymbolHandle GetSymbol() const;

		/// <summary>
		/// Returns child symbol at the given index
		/// </summary>
		SymbolHandle GetChild(int index) const;

		/// <summary>
		/// Returns child symbol at the given index
		/// </summary>
		SymbolHandle operator[](int index) const;

		/// <summary>
		/// Returns the number of child symbols declared in the scope
		/// </summary>
		int GetChildCount() const;

		/// <summary>
		/// Tries to return the child symbol with the given identifier string
		/// </summary>
		optional<SymbolHandle> TryGetChild(string_view ident) const;

		/// <summary>
		/// Tries to return a list of function overloads sharing the given identifier,
		/// within the scope
		/// </summary>
		const IDList* TryGetFuncOverloads(string_view ident) const;

		/// <summary>
		/// Returns true if the given scope is the global scope
		/// </summary>
		bool GetIsGlobal() const;

		/// <summary>
		/// Returns the containing scope, if one exists
		/// </summary>
		optional<ScopeHandle> GetParentScope() const;

		bool operator==(const ScopeHandle& other) const;

		bool operator!=(const ScopeHandle& other) const;

	private:
		int scopeID;
		const ScopeBuilder* pSB;
	};
}