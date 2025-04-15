#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <mutex>
#include "ReplicaUtils/Utils.hpp"
#include "ObjectPool.hpp"

#ifndef REP_LOG_LEVEL
#ifdef NDEBUG
#define REP_LOG_LEVEL 3 // Debug logging disabled by default in release
#else
#define REP_LOG_LEVEL 4 // Default to maximum logging in debug
#endif // !NDEBUG
#endif 

#define REP_LOG_ERROR_LEVEL 1
#define REP_LOG_WARN_LEVEL 2
#define REP_LOG_INFO_LEVEL 3
#define REP_LOG_DEBUG_LEVEL 4

#if REP_LOG_LEVEL >= REP_LOG_ERROR_LEVEL
#define LOG_ERROR() Replica::Logger::Log(Replica::Logger::Level::Error)
#else
#define LOG_ERROR() Replica::Logger::GetNullMessage()
#endif

#if REP_LOG_LEVEL >= REP_LOG_WARN_LEVEL
#define LOG_WARN() Replica::Logger::Log(Replica::Logger::Level::Warning)
#else
#define LOG_WARN() Replica::Logger::GetNullMessage()
#endif

#if REP_LOG_LEVEL >= REP_LOG_INFO_LEVEL
#define LOG_INFO() Replica::Logger::Log(Replica::Logger::Level::Info)
#else
#define LOG_INFO() Replica::Logger::GetNullMessage()
#endif

#if REP_LOG_LEVEL >= REP_LOG_DEBUG_LEVEL
#define LOG_DEBUG() Replica::Logger::Log(Replica::Logger::Level::Debug)
#else
#define LOG_DEBUG() Replica::Logger::GetNullMessage()
#endif

namespace Replica
{ 
    class Logger
    {
    public:
        using MessageBuffer = std::unique_ptr<std::stringstream>;

        enum class Level : uint
        {
            Unknown = 0,
            Error = REP_LOG_ERROR_LEVEL,
            Warning = REP_LOG_WARN_LEVEL,
            Info = REP_LOG_INFO_LEVEL,
            Debug = REP_LOG_DEBUG_LEVEL,
            Discard = 10
        };

        /// <summary>
        /// Used for building log messages. Writes to output streams on destruction.
        /// </summary>
        struct Message
        {
            MAKE_MOVE_ONLY(Message)

            Message();

            Message(Level level, MessageBuffer&& msgBuf);

            ~Message();

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

            static const char* GetLevelName(Level level);
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
        static void Init(std::ostream& logOut);

        /// <summary>
        /// Creates a new log file at the given path and sets it as the output for the logger.
        /// </summary>
        static void InitToFile(std::string_view path);

        /// <summary>
        /// Adds an output stream to the logger. Caller is responsible for stream lifetime.
        /// </summary>
        static void AddStream(std::ostream& logOut);

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
        MAKE_MOVE_ONLY(Logger)

        Logger();

        ~Logger();

        static void AddTimestamp(std::ostream& stream);

        template <typename T>
        static void WriteLine(const T& value)
        {
            std::lock_guard<std::mutex> lock(mutex);

            if (!instance.isInitialized)
                throw std::runtime_error("Tried to write to uninitialized log.");

            for (std::ostream* pOut : instance.logStreams)
            {
                if (pOut->rdstate() == std::ios::goodbit)
                    *pOut << value << std::endl;
                else
                    throw std::runtime_error("Failed to write to log stream.");
            }
        }

        static MessageBuffer GetStrBuf();

        static void ReturnStrBuf(MessageBuffer&&);

        static Logger instance;
        static const NullMessage nullMsg;
        static std::mutex mutex;

        std::fstream logFile;
        Vector<std::ostream*> logStreams;
        ObjectPool<MessageBuffer> sstreamPool;
        bool isInitialized;
        uint logLevel;
    };
}
