#include "pch.hpp"
#include <condition_variable>
#include "WeaveUtils/Logger.hpp"

namespace Weave
{
    Logger Logger::s_Instance;

    static const Logger::NullMessage s_NullMsg = {};
    static std::atomic<bool> s_IsLogInitialized(false);
    static std::atomic<bool> s_CanPoll(false);
    static std::atomic<uint> s_LogLevel(WV_LOG_LEVEL);
    static std::condition_variable s_IsBufferWritePending;

    static std::mutex s_MsgPoolMutex;
    static std::mutex s_WriteMutex;

    /// <summary>
    /// Private constructor: Initializes logger state.
    /// Sets the initial runtime log level based on the compile-time WV_LOG_LEVEL.
    /// Initializes the message history buffer.
    /// </summary>
    Logger::Logger() :
        msgHistory(10),
        historyIndex(0),
        sstreamPool() 
    { }

    /// <summary>
    /// Destructor: Ensures any buffered log messages are written to the buffered streams.
    /// </summary>
    Logger::~Logger()
    {
        s_IsLogInitialized = false;
        s_CanPoll = false;

        if (!logBuffer.view().empty())
        {
            try 
            {
                FlushLogBuffer();
            }
            catch (const std::exception& e) 
            {
                std::cerr << "Error flushing log buffer during Logger destruction: " << e.what() << std::endl;
            }
            catch (...) 
            {
                std::cerr << "Unknown error flushing log buffer during Logger destruction." << std::endl;
            }
        }
    }

    Logger::Logger(Logger&& other) noexcept = default;

    Logger& Logger::operator=(Logger&&) noexcept = default;

    /// <summary>
    /// Returns whether the logger has been initialized.
    /// </summary>
    bool Logger::GetIsInitialized() { return s_IsLogInitialized; }

    /// <summary>
    /// Initializes the logger to output to a file. Thread-safe.
    /// </summary>
    void Logger::InitToFile(const std::filesystem::path& logPath)
    {
        WV_CHECK_MSG(!s_IsLogInitialized, "Tried to initialize logger twice.");

        AddStream([](string_view str)
        {
            std::fstream& log = s_Instance.logFile;

            if (log.rdstate() == std::ios::goodbit)
            {
                log << str;
                log.flush();
            }
        });

        s_Instance.logFile.open(logPath, std::ios::out | std::ios::trunc);
        WV_CHECK_MSG(s_Instance.logFile.is_open(), "Failed to open log file: {}", logPath.string());
    }

    /// <summary>
    /// Adds the given callback and initializes the logger if not yet initialized. Thread-safe.
    /// </summary>
    void Logger::AddStream(LogWriteCallback logOutFunc, bool fast)
    {
        std::lock_guard<std::mutex> lock(s_WriteMutex);

        if (fast)
            s_Instance.logWriteFast.Add(logOutFunc);
        else
            s_Instance.logWriteDeferred.Add(logOutFunc);

        s_IsLogInitialized = true;

        if (!s_Instance.logWriteDeferred.IsEmpty() && !s_CanPoll)
            StartPolling();
    }

    void Logger::StartPolling()
    {
        if (s_IsLogInitialized && !s_CanPoll)
        {
            s_CanPoll = true;
            s_Instance.pollThread = std::jthread([]
            {
                while (s_CanPoll)
                {
                    std::unique_lock<std::mutex> lock(s_WriteMutex);
                    // Block polling until a write is pending and the minimum time has elapsed
                    s_IsBufferWritePending.wait_for(lock, std::chrono::milliseconds(WV_LOG_TIME_MS));
                    // s_WriteMutex lock reacquired
                    // Flush buffered logs
                    s_Instance.FlushLogBuffer();
                }
            });
        }
    }

    /// <summary>
    /// Writes contents of the log buffer to deferred write callbacks and clears the buffer.
    /// Not thread safe. s_WriteMutex must be acquired externally before calling.
    /// </summary>
    void Logger::FlushLogBuffer()
    {
        std::string_view bufferView = logBuffer.view();

        if (bufferView.empty())
            return;

        for (LogWriteCallback WriteFunc : logWriteDeferred)
        {
            try 
            {
                WriteFunc(bufferView);
            }
            catch (const std::exception& e) 
            {
                std::cerr << "Logger: Exception in deferred LogWriteCallback: " << e.what() << std::endl;
            }
            catch (...) 
            {
                std::cerr << "Logger: Unknown exception in deferred LogWriteCallback." << std::endl;
            }
        }

        logBuffer.str({});
        logBuffer.clear();
    }

    /// <summary>
    /// Internal method to write a complete log message. Handles timestamping, formatting,
    /// duplicate checking, buffering, and writing to streams. Thread-safe.
    /// </summary>
    void Logger::WriteToLog(Level level, std::string_view message)
    {
        WV_ASSERT_MSG(s_IsLogInitialized, "Cannote write to an uninitialized logger.");

        if (message.empty() || level == Level::Discard)
            return;        

        // Check for duplicates only if the message is considered "new"
        if (s_Instance.TryBufferLog(message))
        {
            std::lock_guard<std::mutex> lock(s_WriteMutex);
            s_Instance.msgBuffer.str({});
            s_Instance.msgBuffer.clear();

            // Add timestamp and level prefix
            AddTimestamp(s_Instance.msgBuffer);
            s_Instance.msgBuffer << '[' << GetLevelName(level) << "] " << message << std::endl;

            // Write immediately to fast streams
            std::string_view formattedMessage = s_Instance.msgBuffer.view();

            for (LogWriteCallback WriteFunc : s_Instance.logWriteFast)
                WriteFunc(formattedMessage);

            // Write to buffer for slow streams
            if (!s_Instance.logWriteDeferred.IsEmpty())
            {
                s_Instance.logBuffer << formattedMessage;
                s_IsBufferWritePending.notify_one();
            }
        }
    }

    /// <summary>
    /// Retrieves a stringstream buffer from the pool. Thread-safe.
    /// </summary>
    Logger::MessageBuffer Logger::GetStrBuf()
    {
        MessageBuffer pBuf;
        {
            std::lock_guard<std::mutex> lock(s_MsgPoolMutex);
            pBuf = s_Instance.sstreamPool.Get();
        }

        if (!pBuf)
            pBuf = std::make_unique<std::stringstream>();
        else 
        {
            pBuf->str({});
            pBuf->clear();
        }

        return pBuf;
    }

    /// <summary>
    /// Returns a stringstream buffer to the pool. Thread-safe.
    /// </summary>
    void Logger::ReturnStrBuf(MessageBuffer&& buf)
    {
        if (!buf) return;
        std::lock_guard<std::mutex> lock(s_MsgPoolMutex);
        s_Instance.sstreamPool.Return(std::move(buf));
    }

    /// <summary>
    /// Checks the message history for duplicates. Adds new messages. Thread-safe.
    /// </summary>
    bool Logger::TryBufferLog(std::string_view message)
    {
        static std::mutex bufferMutex;
        std::lock_guard<std::mutex> lock(bufferMutex);

        bool isNew = true;
        size_t historyLen = msgHistory.GetLength();

        for (size_t i = 0; i < historyLen; ++i)
        {
            if (msgHistory[i] == message)
            {
                isNew = false;
                break;
            }
        }

        if (isNew && historyLen > 0)
        {
            string& hist = msgHistory[historyIndex];
            hist.clear();
            hist.append(message);
            historyIndex = (historyIndex + 1) % historyLen;
        }
        
        return isNew;
    }

    /// <summary>
    /// Factory method for creating Log::Message objects. Thread-safe.
    /// </summary>
    Logger::Message Logger::Log(Level level)
    {
        if (GetIsLevelEnabled(level) || !s_IsLogInitialized)
            return Message(level, GetStrBuf());
        else
            return Message();
    }

    /// <summary>
    /// Returns the shared NullMessage s_Instance.
    /// </summary>
    const Logger::NullMessage& Logger::GetNullMessage() { return s_NullMsg; }

    /// <summary>
    /// Checks if a log level is enabled (compile-time and runtime). Thread-safe.
    /// </summary>
    bool Logger::GetIsLevelEnabled(Logger::Level level)
    {
        const uint currentRuntimeLevel = s_LogLevel;
        return (static_cast<uint>(level) <= WV_LOG_LEVEL) && (static_cast<uint>(level) <= currentRuntimeLevel);
    }

    /// <summary>
    /// Sets the runtime log level filter. Thread-safe.
    /// </summary>
    void Logger::SetLogLevel(Logger::Level level) { s_LogLevel = static_cast<uint>(level); }

    /// <summary>
    /// Adds a timestamp to the stream.
    /// </summary>
    void Logger::AddTimestamp(std::ostream& stream)
    {
        // Assumes lock is held by caller (WriteToLog)
        const auto now = std::chrono::system_clock::now();
        const auto time_t_now = std::chrono::system_clock::to_time_t(now);
        std::tm tm_now;

        // Use platform-specific thread-safe variants of localtime
#ifdef _WIN32
        localtime_s(&tm_now, &time_t_now);
#else
        localtime_r(&time_t_now, &tm_now); // POSIX thread-safe version
#endif

        // Format: [YYYY-MM-DD][HH:MM:SS]
        stream << std::put_time(&tm_now, "[%Y-%m-%d][%H:%M:%S]");
    }

    /// <summary>
    /// Gets the string name for a log level. Thread-safe (stateless).
    /// </summary>
    std::string_view Logger::GetLevelName(Level level)
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

    // --- Logger::Message Method Implementations ---

    /// <summary>
    /// Default constructor for a discarded message.
    /// </summary>
    Logger::Message::Message() :
        level(Level::Discard),
        pMsgBuf(nullptr)
    { }

    /// <summary>
    /// Constructor for an active log message, takes ownership of the buffer.
    /// </summary>
    Logger::Message::Message(Level level, MessageBuffer&& pBuf) :
        level(level),
        pMsgBuf(std::move(pBuf))
    { }

    /// <summary>
    /// Destructor: Writes the message to the log if applicable and returns the buffer.
    /// </summary>
    Logger::Message::~Message()
    {
        if (pMsgBuf)
        {
            std::string_view messageContent = pMsgBuf->view();

            if (!messageContent.empty())
            {
                // WriteToLog handles thread safety, timestamping, etc.
                Logger::WriteToLog(level, messageContent);
            }

            Logger::ReturnStrBuf(std::move(pMsgBuf));
        }
    }

    /// <summary>
    /// Streams wide string view, converting to UTF-8
    /// </summary>
    Logger::Message& Logger::Message::operator<<(std::wstring_view value)
    {
        if (level != Level::Discard && pMsgBuf)
        {
            static thread_local string utf8ConvBuffer;
            GetMultiByteString_UTF16LE_TO_UTF8(value, utf8ConvBuffer);
            *pMsgBuf << utf8ConvBuffer.data();
        }

        return *this;
    }

    Logger::Message& Logger::Message::operator<<(wchar_t* pValue) { return operator<<(std::wstring_view(pValue)); }

    Logger::Message& Logger::Message::operator<<(const std::wstring& value) { return operator<<(std::wstring_view(value)); }
}