#include "pch.hpp"
#include "WeaveUtils/Logger.hpp"

namespace Weave
{
    // --- Static Member Definitions ---
    Logger Logger::instance;
    const Logger::NullMessage Logger::nullMsg = {};
    std::mutex Logger::mutex;

    /// <summary>
    /// Private constructor: Initializes logger state.
    /// Sets the initial runtime log level based on the compile-time WV_LOG_LEVEL.
    /// Initializes the message history buffer.
    /// </summary>
    Logger::Logger() :
        isInitialized(false),
        logLevel(WV_LOG_LEVEL),
        msgHistory(10),
        historyIndex(0),
        sstreamPool() 
    { }

    /// <summary>
    /// Destructor: Ensures any buffered log messages are written to the buffered streams.
    /// </summary>
    Logger::~Logger()
    {
        if (isInitialized && !logBuffer.view().empty())
        {
            try {
                FlushLogBuffer();
            }
            catch (const std::exception& e) {
                std::cerr << "Error flushing log buffer during Logger destruction: " << e.what() << std::endl;
            }
            catch (...) {
                std::cerr << "Unknown error flushing log buffer during Logger destruction." << std::endl;
            }
        }
    }

    /// <summary>
    /// Returns whether the logger has been initialized. Thread-safe.
    /// </summary>
    bool Logger::GetIsInitialized()
    {
        std::lock_guard<std::mutex> lock(mutex);
        return instance.isInitialized;
    }

    /// <summary>
    /// Initializes the logger with a stream. Thread-safe.
    /// </summary>
    void Logger::Init(std::ostream& logOut, bool fast)
    {
        std::lock_guard<std::mutex> lock(mutex);
        WV_CHECK_MSG(!instance.isInitialized, "Tried to initialize logger twice.");

        if (fast)
            instance.logStreamsFast.Add(&logOut);
        else
            instance.bufferedLogStreams.Add(&logOut);

        instance.isInitialized = true;
        instance.flushTimer.Restart(); // Start the flush timer
    }

    /// <summary>
    /// Initializes the logger to output to a file. Thread-safe.
    /// </summary>
    void Logger::InitToFile(const std::filesystem::path& logPath)
    {
        std::lock_guard<std::mutex> lock(mutex);
        WV_CHECK_MSG(!instance.isInitialized, "Tried to initialize logger twice.");

        instance.logFile.open(logPath, std::ios::out | std::ios::trunc);
        WV_CHECK_MSG(instance.logFile.is_open(), "Failed to open log file: {}", logPath.string());

        instance.bufferedLogStreams.Add(&instance.logFile);
        instance.isInitialized = true;
        instance.flushTimer.Restart(); // Start the flush timer
    }

    /// <summary>
    /// Adds an additional output stream. Thread-safe.
    /// </summary>
    void Logger::AddStream(std::ostream& stream, bool fast)
    {
        std::lock_guard<std::mutex> lock(mutex);

        if (!instance.isInitialized)
        {
            if (fast)
                instance.logStreamsFast.Add(&stream);
            else
                instance.bufferedLogStreams.Add(&stream);

            instance.isInitialized = true;
            instance.flushTimer.Restart();
        }
        else
        {
            if (fast)
                instance.logStreamsFast.Add(&stream);
            else
                instance.bufferedLogStreams.Add(&stream);
        }
    }

    /// <summary>
    /// Internal method to write a complete log message. Handles timestamping, formatting,
    /// duplicate checking, buffering, and writing to streams. Thread-safe.
    /// </summary>
    void Logger::WriteToLog(Level level, std::string_view message)
    {
        if (message.empty() || level == Level::Discard)
            return;

        std::lock_guard<std::mutex> lock(mutex);

        // Check for duplicates only if the message is considered "new"
        if (instance.TryBufferLog(message))
        {
            // Clear and reuse the temporary message buffer
            instance.msgBuffer.str({});
            instance.msgBuffer.clear();

            // Add timestamp and level prefix
            AddTimestamp(instance.msgBuffer);
            instance.msgBuffer << '[' << GetLevelName(level) << "] " << message << std::endl;

            // Write immediately to fast streams
            std::string_view formattedMessage = instance.msgBuffer.view();

            for (std::ostream* pOut : instance.logStreamsFast)
            {
                if (pOut && pOut->good()) // Check stream pointer and state
                {
                    (*pOut) << formattedMessage << std::flush;
                }
                // TODO: Consider handling bad streams (e.g., remove them?)
            }

            // Append to the main log buffer for buffered streams
            instance.logBuffer << formattedMessage;

            if (instance.flushTimer.GetElapsedS() >= WV_LOG_TIME_S)
                instance.FlushLogBuffer(); // FlushLogBuffer already handles locking and timer reset
        }
    }

    /// <summary>
    /// Retrieves a stringstream buffer from the pool. Thread-safe.
    /// </summary>
    Logger::MessageBuffer Logger::GetStrBuf()
    {
        std::lock_guard<std::mutex> lock(mutex);
        MessageBuffer pBuf = instance.sstreamPool.Get();

        if (!pBuf)
        {
            pBuf = std::make_unique<std::stringstream>();
        }
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
        std::lock_guard<std::mutex> lock(mutex);
        instance.sstreamPool.Return(std::move(buf));
    }


    /// <summary>
    /// Checks the message history for duplicates. Adds new messages. Not thread-safe itself, requires external locking.
    /// </summary>
    bool Logger::TryBufferLog(std::string_view message)
    {
        bool isNew = true;
        size_t historyLen = msgHistory.GetLength();

        for (size_t i = 0; i < historyLen; ++i)
        {
            if (msgHistory[i].view() == message)
            {
                isNew = false;
                break;
            }
        }

        if (isNew && historyLen > 0)
        {
            std::stringstream& hist = msgHistory[historyIndex];
            hist.str({});
            hist.clear();
            hist << message;

            historyIndex = (historyIndex + 1) % historyLen;
        }
        
        return isNew;
    }

    /// <summary>
    /// Flushes the main log buffer to buffered streams. Not thread-safe itself, requires external locking.
    /// </summary>
    void Logger::FlushLogBuffer()
    {
        std::string_view bufferView = logBuffer.view();

        if (bufferView.empty())
            return;

        for (std::ostream* pOut : bufferedLogStreams)
        {
            if (pOut && pOut->good())
            {
                (*pOut) << bufferView << std::flush;
            }
            // TODO: Consider handling bad streams (e.g., remove them?)
        }

        logBuffer.str({});
        logBuffer.clear();
        flushTimer.Restart();
    }


    /// <summary>
    /// Factory method for creating Log::Message objects. Thread-safe.
    /// </summary>
    Logger::Message Logger::Log(Level level)
    {
        // GetIsLevelEnabled handles locking for reading runtime level
        if (GetIsLevelEnabled(level))
        {
            // GetStrBuf handles locking for pool access
            return Message(level, GetStrBuf());
        }
        else
        {
            // Return a default (discard) message - no buffer needed.
            return Message();
        }
    }

    /// <summary>
    /// Returns the shared NullMessage instance. Thread-safe (returns const ref to static).
    /// </summary>
    const Logger::NullMessage& Logger::GetNullMessage()
    {
        return nullMsg;
    }

    /// <summary>
    /// Checks if a log level is enabled (compile-time and runtime). Thread-safe.
    /// </summary>
    constexpr bool Logger::GetIsLevelEnabled(Logger::Level level)
    {
        const uint currentRuntimeLevel = instance.logLevel;
        return (static_cast<uint>(level) <= WV_LOG_LEVEL) && (static_cast<uint>(level) <= currentRuntimeLevel);
    }

    /// <summary>
    /// Sets the runtime log level filter. Thread-safe.
    /// </summary>
    void Logger::SetLogLevel(Logger::Level level) { instance.logLevel = static_cast<uint>(level); }

    /// <summary>
    /// Adds a timestamp to the stream. Not thread-safe itself, requires external locking if stream is shared.
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
    /// Streams wide string view, converting to UTF-8. Requires external lock if utf8ConvBuffer is contended.
    /// </summary>
    Logger::Message& Logger::Message::operator<<(std::wstring_view value)
    {
        if (level != Level::Discard && pMsgBuf)
        {
            std::lock_guard<std::mutex> lock(Logger::mutex);
            GetMultiByteString_UTF16LE_TO_UTF8(value, instance.utf8ConvBuffer);
            *pMsgBuf << instance.utf8ConvBuffer.data();
        }
        return *this;
    }

    /// <summary>
    /// Streams wide C-string via the wstring_view overload.
    /// </summary>
    Logger::Message& Logger::Message::operator<<(wchar_t* pValue)
    {
        return operator<<(std::wstring_view(pValue));
    }

    /// <summary>
    /// Streams wide string via the wstring_view overload.
    /// </summary>
    Logger::Message& Logger::Message::operator<<(const std::wstring& value)
    {
        return operator<<(std::wstring_view(value));
    }
}