#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <mutex>
#include "WeaveUtils/Utils.hpp"
#include "ObjectPool.hpp"

#ifndef WV_LOG_LEVEL
#ifdef NDEBUG
#define WV_LOG_LEVEL 3 // Debug logging disabled by default in release
#else
#define WV_LOG_LEVEL 4 // Default to maximum logging in debug
#endif // !NDEBUG
#endif 

#ifndef WV_LOG_TIME_S
// Defines the minimum time between log file writes
#define WV_LOG_TIME_S 10.0
#endif // !WV_LOG_TIME_S

#define WV_LOG_ERROR_LEVEL 1
#define WV_LOG_WARN_LEVEL 2
#define WV_LOG_INFO_LEVEL 3
#define WV_LOG_DEBUG_LEVEL 4

#if WV_LOG_LEVEL >= WV_LOG_ERROR_LEVEL
#define LOG_ERROR() Weave::Logger::Log(Weave::Logger::Level::Error)
#else
#define LOG_ERROR() Weave::Logger::GetNullMessage()
#endif

#if WV_LOG_LEVEL >= WV_LOG_WARN_LEVEL
#define LOG_WARN() Weave::Logger::Log(Weave::Logger::Level::Warning)
#else
#define LOG_WARN() Weave::Logger::GetNullMessage()
#endif

#if WV_LOG_LEVEL >= WV_LOG_INFO_LEVEL
#define LOG_INFO() Weave::Logger::Log(Weave::Logger::Level::Info)
#else
#define LOG_INFO() Weave::Logger::GetNullMessage()
#endif

#if WV_LOG_LEVEL >= WV_LOG_DEBUG_LEVEL
#define LOG_DEBUG() Weave::Logger::Log(Weave::Logger::Level::Debug)
#else
#define LOG_DEBUG() Weave::Logger::GetNullMessage()
#endif

namespace Weave
{ 
    class Logger
    {
    public:
        using MessageBuffer = std::unique_ptr<std::stringstream>;

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
        /// Used for building log messages. Writes to output streams on destruction.
        /// </summary>
        struct Message
        {
            friend Logger;
            MAKE_MOVE_ONLY(Message)
          
            ~Message();

            Message& operator<<(wstring_view value)
            {
                if (level != Level::Discard)
                {
                    GetMultiByteString_UTF16LE_TO_UTF8(value, instance.utf8ConvBuffer);
                    *pMsgBuf << instance.utf8ConvBuffer.data();
                }

                return *this;
            }

            Message& operator<<(wchar_t* pValue)
            {
                return operator<<(wstring_view(pValue));
            }

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

            Message();

            Message(Level level, MessageBuffer&& msgBuf);
        };

        /// <summary>
        /// Empty message handler for discarding logs below a set threshold
        /// </summary>
        struct NullMessage
        {
            template <typename T>
            const NullMessage& operator<<(const T&) const { return *this; }
        };

        /// <summary>
        /// Returns true if the logger has been initialized.
        /// </summary>
        static bool GetIsInitialized();

        /// <summary>
        /// Initializes the log to the given output stream. Caller is responsible for stream lifetime.
        /// </summary>
        static void Init(std::ostream& logOut, bool fast = false);

        /// <summary>
        /// Creates a new log file at the given path and sets it as the output for the logger.
        /// </summary>
        static void InitToFile(const std::filesystem::path& logPath);

        /// <summary>
        /// Adds an output stream to the logger. Caller is responsible for stream lifetime. 
        /// If fast == true, then the stream output will not be buffered.
        /// </summary>
        static void AddStream(std::ostream& logOut, bool fast = false);

        /// <summary>
        /// Writes a time stamped string to the log
        /// </summary>
        static void WriteToLog(Level level, string_view message);

        /// <summary>
        /// Returns the name of the give log level
        /// </summary>
        static string_view GetLevelName(Level level);

        /// <summary>
        /// Resets the logger to its default state.
        /// </summary>
        static void Reset();

        /// <summary>
        /// Returns a new log message at the given level.
        /// </summary>
        static Message Log(Level level);

        /// <summary>
        /// Returns an empty log message stream object
        /// </summary>
        static const NullMessage& GetNullMessage();

        /// <summary>
        /// Returns true if the given logging level is enabled
        /// </summary>
        static constexpr bool GetIsLevelEnabled(Logger::Level level);

        /// <summary>
        /// Sets runtime logging level
        /// </summary>
        static void SetLogLevel(Logger::Level level);

    private:
        static Logger instance;
        static const NullMessage nullMsg;
        static std::mutex mutex;

        std::stringstream logBuffer;
        std::stringstream msgBuffer;
        string utf8ConvBuffer;

        DynamicArray<std::stringstream> msgHistory;
        uint historyIndex;
        Stopwatch flushTimer;

        // Main logging file
        std::fstream logFile;
        Vector<std::ostream*> logStreams;
        // Fast log streams, unbuffered. Suitable for consoles.
        Vector<std::ostream*> logStreamsFast;

        ObjectPool<MessageBuffer> sstreamPool;
        bool isInitialized;
        uint logLevel;

        MAKE_MOVE_ONLY(Logger)

        Logger();

        ~Logger();

        static void AddTimestamp(std::ostream& stream);

        static MessageBuffer GetStrBuf();

        static void ReturnStrBuf(MessageBuffer&&);

        bool TryBufferLog(string_view message);

        void FlushLogBuffer();
    };
}
