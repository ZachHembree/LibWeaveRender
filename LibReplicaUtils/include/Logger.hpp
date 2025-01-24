#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <mutex>
#include "ReplicaUtils.hpp"
#include "ObjectPool.hpp"

namespace Replica
{ 
    class Logger
    {
    public:
        enum class Level : byte
        {
            Unknown = 0,
            Info = 1,
            Error = 2,
            Warning = 3,
            Debug = 4
        };

        /// <summary>
        /// Used for building log messages. Writes to output streams on destruction.
        /// </summary>
        struct Message
        {
            MAKE_MOVE_ONLY(Message)

            Message(Level level, std::stringstream&& msgBuf);

            ~Message();

            template <typename T>
            Message& operator<<(const T& value)
            {
                msgBuf << value;
                return *this;
            }

        private:
            Level level;
            std::stringstream msgBuf;

            static const char* GetLevelName(Level level);
        };

        /// <summary>
        /// Returns true if the logger has been initialized.
        /// </summary>
        static bool GetIsInitialized();

        /// <summary>
        /// Initializes the log to the given output stream
        /// </summary>
        static void Init(std::ostream& logOut);

        /// <summary>
        /// Creates a new log file at the given path and sets it as the output for the logger.
        /// </summary>
        static void InitToFile(std::string_view path);

        /// <summary>
        /// Adds an output stream to the logger.
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

        static void AddTimestamp(std::ostream& stream);

        template <typename T> 
        static void WriteValue(const T& value)
        {
            std::lock_guard<std::mutex> lock(mutex);

            if (!instance.isInitialized)
                throw std::runtime_error("Tried to write to uninitialized log.");

            for (std::ostream* pOut : instance.logStreams)
            {
                if (pOut->rdstate() == std::ios::goodbit)
                    *pOut << value;
                else
                    throw std::runtime_error("Failed to write to log stream.");
            }
        }

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

    private:
        MAKE_MOVE_ONLY(Logger)

        Logger();

        ~Logger();

        static std::stringstream GetStrBuf();

        static void ReturnStrBuf(std::stringstream&&);

        static Logger instance;
        static std::mutex mutex;

        std::fstream logFile;
        Vector<std::ostream*> logStreams;
        ObjectPool<std::stringstream> sstreamPool;
        bool isInitialized;
    };
}

#define LOG_INFO() Replica::Logger::Log(Replica::Logger::Level::Info)
#define LOG_ERROR() Replica::Logger::Log(Replica::Logger::Level::Error)
#define LOG_WARN() Replica::Logger::Log(Replica::Logger::Level::Warning)
#define LOG_DEBUG() Replica::Logger::Log(Replica::Logger::Level::Debug)
