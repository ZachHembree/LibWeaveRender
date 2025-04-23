#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <mutex>
#include <atomic>
#include "WeaveUtils/Utils.hpp"
#include "ObjectPool.hpp"

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

#ifndef WV_LOG_TIME_S
// 
/// Defines the minimum time in seconds between automatic flushes of the log buffer
/// for buffered streams (like files). A smaller value means more frequent writes.
/// Defaults to 10.0 seconds.
/// 
#define WV_LOG_TIME_S 10.0
#endif // !WV_LOG_TIME_S

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
#else
// Disabled Error log macro (due to compile-time WV_LOG_LEVEL).
#define WV_LOG_ERROR() Weave::Logger::GetNullMessage()
#endif

#if WV_LOG_LEVEL >= WV_LOG_WARN_LEVEL
// Logs a Warning message. Usage: WV_LOG_WARN() << "Warning message " << value;
#define WV_LOG_WARN() Weave::Logger::Log(Weave::Logger::Level::Warning)
#else
// Disabled Warning log macro (due to compile-time WV_LOG_LEVEL).
#define WV_LOG_WARN() Weave::Logger::GetNullMessage()
#endif

#if WV_LOG_LEVEL >= WV_LOG_INFO_LEVEL
// Logs an Info message. Usage: WV_LOG_INFO() << "Info message " << value;
#define WV_LOG_INFO() Weave::Logger::Log(Weave::Logger::Level::Info)
#else
// Disabled Info log macro (due to compile-time WV_LOG_LEVEL).
#define WV_LOG_INFO() Weave::Logger::GetNullMessage()
#endif

#if WV_LOG_LEVEL >= WV_LOG_DEBUG_LEVEL
// Logs a Debug message. Usage: WV_LOG_DEBUG() << "Debug message " << value;
#define WV_LOG_DEBUG() Weave::Logger::Log(Weave::Logger::Level::Debug)
#else
// Disabled Debug log macro (due to compile-time WV_LOG_LEVEL).
#define WV_LOG_DEBUG() Weave::Logger::GetNullMessage()
#endif

namespace Weave
{
    /// <summary>
    /// Provides a thread-safe, singleton logging facility with multiple output streams,
    /// configurable compile-time and runtime log levels, message buffering,
    /// and basic duplicate message suppression.
    /// </summary>
    class Logger
    {
    public:
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
        /// Represents a log message being built.
        /// RAII: Acquires a stringstream buffer on creation and writes the
        /// formatted log entry to the Logger's output streams upon destruction.
        /// This object is move-only.
        /// </summary>
        struct Message
        {
            friend Logger;
            MAKE_MOVE_ONLY(Message)

            /// <summary>
            /// Destructor. If the message is not discarded and has content,
            /// it triggers the write operation to the configured log outputs
            /// and returns the internal buffer to the pool.
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
            Level level;            ///< The logging level of this message.
            MessageBuffer pMsgBuf;  ///< Unique pointer to the stringstream buffer holding the message content.

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
        /// Checks if the logger has been initialized via Init() or InitToFile().
        /// </summary>
        /// <returns>True if the logger is initialized, false otherwise.</returns>
        static bool GetIsInitialized();

        /// <summary>
        /// Initializes the logger with a primary output stream.
        /// This should be called only once. Subsequent calls to AddStream can add more outputs.
        /// </summary>
        /// <param name="logOut">The output stream to write logs to. The caller is responsible for the stream's lifetime.</param>
        /// <param name="fast">If true, writes to this stream are unbuffered and immediate. Suitable for console output.
        /// If false (default), writes are buffered and flushed periodically or on logger destruction. Suitable for files.</param>
        /// <remarks>Throws an exception if the logger is already initialized.</remarks>
        static void Init(std::ostream& logOut, bool fast = false);

        /// <summary>
        /// Initializes the logger to write to a specified file path.
        /// Creates/truncates the file. This implicitly calls Init with the file stream.
        /// </summary>
        /// <param name="logPath">The path to the log file.</param>
        /// <remarks>Throws an exception if the file cannot be opened or if the logger is already initialized.</remarks>
        static void InitToFile(const std::filesystem::path& logPath);

        /// <summary>
        /// Adds an additional output stream to the logger.
        /// If the logger hasn't been initialized, this call will initialize it with the given stream.
        /// </summary>
        /// <param name="logOut">The output stream to add. The caller is responsible for the stream's lifetime.</param>
        /// <param name="fast">If true, writes to this stream are unbuffered and immediate. If false, writes are buffered.</param>
        static void AddStream(std::ostream& logOut, bool fast = false);

        /// <summary>
        /// Writes a pre-formatted message string directly to the log outputs.
        /// This is primarily used internally by the Message destructor.
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
        /// Checks if a given log level is currently enabled, considering both the
        /// compile-time level (WV_LOG_LEVEL) and the runtime level (set by SetLogLevel).
        /// </summary>
        /// <param name="level">The log level to check.</param>
        /// <returns>True if the level is enabled, false otherwise.</returns>
        static constexpr bool GetIsLevelEnabled(Logger::Level level);

        /// <summary>
        /// Sets the maximum log level that will be processed at runtime.
        /// Messages with a level higher than this will be discarded, even if enabled at compile time.
        /// </summary>
        /// <param name="level">The maximum log level to allow at runtime.</param>
        /// <remarks>Requires the logger to be initialized first.</remarks>
        static void SetLogLevel(Logger::Level level);

    private:
        MAKE_MOVE_ONLY(Logger)

        static Logger instance;
        static const NullMessage nullMsg;
        static std::mutex mutex;

        std::stringstream logBuffer;
        std::stringstream msgBuffer;
        std::string utf8ConvBuffer;

        /// Circular buffer for duplicate detection.
        DynamicArray<std::stringstream> msgHistory; 
        uint historyIndex;
        Stopwatch flushTimer;

        std::fstream logFile;
        UniqueVector<std::ostream*> bufferedLogStreams;
        UniqueVector<std::ostream*> logStreamsFast;

        ObjectPool<MessageBuffer> sstreamPool;
        std::atomic<bool> isInitialized;
        std::atomic<uint> logLevel; 

        Logger();

        ~Logger();

        /// <summary>Adds a formatted timestamp "[YYYY-MM-DD][HH:MM:SS]" to the given stream.</summary>
        /// <param name="stream">The output stream to write the timestamp to.</param>
        static void AddTimestamp(std::ostream& stream);

        /// <summary>Gets a stringstream buffer from the pool or creates a new one.</summary>
        /// <returns>A MessageBuffer (unique_ptr to a stringstream).</returns>
        static MessageBuffer GetStrBuf();

        /// <summary>Returns a stringstream buffer to the pool.</summary>
        /// <param name="buf">The MessageBuffer to return (ownership is transferred).</param>
        static void ReturnStrBuf(MessageBuffer&& buf);

        /// <summary>
        /// Checks if the message is a duplicate of a recently logged message.
        /// If it's a new message, it's added to the history buffer.
        /// </summary>
        /// <param name="message">The message content to check.</param>
        /// <returns>True if the message is new and should be logged, false if it's a duplicate.</returns>
        bool TryBufferLog(std::string_view message);

        /// <summary>
        /// Writes the content of the internal logBuffer to all non-fast output streams
        /// and clears the buffer. Resets the flush timer.
        /// </summary>
        /// <remarks>Throws exception if called when logger is not initialized.</remarks>
        void FlushLogBuffer();
    };
} 