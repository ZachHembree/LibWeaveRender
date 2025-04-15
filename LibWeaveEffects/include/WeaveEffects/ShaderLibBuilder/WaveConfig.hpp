#pragma once
#pragma warning(disable : 4996)

#define BOOST_WAVE_PRAGMA_KEYWORD "shader"
#define BOOST_WAVE_SUPPORT_CPP0X 1
#define BOOST_WAVE_SUPPORT_CPP1Z 0
#define BOOST_WAVE_SUPPORT_CPP2A 0
#define BOOST_WAVE_SUPPORT_MS_EXTENSIONS 0
#define BOOST_ALLOW_DEPRECATED_HEADERS

#include <boost/wave.hpp>
#include <boost/wave/cpplexer/cpp_lex_token.hpp>
#include <boost/wave/cpplexer/cpp_lex_iterator.hpp>
#include "boost/utility/string_view.hpp"

namespace Weave::Effects
{
	class VariantPreprocessor;

	/// <summary>
	/// Base class for exceptions thrown by Wave
	/// </summary>
	using WaveException = boost::wave::cpp_exception;

	/// <summary>
	/// Enum flags used by Boost.Wave to determine feature support
	/// </summary>
	using WaveLangSupport = boost::wave::language_support;

	/// <summary>
	/// Defines language features used by VariantPreprocessor
	/// </summary>
	constexpr WaveLangSupport VarProcLangFlags = WaveLangSupport(
		WaveLangSupport::support_option_no_newline_at_end_of_file |
		WaveLangSupport::support_option_variadics |
		WaveLangSupport::support_option_convert_trigraphs |
		WaveLangSupport::support_option_emit_line_directives |
		WaveLangSupport::support_option_include_guard_detection |
		WaveLangSupport::support_option_emit_pragma_directives |
		WaveLangSupport::support_option_insert_whitespace
	);
	
	/// <summary>
	/// Represents individual tokens (e.g., keywords, identifiers)
	/// </summary>
	using WaveLexToken = boost::wave::cpplexer::lex_token<>;

	/// <summary>
	/// Presents access to the lexer through WaveLexToken iterators. Tokens are evaluated on the fly.
	/// </summary>
	using WaveLexIterator = boost::wave::cpplexer::lex_iterator<WaveLexToken>;

	/// <summary>
	/// Base class for accessing preprocessor hooks
	/// </summary>
	using WaveContextPolicyBase = boost::wave::context_policies::eat_whitespace<WaveLexToken>;

	/// <summary>
	/// Extends preprocessor behavior
	/// </summary>
	class WaveContextPolicy : public WaveContextPolicyBase
	{
	public:
		WaveContextPolicy();

		WaveContextPolicy(VariantPreprocessor& procMain);

		/// <summary>
		/// Custom pragma hook
		/// </summary>
		template <typename WaveContextT, typename WaveTokenSeqT>
		bool interpret_pragma(
			WaveContextT const& ctx,
			WaveTokenSeqT& pending,
			WaveLexToken const& option,
			WaveTokenSeqT const& values,
			WaveLexToken const& pragma_token);

	private:
		VariantPreprocessor* pMain;
	};

	/// <summary>
	/// Wave-compatible string view
	/// </summary>
	using WaveStringView = boost::string_view;

	/// <summary>
	/// Determines iterator type for source input
	/// </summary>
	using WaveSrcIterator = WaveStringView::iterator;

	/// <summary>
	/// Defines how includes are loaded
	/// </summary>
	using WaveInputPolicy = boost::wave::iteration_context_policies::load_file_to_string;

	/// <summary>
	/// Main preprocessor interface. Used for configuring macros, includes and retrieving lexing iterators
	/// </summary>
	using WaveContext = boost::wave::context<WaveSrcIterator, WaveLexIterator, WaveInputPolicy, WaveContextPolicy>;

	/// <summary>
	/// A list of WaveLexTokens, in order
	/// </summary>
	using WaveLexTokenSeq = WaveContext::token_sequence_type;

	/// <summary>
	/// String type used internally by wave lexer
	/// </summary>
	using WaveString = WaveContext::string_type;

	/// <summary>
	/// Encapsulates WaveLexIterator for iterating tokens with WaveContext
	/// </summary>
	using WaveTokenIterator = WaveContext::iterator_type;
}