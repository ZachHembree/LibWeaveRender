#pragma once
#include <sstream>
#include <fstream>
#include <mutex>
#include <atomic>
#include <thread>
#include "GlobalUtils.hpp"
#include "TextUtils.hpp"
#include "DynamicCollections.hpp"
#include "ObjectPool.hpp"
#include "WeaveException.hpp"

// --- Compile-Time Configuration ---

#ifndef WV_LOG_LEVEL
#ifdef NDEBUG
// 
/// Default compile-time log level for Release builds.
/// Controls which log macros compile down to actual logging calls.
/// Levels: 1=Error, 2=Warning, 3=Info, 4=Debug.
/// Set to 3 (Info) by default in Release.
/// 
#define WV_LOG_LEVEL 3
#else
// 
/// Default compile-time log level for Debug builds.
/// Controls which log macros compile down to actual logging calls.
/// Levels: 1=Error, 2=Warning, 3=Info, 4=Debug.
/// Set to 4 (Debug) by default in Debug.
/// 
#define WV_LOG_LEVEL 4
#endif // !NDEBUG
#endif // !WV_LOG_LEVEL

#ifndef WV_LOG_TIME_MS
// 
/// Defines the minimum time in milliseconds between automatic flushes of the log buffer
/// for buffered writes. A smaller value means more frequent writes.
/// 
#define WV_LOG_TIME_MS 500
#endif // !WV_LOG_TIME_MS

// --- Log Level Definitions ---

#define WV_LOG_ERROR_LEVEL 1
#define WV_LOG_WARN_LEVEL 2
#define WV_LOG_INFO_LEVEL 3
#define WV_LOG_DEBUG_LEVEL 4

// --- Logging Macros ---
// These macros provide a convenient way to log messages.
// They check the compile-time level (WV_LOG_LEVEL) and runtime level.
// If the level is disabled, they return a NullMessage object, effectively
// compiling out the logging overhead for disabled levels.

#if WV_LOG_LEVEL >= WV_LOG_ERROR_LEVEL
// Logs an Error message. Usage: WV_LOG_ERROR() << "Error message " << value;
#define WV_LOG_ERROR() Weave::Logger::Log(Weave::Logger::Level::Error)
// Logs a Warning message with std::format. Usage: WV_LOG_ERROR_FMT("Error message {}", value);
#define WV_LOG_ERROR_FMT(...) Weave::Logger::Log(Weave::Logger::Level::Error, __VA_ARGS__)
#else
// Disabled Error log macro (due to compile-time WV_LOG_LEVEL).
#define WV_LOG_ERROR() Weave::Logger::GetNullMessage()
// Disabled Info log macro (due to compile-time WV_LOG_LEVEL).
#define WV_LOG_ERROR_FMT(...) WV_EMPTY()
#endif

#if WV_LOG_LEVEL >= WV_LOG_WARN_LEVEL
// Logs a Warning message. Usage: WV_LOG_WARN() << "Warning message " << value;
#define WV_LOG_WARN() Weave::Logger::Log(Weave::Logger::Level::Warning)
// Logs a Warning message with std::format. Usage: WV_LOG_WARN_FMT("Warning message {}", value);
#define WV_LOG_WARN_FMT(...) Weave::Logger::Log(Weave::Logger::Level::Warning, __VA_ARGS__)
#else
// Disabled Warning log macro (due to compile-time WV_LOG_LEVEL).
#define WV_LOG_WARN() Weave::Logger::GetNullMessage()
// Disabled Info log macro (due to compile-time WV_LOG_LEVEL).
#define WV_LOG_WARN_FMT(...) WV_EMPTY()
#endif

#if WV_LOG_LEVEL >= WV_LOG_INFO_LEVEL
// Logs an Info message. Usage: WV_LOG_INFO() << "Info message " << value;
#define WV_LOG_INFO() Weave::Logger::Log(Weave::Logger::Level::Info)
// Logs a Info message with std::format. Usage: WV_LOG_INFO_FMT("Info message {}", value);
#define WV_LOG_INFO_FMT(...) Weave::Logger::Log(Weave::Logger::Level::Info, __VA_ARGS__)
#else
// Disabled Info log macro (due to compile-time WV_LOG_LEVEL).
#define WV_LOG_INFO() Weave::Logger::GetNullMessage()
// Disabled Info log macro (due to compile-time WV_LOG_LEVEL).
#define WV_LOG_INFO_FMT(...) WV_EMPTY()
#endif

#if WV_LOG_LEVEL >= WV_LOG_DEBUG_LEVEL
// Logs a Debug message. Usage: WV_LOG_DEBUG() << "Debug message " << value;
#define WV_LOG_DEBUG() Weave::Logger::Log(Weave::Logger::Level::Debug)
// Logs a Debug message with std::format. Usage: WV_LOG_DEBUG_FMT("Debug message {}", value);
#define WV_LOG_DEBUG_FMT(...) Weave::Logger::Log(Weave::Logger::Level::Debug, __VA_ARGS__)
#else
// Disabled Debug log macro (due to compile-time WV_LOG_LEVEL).
#define WV_LOG_DEBUG() Weave::Logger::GetNullMessage()
// Disabled Debug log macro (due to compile-time WV_LOG_LEVEL).
#define WV_LOG_DEBUG_FMT(...) WV_EMPTY()
#endif

namespace Weave
{
    /// <summary>
    /// Provides a thread-safe, singleton logging facility with multiple output streams, 
    /// configurable compile-time and runtime log levels, message buffering, and basic 
    /// duplicate message suppression.
    /// </summary>
    class Logger
    {
    public:
        /// <summary>
        /// Logger callback function for writing to external streams. Must be thread safe.
        /// </summary>
        typedef void (*LogWriteCallback)(string_view);

        using MessageBuffer = std::unique_ptr<std::stringstream>;

        /// <summary>
        /// Defines the severity levels for log messages.
        /// </summary>
        enum class Level : uint
        {
            Unknown = 0,
            Error = WV_LOG_ERROR_LEVEL,
            Warning = WV_LOG_WARN_LEVEL,
            Info = WV_LOG_INFO_LEVEL,
            Debug = WV_LOG_DEBUG_LEVEL,
            Discard = 10
        };

        /// <summary>
        /// Represents a log message being built. RAII: Acquires a stringstream buffer 
        /// on creation and writes the formatted log entry to the Logger's output 
        /// streams upon destruction.
        /// </summary>
        struct Message
        {
            friend Logger;
            MAKE_MOVE_ONLY(Message)

            /// <summary>
            /// Destructor. If the message is not discarded and has content, it triggers 
            /// the write operation to the configured log outputs and returns the internal 
            /// buffer to the pool.
            /// </summary>
            ~Message();

            /// <summary>Streams a wide string view into the log message, converting it to UTF-8.</summary>
            Message& operator<<(std::wstring_view value);

            /// <summary>Streams a null-terminated wide C-string into the log message, converting it to UTF-8.</summary>
            Message& operator<<(wchar_t* pValue);

            /// <summary>Streams a wide string into the log message, converting it to UTF-8.</summary>
            Message& operator<<(const std::wstring& value);

            /// <summary>
            /// Generic stream insertion operator. Appends the value to the internal message buffer
            /// if the log level is not Discard.
            /// </summary>
            template <typename T>
            Message& operator<<(const T& value)
            {
                if (level != Level::Discard)
                    *pMsgBuf << value;

                return *this;
            }

        private:
            Level level;
            MessageBuffer pMsgBuf;

            /// <summary>Private default constructor for creating a discarded message.</summary>
            Message();

            /// <summary>Private constructor used by Logger::Log to create an active message.</summary>
            /// <param name="level">The log level for this message.</param>
            /// <param name="msgBuf">The stringstream buffer acquired from the pool.</param>
            Message(Level level, MessageBuffer&& msgBuf);
        };

        /// <summary>
        /// A placeholder object returned by logging macros when the log level is disabled.
        /// Its stream operator does nothing, effectively discarding any data streamed into it.
        /// </summary>
        struct NullMessage
        {
            template <typename T>
            const NullMessage& operator<<(const T&) const { return *this; }
        };

        /// <summary>
        /// Checks if the logger has been initialized via AddStream() or InitToFile().
        /// </summary>
        /// <returns>True if the logger is initialized, false otherwise.</returns>
        static bool GetIsInitialized();

        /// <summary>
        /// Initializes the logger to write to a specified file path.
        /// Creates/truncates the file. This implicitly calls AddStream with the file stream.
        /// </summary>
        /// <param name="logPath">The path to the log file.</param>
        /// <remarks>Throws an exception if the file cannot be opened or if the logger is already initialized.</remarks>
        static void InitToFile(const std::filesystem::path& logPath);

        /// <summary>
        /// Adds an additional output stream callback to the logger.
        /// If the logger hasn't been initialized, this call will initialize it with the given callback.
        /// </summary>
        /// <param name="logOutFunc">The thread safe write callback to add.</param>
        /// <param name="fast">If true, writes to this callback are unbuffered and immediate. If false, writes are buffered.</param>
        static void AddStream(LogWriteCallback logOutFunc, bool fast = false);

        /// <summary>
        /// Creates a log Message object for the specified level.
        /// Used by the logging macros (WV_LOG_ERROR, etc.).
        /// Checks both compile-time and runtime log levels.
        /// </summary>
        /// <param name="level">The desired log level.</param>
        /// <returns>A Logger::Message object ready for streaming, or a discarded Message if the level is disabled.</returns>
        static Message Log(Level level);

        /// <summary>
        /// Returns a reference to the singleton NullMessage object.
        /// Used by the logging macros when a level is disabled.
        /// </summary>
        /// <returns>A const reference to the NullMessage instance.</returns>
        static const NullMessage& GetNullMessage();

        /// <summary>
        /// Writes a log message using std::format with the given level
        /// </summary>
        template<typename... FmtArgs>
        static void Log(Level level, string_view fmt, FmtArgs&&... args)
        {
            if (GetIsLevelEnabled(level) && GetIsInitialized())
            {
                string msg = GetStringBuf();
                
                try
                {
                    VFormatTo(msg, fmt, std::forward<FmtArgs>(args)...);
                    WriteToLog(level, msg);
                }
                catch (const std::format_error& e) 
                {
                    WriteToLog(Level::Error, std::format("Format error in log: {}", e.what()));
                }

                ReturnStringBuf(std::move(msg));
            }
        }

        /// <summary>
        /// Checks if a given log level is currently enabled, considering both the
        /// compile-time level (WV_LOG_LEVEL) and the runtime level (set by SetLogLevel).
        /// </summary>
        /// <param name="level">The log level to check.</param>
        /// <returns>True if the level is enabled, false otherwise.</returns>
        static bool GetIsLevelEnabled(Logger::Level level);

        /// <summary>
        /// Sets the maximum log level that will be processed at runtime.
        /// Messages with a level higher than this will be discarded, even if enabled at compile time.
        /// </summary>
        /// <param name="level">The maximum log level to allow at runtime.</param>
        /// <remarks>Requires the logger to be initialized first.</remarks>
        static void SetLogLevel(Logger::Level level);

    private:
        MAKE_NO_COPY(Logger)

        static Logger s_Instance;
        std::jthread pollThread;

        std::stringstream logBuffer;
        std::stringstream msgBuffer;

        /// Circular buffers for duplicate detection.
        DynamicArray<string> msgHistory; 
        DynamicArray<double> msgTimes;
        DynamicArray<uint> msgCounts;
        uint historyIndex;

        std::fstream logFile;
        UniqueVector<LogWriteCallback> logWriteDeferred;
        UniqueVector<LogWriteCallback> logWriteFast;

        ObjectPool<MessageBuffer> sstreamPool;
        ObjectPool<string> stringPool;

        Logger();

        ~Logger();

        Logger(Logger&&) noexcept;

        Logger& operator=(Logger&&) noexcept;

        /// <summary>
        /// Writes a pre-formatted message string directly to the log outputs.
        /// It adds timestamps, level names, handles buffering, and duplicate suppression.
        /// </summary>
        /// <param name="level">The level of the message.</param>
        /// <param name="message">The message content (without timestamp or level prefix).</param>
        static void WriteToLog(Level level, std::string_view message);

        /// <summary>
        /// Gets the string representation of a log level enum.
        /// </summary>
        /// <param name="level">The log level.</param>
        /// <returns>A string view representing the level (e.g., "INFO", "ERROR").</returns>
        static std::string_view GetLevelName(Level level);

        /// <summary>Adds a formatted timestamp "[YYYY-MM-DD][HH:MM:SS]" to the given stream.</summary>
        /// <param name="stream">The output stream to write the timestamp to.</param>
        static void AddTimestamp(std::ostream& stream);

        /// <summary>Gets a stringstream buffer from the pool or creates a new one.</summary>
        /// <returns>A MessageBuffer (unique_ptr to a stringstream).</returns>
        static MessageBuffer GetStreamBuf();

        /// <summary>Returns a stringstream buffer to the pool.</summary>
        /// <param name="buf">The MessageBuffer to return (ownership is transferred).</param>
        static void ReturnStreamBuf(MessageBuffer&& buf);

        /// <summary>Gets a temporary string buffer from the pool or creates a new one.</summary>
        static string GetStringBuf();

        /// <summary>Returns a string buffer to the pool.</summary>
        static void ReturnStringBuf(string&& buf);

        /// <summary>
        /// Checks if the message is a duplicate of a recently logged message.
        /// If it's a new message, it's added to the history buffer.
        /// </summary>
        /// <param name="message">The message content to check.</param>
        /// <returns>True if the message is new and should be logged, false if it's a duplicate.</returns>
        bool TryBufferLog(std::string_view message);

        /// <summary>
        /// Writes out buffered logs to default callbacks
        /// </summary>
        void FlushLogBuffer();

        /// <summary>
        /// Initializes deferred log polling
        /// </summary>
        static void StartPolling();
    };
} 

#ifndef NDEBUG
#define WV_SOFT_THROW(...) WV_THROW(__VA_ARGS__)
#else
#define WV_SOFT_THROW(...) WV_LOG_WARN_FMT(__VA_ARGS__)
#endif

#define WV_ENSURE_MSG(COND, ...) WV_IF_NOT(COND, WV_SOFT_THROW(__VA_ARGS__))
#define WV_ENSURE(COND) WV_CHECK_MSG(COND, "Ensure failed")
