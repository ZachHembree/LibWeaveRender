#include "pch.hpp"
#include "ReplicaUtils/Logger.hpp"

namespace Replica
{
    Logger Logger::instance;
    std::mutex Logger ::mutex;

    Logger::Logger() : isInitialized(false)
    { }

    Logger::~Logger() = default;

    void Logger::Reset() { instance = Logger(); }

    bool Logger::GetIsInitialized() { return instance.isInitialized; }

    void Logger::Init(std::ostream& logOut)
    {
        std::lock_guard<std::mutex> lock(mutex);

        if (!instance.isInitialized)
            instance.logStreams.Add(&logOut);
        else
            throw std::runtime_error("Tried to initialize logger twice.");

        instance.isInitialized = true;
    }

    void Logger::InitToFile(std::string_view path)
    {
        std::filesystem::path logPath(path);
        instance.logFile = std::fstream(logPath, std::ios::out | std::ios::trunc);
        
        if (!instance.logFile.is_open())
        {
            throw std::runtime_error("Failed to open log file: " + string(path));
        }

        Init(instance.logFile);
    }

    void Logger::AddStream(std::ostream& stream)
    {
        if (!instance.isInitialized)
            Init(stream);
        else
            instance.logStreams.Add(&stream);
    }

    std::stringstream Logger::GetStrBuf()
    {
        std::lock_guard<std::mutex> lock(mutex);
        std::stringstream buf = instance.sstreamPool.Get();
        buf.clear();
        buf.str({});

        return buf;
    }

    void Logger::ReturnStrBuf(std::stringstream&& buf)
    {
        std::lock_guard<std::mutex> lock(mutex);
        instance.sstreamPool.Return(std::move(buf));
    }

    Logger::Message Logger::Log(Level level)
    {
        return Message(level, GetStrBuf());
    }

    void Logger::AddTimestamp(std::ostream& stream)
    {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        std::tm tm_now;

#ifdef _WIN32
        localtime_s(&tm_now, &time_t_now);
#else
        localtime_r(&time_t_now, &tm_now);
#endif

        stream << std::put_time(&tm_now, "%Y-%m-%d %H:%M:%S");
    }

    Logger::Message::Message(Level level, std::stringstream&& buf) : 
        level(level), 
        msgBuf(std::move(buf))
    {
        // Start timestamp
        msgBuf << "[";
        AddTimestamp(msgBuf);
        msgBuf << "] " << std::format("[{}] ", GetLevelName(level));
    }

    Logger::Message::~Message()
    {
        Logger::WriteLine(msgBuf.str());   
        Logger::ReturnStrBuf(std::move(msgBuf));
    }

    const char* Logger::Message::GetLevelName(Level level)
    {
        switch (level)
        {
        case Level::Info: return "INFO";
        case Level::Error: return "ERROR";
        case Level::Warning: return "WARNING";
        case Level::Debug: return "DEBUG";
        default: return "UNKNOWN";
        }
    }
}