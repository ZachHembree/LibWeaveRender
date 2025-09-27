#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include "WeaveEffects/EffectParseException.hpp"
#include "WeaveUtils/Logger.hpp"
#include "WeaveUtils/GenericMain.hpp"
#include "WeaveUtils/Stopwatch.hpp"
#include "WeaveUtils/Compression.hpp"
#include "WeaveEffects/ShaderLibBuilder.hpp"
#include "WeaveEffects/ShaderDataSerialization.hpp"
#include "FXHelpText.hpp"

namespace fs = std::filesystem;
using namespace Weave;
using namespace Weave::Effects;

//-----------------------------------------------------------------------------
// Global Configuration Settings (controlled via command-line arguments)
//-----------------------------------------------------------------------------

// If true, the help page is shown
static bool shouldShowHelp = false;
// If true, enables debug information during shader compilation.
static bool isDebugging = false;
// If true, outputs the library as a C++ header file (.hpp) instead of binary (.bin).
static bool isHeaderLib = false;
// If true, merges all input files into a single output library file.
static bool isMerging = false;
// Specifies the target shader feature level (e.g., "5_0").
static string featureLevel;
// Specifies the output directory or file path.
static string outputDir;
// Specifies directory where the preprocessor should read/write cache files
static string cacheDir;
// Stores the set of input file paths to process.
static std::unordered_set<string> inputFiles;

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

// std::format formatting string for shader cache files
static constexpr string_view s_CacheFileFormat = "{}.cache";

// Default subfolder used when no cache directory is specified, relative to working directory.
static constexpr string_view s_DefaultCacheSubDir = "wfxc";

// Log file, relative to the working directory
static constexpr string_view s_LogFile = "wfxc.log";

/// <summary>
/// Finds all regular files with a specific extension within a directory.
/// </summary>
/// <param name="dir">The directory path to search within.</param>
/// <param name="ext">The desired file extension (including the dot, e.g., ".wfx").</param>
/// <param name="files">The set to which found file paths (as strings) will be added.</param>
static void GetFilesByExtension(const fs::path& dir, string_view ext, std::unordered_set<string>& files)
{
    // Ensure the directory exists before iterating
    if (!fs::exists(dir) || !fs::is_directory(dir))
    {
        WV_LOG_WARN() << "Directory not found or is not a directory: " << dir;
        return;
    }

    const auto& dirIt = fs::directory_iterator(dir);

    for (const fs::directory_entry& entry : dirIt)
    {
        if (entry.is_regular_file() && entry.path().extension() == ext)
            files.emplace(entry.path().string());
    }
}

/// <summary>
/// Helper to read the next argument specifically as a string value for an option.
/// Ensures the option is not specified more than once.
/// </summary>
/// <param name="args">The array of command-line arguments.</param>
/// <param name="pos">The current position index within the args array. Will be incremented.</param>
/// <param name="param">The string variable to store the parsed argument value.</param>
/// <exception cref="EffectParseException">If no argument follows, the next token is an option, or the option was already set.</exception>
static void SetStringParam(const IDynamicArray<string_view>& args, int& pos, string& param)
{
    int originalPos = pos;
    pos++;

    FX_CHECK_MSG(pos < args.GetLength() && (args[pos].empty() || args[pos][0] != '-'),
        "Expected argument after option '{}'", args[originalPos]);
    FX_CHECK_MSG(param.empty(),
        "Option '{}' specified more than once", args[originalPos]);

    param = args[pos];
}

//-----------------------------------------------------------------------------
// Command-line Option Handlers
//-----------------------------------------------------------------------------

// Sets help page flag
static void SetShowHelp(const IDynamicArray<string_view>& args, int& pos) { shouldShowHelp = true; }

// Sets the global flag to enable header library output.
static void SetHeaderLib(const IDynamicArray<string_view>& args, int& pos) { isHeaderLib = true; }

// Sets the global flag to enable debug compilation.
static void SetDebug(const IDynamicArray<string_view>& args, int& pos) { isDebugging = true; }

// Sets the global flag to enable merging of inputs.
static void SetMerge(const IDynamicArray<string_view>& args, int& pos) { isMerging = true; }

// Sets the global string for the target feature level using SetStringParam.
static void SetFeatureLevel(const IDynamicArray<string_view>& args, int& pos) { SetStringParam(args, pos, featureLevel); }

// Sets the global string for the output directory/file using SetStringParam.
static void SetOutput(const IDynamicArray<string_view>& args, int& pos) { SetStringParam(args, pos, outputDir); }

// Sets the global string for the cache directory/file using SetStringParam.
static void SetCache(const IDynamicArray<string_view>& args, int& pos) { SetStringParam(args, pos, cacheDir); }

/// <summary>
/// Sets the input file(s). Handles single files or wildcard patterns (*.ext).
/// </summary>
/// <param name="args">The array of command-line arguments.</param>
/// <param name="pos">The current position index within the args array. Will be incremented.</param>
/// <exception cref="EffectParseException">If no argument follows, or if wildcard pattern is malformed.</exception>
static void SetInput(const IDynamicArray<string_view>& args, int& pos)
{
    pos++;
    FX_CHECK_MSG(pos < args.GetLength() && (args[pos].empty() || args[pos][0] != '-'),
        "Expected argument after option '{}'", args[pos - 1]);

    string_view inPathStr = args[pos];
    fs::path inPath(inPathStr);
    string filename = inPath.filename().string(); // Use filesystem path methods

    // Check if the filename part contains a wildcard '*'
    if (filename.find('*') != string::npos)
    {
        // Extract the directory part and the extension part from the wildcard
        fs::path searchDir = inPath.has_parent_path() ? inPath.parent_path() : fs::current_path();
        size_t wildcardPos = filename.find('*');
        string ext = filename.substr(wildcardPos + 1); // Assumes format is *.ext

        FX_CHECK_MSG(!ext.empty() && ext[0] == '.', "Expected file extension after wildcard (e.g., '*.wfx'). Found: '{}'", ext);

        GetFilesByExtension(searchDir, ext, inputFiles);
    }
    else
    {
        // Treat as a single file path
        inputFiles.emplace(string(inPathStr)); // Store the original string representation
    }
}

/// Type alias for command-line option handler functions.
typedef void (*OptionHandlerFunc)(const IDynamicArray<string_view>& args, int& pos);

/// Maps single-character flags (e.g., '-d') to their handler functions.
static const std::unordered_map<char, OptionHandlerFunc> s_FlagMap
{
    { 'd', SetDebug },
    { 'h', SetHeaderLib },
    { 'm', SetMerge },
};

/// Maps long options (e.g., '--debug') to their handler functions.
static const std::unordered_map<string_view, OptionHandlerFunc> s_OptionMap
{
    { "help", SetShowHelp },
    { "debug", SetDebug },
    { "header", SetHeaderLib },
    { "merge", SetMerge },
    { "feature-level", SetFeatureLevel },
    { "input", SetInput },
    { "output", SetOutput },
    { "cache", SetCache }
};

//-----------------------------------------------------------------------------
// Output Generation Utilities
//-----------------------------------------------------------------------------

/// <summary>
/// Converts binary data from a stringstream into a C++ header file format
/// (constexpr uint64_t array).
/// </summary>
/// <param name="name">The base name for the C++ variable (e.g., "MyLibrary").</param>
/// <param name="binStream">The stringstream containing the binary data. It will be cleared
/// and overwritten with the header content.</param>
static void ConvertBinaryToHeader(string_view name, std::stringstream& binStream)
{
    static string binaryData;
    binaryData.clear();
    binaryData.append(binStream.view());
    binStream.str({});
    binStream.clear();

    static string varName;
    varName.clear();
    varName.append("s_FX_");
    varName.append(name);

    const size_t packedSize = (binaryData.size() + 7) / 8;
    binStream << "#include <cstdint>\n// Generated by WFXC\n";
    binStream << "constexpr uint64_t " << varName << "[" << packedSize << "] = {\n";

    for (size_t i = 0; i < (packedSize - 1); i++)
    {
        uint64_t value;
        memcpy(&value, &binaryData[8 * i], sizeof(uint64_t));

        binStream << "0x" << std::hex << value << ",";

        if (i > 0 && i % 5 == 0)
            binStream << std::endl;
    }

    const size_t trailingStart = 8 * (packedSize - 1);
    const size_t remBytes = (binaryData.size() - trailingStart);
    ulong lastValue = 0;
    memcpy(&lastValue, &binaryData[trailingStart], remBytes);

    binStream << "0x" << std::hex << lastValue << std::endl;
    binStream << "};";
}

/// <summary>
/// Reads the entire content of a specified file into a stringstream.
/// </summary>
/// <param name="inputPath">The path to the input file.</param>
/// <param name="ss">The stringstream to fill with the file content.</param>
/// <exception cref="EffectParseException">If the file doesn't exist, isn't a regular file, or cannot be opened.</exception>
static void GetInput(const fs::path& inputPath, std::stringstream& ss)
{
    ss.str({});
    ss.clear();

    FX_CHECK_MSG(fs::exists(inputPath), "Input path does not exist: {}", inputPath.string());
    FX_CHECK_MSG(fs::is_regular_file(inputPath), "Input path is not a regular file: {}", inputPath.string());

    std::ifstream inputStream(inputPath, std::ios::binary);

    FX_CHECK_MSG(inputStream.is_open(), "Failed to open input file: {}", inputPath.string());
    ss << inputStream.rdbuf();
}

/// <summary>
/// Ensures the directory structure for the output path exists, creating it if necessary.
/// </summary>
/// <param name="outputPath">The full path (including filename) for the output file.</param>
static void ValidateOutputDir(const fs::path& outputPath)
{
    fs::path parentDir = outputPath.parent_path();

    // Check if the parent path is non-empty and doesn't exist
    if (!parentDir.empty() && !fs::exists(parentDir))
    {
        WV_LOG_INFO() << "Attempting to create output directories: " << parentDir;

        try
        {
            if (fs::create_directories(parentDir))
                WV_LOG_INFO() << "Created output directories: " << parentDir;
            else
                WV_LOG_WARN() << "Could not create output directories (or they already existed): " << parentDir;
        }
        catch (const fs::filesystem_error& e)
        {
            FX_THROW("Filesystem error creating directory '{}': {}", parentDir.string(), e.what());
        }
    }

    FX_CHECK_MSG(!fs::is_directory(outputPath),
        "Output path specifies an existing directory, but a file is expected: {}", outputPath.string());
}

/// <summary>
/// Writes the content of a stringstream to the specified output file.
/// </summary>
/// <param name="outputPath">The path to the output file.</param>
/// <param name="ss">The stringstream containing the data to write.</param>
/// <exception cref="EffectParseException">If the output file cannot be opened for writing.</exception>
static void WriteBinary(const fs::path& outputPath, string_view data)
{
    ValidateOutputDir(outputPath); // Ensure parent directory exists

    std::ofstream dstFile(outputPath, std::ios::binary);
    FX_CHECK_MSG(dstFile.is_open(), "Failed to open output file for writing: {}", outputPath.string());

    dstFile << data; // Write the buffer to the file
}

//-----------------------------------------------------------------------------
// Core Library Processing Logic
//-----------------------------------------------------------------------------

/// <summary>
/// Creates a file path for the a library cache with the given name
/// </summary>
static fs::path GetCachePath(string_view libName)
{
    fs::path cachePath(cacheDir);
    string cacheName = std::format(s_CacheFileFormat, libName);
    return (cachePath / cacheName);
}

/// <summary>
/// Attempts to load the cache file corresponding to the given input, within the configured
/// cache directory, into the library builder.
/// </summary>
static void GetCache(string_view libName, std::stringstream& streamBuf, ShaderLibBuilder& libBuilder)
{
    fs::path cachePath = GetCachePath(libName);

    if (fs::exists(cachePath) && fs::is_regular_file(cachePath))
    {
        GetInput(cachePath, streamBuf);
        libBuilder.SetCache(GetDeserializedLibDef(streamBuf.view()));
    }
    else
        WV_LOG_INFO() << "No cache found for " << libName << ". Falling back to full compilation...";
}

/// <summary>
/// Finalizes the shader library, serializes it, optionally converts to a header,
/// and writes it to the specified output file.
/// </summary>
/// <param name="name">The base name for the library (used for header variable naming).</param>
/// <param name="libBuilder">The ShaderLibBuilder instance containing the compiled library data.</param>
/// <param name="streamBuf">A reusable stringstream buffer for serialization and writing.</param>
/// <param name="output">The path to the output file.</param>
static void WriteLibrary(string_view name, ShaderLibBuilder& libBuilder, std::stringstream& streamBuf,
    ZLibArchive& zipBuffer, fs::path output)
{
    // Get finished library definition
    libBuilder.SetName(name);
    ShaderLibDef::Handle shaderLib = libBuilder.GetDefinition();
    GetCompressedSerializedStream(shaderLib, zipBuffer, streamBuf);

    // Cache binary
    WriteBinary(GetCachePath(name), streamBuf.view());

    // Convert serialized binary data to a C++ header if requested
    if (isHeaderLib)
        ConvertBinaryToHeader(name, streamBuf);

    // Set the correct output file extension
    output.replace_extension(isHeaderLib ? ".hpp" : ".bin");

    // Write the final data (binary or header text) to the output file
    WriteBinary(output, streamBuf.view());

    // Log success and statistics
    WV_LOG_INFO() << "Wrote library to: " << output.native();

    auto descLog = WV_LOG_INFO();
    shaderLib.WriteDescriptionString(descLog);
    descLog << "\nCompression Ratio: " << 100.0f * zipBuffer.GetCompressionRatio() << "%";

    libBuilder.Clear();
}

static void ValidateConfiguration()
{
    // Set default feature level if not provided
    if (featureLevel.empty())
    {
        featureLevel = "5_0";
        WV_LOG_INFO() << "Using default feature level: " << featureLevel;
    }

    // Set default cache folder if one is not specified
    if (cacheDir.empty())
    {
        cacheDir = (fs::current_path() / s_DefaultCacheSubDir).string();
        WV_LOG_INFO() << "Using default cache directory: " << cacheDir;
    }
    else
        WV_LOG_INFO() << "Using custom cache directory: " << cacheDir;

    // Validate output path configuration based on merging status and number of inputs
    if (!outputDir.empty())
    {
        fs::path outPath(outputDir);

        if (inputFiles.size() > 1)
        {
            if (!isMerging)
            {
                // Multiple inputs, not merging: output must be a directory
                FX_CHECK_MSG(!fs::exists(outPath) || fs::is_directory(outPath),
                    "Multiple inputs require output path ('{}') to be a directory when not merging (--merge).", outputDir);
            }
            else
            {
                // Multiple inputs, merging: output must be a file path
                FX_CHECK_MSG(!fs::is_directory(outPath),
                    "Merging multiple inputs requires output path ('{}') to be a file, not a directory.", outputDir);
            }
        }
        // If single input, outPath can be a file or directory (handled later)
    }
    else if (isMerging && inputFiles.size() > 1)
        FX_THROW("Output file (--output <filepath>) must be specified when merging multiple input files.");

    if (isMerging && outputDir.empty())
        FX_THROW("Output path (--output <filepath>) is required when merging.");
}

/// <summary>
/// Main function to create the shader library/libraries based on parsed options.
/// Handles reading inputs, configuring the builder, processing files, and writing outputs.
/// </summary>
/// <exception cref="EffectParseException">If configuration is invalid (e.g., no input, invalid output path).</exception>
static void CreateLibrary()
{
    ShaderLibBuilder libBuilder;
    std::stringstream streamBuf;
    ZLibArchive zipBuffer;
    fs::path outPath(outputDir);

    // Configure the library builder
    libBuilder.SetFeatureLevel(featureLevel);
    libBuilder.SetDebug(isDebugging);

    Stopwatch timer;
    timer.Start();

    // Use shared caching
    if (isMerging)
        GetCache(outPath.stem().string(), streamBuf, libBuilder);

    // Process each input file
    for (const string& inputFileStr : inputFiles)
    {
        fs::path inFile(inputFileStr);
        WV_LOG_INFO() << "Processing input file: " << inFile.native();

        // Read input file content into the buffer
        GetInput(inFile, streamBuf);

        string baseName = inFile.stem().string(); // Base name for library/variable naming
        string inputPathString = inFile.string(); // Full path string for builder context

        // Use per-input caching
        if (!isMerging)
            GetCache(baseName, streamBuf, libBuilder);

        // Preprocess and add to library builder
        libBuilder.AddRepo(inputPathString, streamBuf.view());

        // If not merging, write out a separate library file for each input
        if (!isMerging)
        {
            fs::path currentOutFile;

            if (outputDir.empty()) // No output dir specified, place output next to input
                currentOutFile = inFile;
            else // Output dir specified
            {
                if (fs::is_directory(outPath)) // Write into the directory using the input filename
                    currentOutFile = outPath / inFile.filename();
                else // Output path is a file (only allowed for single input implicitly)
                    currentOutFile = outPath;
            }

            WriteLibrary(baseName, libBuilder, streamBuf, zipBuffer, currentOutFile);
        }
    }

    // If merging, write the single combined library after processing all inputs
    if (isMerging)
    {
        // Use output filename stem for name
        string mergedName = outPath.stem().string();
        WriteLibrary(mergedName, libBuilder, streamBuf, zipBuffer, outPath);
    }

    timer.Stop();
    WV_LOG_INFO() << "Total processing time: " << timer.GetElapsedMS() << " ms";
}

/// <summary>
/// Parses command-line arguments using the defined flag and option maps.
/// </summary>
/// <exception cref="EffectParseException">If an unknown option or flag is encountered, or if arguments are malformed.</exception>
static void HandleOptions(const IDynamicArray<string_view>& args)
{
    FX_CHECK_MSG(args.GetLength() > 1, "No arguments provided");

    for (int i = 1; i < args.GetLength(); ++i)
    {
        const string_view& arg = args[i];

        if (!arg.empty() && arg[0] == '-')
        {
            if (arg.size() > 1)
            {
                if (arg[1] == '-') // Long option (--option)
                {
                    string_view opt = arg.substr(2);
                    const auto& it = s_OptionMap.find(opt);
                    FX_CHECK_MSG(it != s_OptionMap.end(), "Unknown option: {}", arg);
                    const OptionHandlerFunc OptHandler = it->second;
                    OptHandler(args, i);
                }
                else // Short flag(s) (-f or -abc)
                {
                    string_view flags = arg.substr(1);

                    for (char flag : flags)
                    {
                        const auto& it = s_FlagMap.find(flag);
                        FX_CHECK_MSG(it != s_FlagMap.end(), "Unknown flag: -{}", flag);
                        const OptionHandlerFunc FlagHandler = it->second;
                        FlagHandler(args, i);
                    }
                }
            }
            else
                FX_THROW("Invalid argument: '-'");
        }
        else // Not an option/flag
        {
            if (inputFiles.empty()) // Assume first non-option is input (potentially a single file)
                inputFiles.emplace(string(arg));
            else if (outputDir.empty()) // Assume second non-option is output directory/file
                outputDir = arg;
            else
                FX_THROW("Unexpected positional argument: {}. Use --input and --output options.", arg);
        }
    }

    if (!shouldShowHelp)
        FX_CHECK_MSG(!inputFiles.empty(), "No input files specified. Use --input <file> or --input <*.ext>.");
}

// Converts c-string arguments into dynamic array of string_views
static DynamicArray<string_view> GetArgs(int argc, char* argv[])
{
    DynamicArray<string_view> args(argc);

#ifdef NDEBUG
    for (int i = 0; i < argc; ++i)
        args[i] = string_view(argv[i]);

#else // Debug Build - Use hardcoded defaults if no args provided
    if (argc == 1)
    {
        args =
        {
            string_view(argv[0]),
            "--input", "*.wfx",
            "--output", "output_debug",
            "--feature-level", "5_0",
            "-hm",
        };
    }
    else
    {
        // Use actual command line args if provided
        for (int i = 0; i < argc; ++i)
            args[i] = string_view(argv[i]);
    }

#endif

    return args;
}

/// <summary>
/// Main control function for CLI execution.
/// Handles argument parsing, logger initialization, and library execution.
/// Uses GenericMain() to wrap each major stage for consistent exception handling.
/// </summary>
/// <param name="args">Pre-parsed string_view arguments.</param>
/// <returns>Exit code representing success or failure.</returns>
static int RunCLI(const IDynamicArray<string_view>& args)
{
    // Handle arguments
    int exitCode = 0;
    const GenericMainT<const IDynamicArray<string_view>&> OptionFunc = HandleOptions;
    exitCode = GenericMain(std::cerr, OptionFunc, args);

    if (exitCode != 0)
        shouldShowHelp = true; // Incorrect arguments

    if (shouldShowHelp) // Early exit
    {
        std::cout << g_FX_HelpText;
    }
    else // Normal startup
    {
        // Initialize logging
        const GenericMainT<const fs::path&> LogFunc = [](const fs::path& logPath)
        {
            // Initialize logging to file and console
            Logger::InitToFile(logPath);
            Logger::AddStream([](string_view str) { std::cout << str; }, true);
        };

        const fs::path logPath = (fs::current_path() / s_LogFile);
        exitCode = GenericMain(std::cerr, LogFunc, logPath);

        if (exitCode != 0)
            return exitCode; // Log init failed

        // Preprocess shaders
        const GenericMainT<const IDynamicArray<string_view>&> LibFunc = [](const IDynamicArray<string_view>& args)
        {
            WV_LOG_INFO() << "WFXC " << VERSION_STRING;
            WV_LOG_INFO() << "Working Directory: " << fs::current_path().string();

            {
                // Log the command line used to invoke the program
                auto cmdLine = WV_LOG_INFO();
                cmdLine << "Command Line: ";
                for (string_view arg : args) { cmdLine << arg << " "; }
            }

            ValidateConfiguration();
            CreateLibrary();
            WV_LOG_INFO() << "Processing completed successfully.";
        };

        auto logErr = WV_LOG_ERROR();
        exitCode = GenericMain(logErr, LibFunc, args);
    }

    return exitCode;
}

/// <summary>
/// Application entry point.
/// Initializes the argument list and delegates execution to the CLI runner.
/// </summary>
/// <param name="argc">Argument count.</param>
/// <param name="argv">Argument values.</param>
/// <returns>Process exit code.</returns>
int main(int argc, char* argv[])
{
    const DynamicArray<string_view> args = GetArgs(argc, argv);
    const int exitCode = RunCLI(args);

#ifndef NDEBUG
    (void)std::getchar();
#endif

    return exitCode;
}