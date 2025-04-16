#include "pch.hpp"
#include "WeaveUtils/Logger.hpp"

namespace Weave
{
    Logger Logger::instance;
    const Logger::NullMessage Logger::nullMsg = {};
    std::mutex Logger ::mutex;

    Logger::Logger() : isInitialized(false), logLevel(WV_LOG_LEVEL)
    { }

    Logger::~Logger() = default;

    void Logger::Reset() { instance = Logger(); }

    bool Logger::GetIsInitialized() { return instance.isInitialized; }

    void Logger::Init(std::ostream& logOut)
    {
        std::lock_guard<std::mutex> lock(mutex);
        WV_CHECK_MSG(!instance.isInitialized, "Tried to initialize logger twice.");

        instance.logStreams.Add(&logOut);
        instance.isInitialized = true;
    }

    void Logger::InitToFile(const std::filesystem::path& logPath)
    {
        instance.logFile = std::fstream(logPath, std::ios::out | std::ios::trunc);
        WV_CHECK_MSG(instance.logFile.is_open(), "Failed to open log file: {}", logPath.string());

        Init(instance.logFile);
    }

    void Logger::AddStream(std::ostream& stream)
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (!instance.isInitialized)
            Init(stream);
        else
            instance.logStreams.Add(&stream);
    }

    Logger::MessageBuffer Logger::GetStrBuf()
    {
        std::lock_guard<std::mutex> lock(mutex);
        MessageBuffer pBuf = instance.sstreamPool.Get();

        if (pBuf.get() == nullptr)
            pBuf.reset(new std::stringstream());

        pBuf->clear();
        pBuf->str({});

        return pBuf;
    }

    void Logger::ReturnStrBuf(MessageBuffer&& buf)
    {
        std::lock_guard<std::mutex> lock(mutex);
        instance.sstreamPool.Return(std::move(buf));
    }

    Logger::Message Logger::Log(Level level)
    {
        if (GetIsLevelEnabled(level))
            return Message(level, GetStrBuf());
        else
            return Message();
    }

    const Logger::NullMessage& Logger::GetNullMessage() { return nullMsg; }

    constexpr bool Logger::GetIsLevelEnabled(Logger::Level level)
    {
        return (uint)level <= WV_LOG_LEVEL && (uint)level <= instance.logLevel;
    }

    void Logger::SetLogLevel(Logger::Level level)
    {
        std::lock_guard<std::mutex> lock(mutex);
        WV_CHECK_MSG(instance.isInitialized, "Logger must be initialized before setting runtime log level");
        instance.logLevel = (uint)level;
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

        stream << std::put_time(&tm_now, "[%Y-%m-%d][%H:%M:%S]");
    }

    Logger::Message::Message() :
        level(Level::Discard),
        pMsgBuf(nullptr)
    {}

    Logger::Message::Message(Level level, MessageBuffer&& pBuf) :
        level(level), 
        pMsgBuf(std::move(pBuf))
    { }

    Logger::Message::~Message()
    {
        if (pMsgBuf.get() != nullptr)
        {
            if (!pMsgBuf->view().empty())
                Logger::WriteLine(pMsgBuf->view());

            Logger::ReturnStrBuf(std::move(pMsgBuf));
        }
    }

    const char* Logger::Message::GetLevelName(Level level)
    {
        switch (level)
        {
        case Level::Info: return "INFO";
        case Level::Error: return "ERROR";
        case Level::Warning: return "WARN";
        case Level::Debug: return "DEBUG";
        default: return "UNKNOWN";
        }
    }
}