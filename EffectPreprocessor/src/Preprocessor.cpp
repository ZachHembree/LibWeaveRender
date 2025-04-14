#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include "ReplicaEffects/EffectParseException.hpp"
#include "ReplicaUtils/Logger.hpp"
#include "ReplicaEffects/ShaderLibBuilder.hpp"
#include "FXHelpText.hpp"

namespace fs = std::filesystem;

using namespace Replica;
using namespace Replica::Effects;

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
// Stores the set of input file paths to process.
static std::unordered_set<string> inputFiles;

/**
 * @brief Finds all regular files with a specific extension within a directory.
 * @param dir The directory path to search within.
 * @param ext The desired file extension (including the dot, e.g., ".rpfx").
 * @param files[out] The set to which found file paths (as strings) will be added.
 */
static void GetFilesByExtension(const fs::path& dir, string_view ext, std::unordered_set<string>& files)
{
    // Ensure the directory exists before iterating
    if (!fs::exists(dir) || !fs::is_directory(dir)) 
    {
        LOG_WARN() << "Directory not found or is not a directory: " << dir;
        return;
    }

    const auto& dirIt = fs::directory_iterator(dir);

    for (const fs::directory_entry& entry : dirIt)
    {
        if (entry.is_regular_file() && entry.path().extension() == ext)
            files.emplace(entry.path().string());
    }
}

/**
 * @brief Helper to read the next argument specifically as a string value for an option.
 * Ensures the option is not specified more than once.
 * @param args The array of command-line arguments.
 * @param pos[in,out] The current position index within the args array. Will be incremented.
 * @param param[out] The string variable to store the parsed argument value.
 * @throws EffectParseException If no argument follows, the next token is an option, or the option was already set.
 */
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

/**
 * @brief Sets the input file(s). Handles single files or wildcard patterns (*.ext).
 * @param args The array of command-line arguments.
 * @param pos[in,out] The current position index within the args array. Will be incremented.
 * @throws EffectParseException If no argument follows, or if wildcard pattern is malformed.
 */
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

        FX_CHECK_MSG(!ext.empty() && ext[0] == '.', "Expected file extension after wildcard (e.g., '*.rpfx'). Found: '{}'", ext);

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
    { "output", SetOutput }
};

//-----------------------------------------------------------------------------
// Output Generation Utilities
//-----------------------------------------------------------------------------

/**
 * @brief Writes a single byte to an output stream in hexadecimal format (0xHH).
 * @param value The character (byte) value to write.
 * @param ss The output stream.
 * @return The output stream.
 */
static std::ostream& WriteCharHex(char value, std::ostream& ss)
{
    return ss << "0x" << std::hex << (int)(unsigned char)value;
}

/**
 * @brief Converts binary data from a stringstream into a C++ header file format
 * (constexpr unsigned char array).
 * @param name The base name for the C++ variable (e.g., "MyLibrary").
 * @param binStream[in,out] The stringstream containing the binary data. It will be cleared
 * and overwritten with the header content.
 */
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

    binStream << "// Generated by RPFX Preprocessor\n";
    binStream << "constexpr unsigned char " << varName << "[" << binaryData.size() << "] = {\n";

    for (size_t i = 0; i < binaryData.size() - 1; ++i)
    {
        WriteCharHex(binaryData[i], binStream) << ",";

        if (i > 0 && i % 20 == 0)
            binStream << std::endl;
    }

    WriteCharHex(binaryData.back(), binStream) << std::endl;
    binStream << "};";
}

/**
 * @brief Reads the entire content of a specified file into a stringstream.
 * @param inputPath The path to the input file.
 * @param ss[out] The stringstream to fill with the file content.
 * @throws EffectParseException If the file doesn't exist, isn't a regular file, or cannot be opened.
 */
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

/**
 * @brief Ensures the directory structure for the output path exists, creating it if necessary.
 * @param outputPath The full path (including filename) for the output file.
 */
static void ValidateOutputDir(const fs::path& outputPath)
{
    fs::path parentDir = outputPath.parent_path();

    // Check if the parent path is non-empty and doesn't exist
    if (!parentDir.empty() && !fs::exists(parentDir))
    {
        LOG_INFO() << "Attempting to create output directories: " << parentDir;

        try 
        {
            if (fs::create_directories(parentDir))
                LOG_INFO() << "Created output directories: " << parentDir;
            else
                LOG_WARN() << "Could not create output directories (or they already existed): " << parentDir;
        }
        catch (const fs::filesystem_error& e) 
        {
            FX_THROW("Filesystem error creating directory '{}': {}", parentDir.string(), e.what());
        }
    }

    FX_CHECK_MSG(!fs::is_directory(outputPath), 
        "Output path specifies an existing directory, but a file is expected: {}", outputPath.string());
}

/**
 * @brief Writes the content of a stringstream to the specified output file.
 * @param outputPath: The path to the output file.
 * @param ss: The stringstream containing the data to write.
 * @throws EffectParseException If the output file cannot be opened for writing.
 */
static void WriteBinary(const fs::path& outputPath, const std::stringstream& ss)
{
    ValidateOutputDir(outputPath); // Ensure parent directory exists

    std::ofstream dstFile(outputPath, std::ios::binary);
    FX_CHECK_MSG(dstFile.is_open(), "Failed to open output file for writing: {}", outputPath.string());

    dstFile << ss.rdbuf(); // Write the stringstream buffer to the file
}

//-----------------------------------------------------------------------------
// Core Library Processing Logic
//-----------------------------------------------------------------------------

/**
 * @brief Finalizes the shader library, serializes it, optionally converts to a header,
 * and writes it to the specified output file.
 * @param name: The base name for the library (used for header variable naming).
 * @param libBuilder: The ShaderLibBuilder instance containing the compiled library data.
 * @param streamBuf: A reusable stringstream buffer for serialization and writing.
 * @param output: The path to the output file.
 */
static void WriteLibrary(string_view name, ShaderLibBuilder& libBuilder, std::stringstream& streamBuf, const fs::path& output)
{
    ShaderLibDef::Handle shaderLib = libBuilder.GetDefinition();
    streamBuf.str({});
    streamBuf.clear();

    // Serialize the library definition into the stringstream buffer
    Serializer libWriter(streamBuf);
    libWriter(shaderLib);

    // Convert serialized binary data to a C++ header if requested
    if (isHeaderLib)
        ConvertBinaryToHeader(name, streamBuf);

    // Write the final data (binary or header text) to the output file
    WriteBinary(output, streamBuf);

    // Log success and statistics
    LOG_INFO() << "Successfully wrote library to: " << output;
    LOG_INFO() << "  Library Stats:";
    LOG_INFO() << "    Shaders:   " << (shaderLib.regHandle.pShaders ? shaderLib.regHandle.pShaders->GetLength() : 0);
    LOG_INFO() << "    Effects:   " << (shaderLib.regHandle.pEffects ? shaderLib.regHandle.pEffects->GetLength() : 0);
    LOG_INFO() << "    Constants: " << (shaderLib.regHandle.pConstants ? shaderLib.regHandle.pConstants->GetLength() : 0);
    LOG_INFO() << "    Resources: " << (shaderLib.regHandle.pResources ? shaderLib.regHandle.pResources->GetLength() : 0);

    if (shaderLib.pPlatform) 
    {
        LOG_INFO() << "  Platform Info:";
        LOG_INFO() << "    Compiler:     " << shaderLib.pPlatform->compilerVersion;
        LOG_INFO() << "    Feature Level:" << shaderLib.pPlatform->featureLevel;
    }
    else
        LOG_WARN() << "  Platform info not available in ShaderLibDef.";

    libBuilder.Clear();
}

/**
 * @brief Main function to create the shader library/libraries based on parsed options.
 * Handles reading inputs, configuring the builder, processing files, and writing outputs.
 * @throws EffectParseException If configuration is invalid (e.g., no input, invalid output path).
 */
static void CreateLibrary()
{
    FX_CHECK_MSG(!inputFiles.empty(), "No input files specified. Use --input <file> or --input <*.ext>.");

    ShaderLibBuilder libBuilder;
    std::stringstream streamBuf;

    // Set default feature level if not provided
    if (featureLevel.empty()) 
    {
        featureLevel = "5_0";
        LOG_INFO() << "Using default feature level: " << featureLevel;
    }

    // Configure the library builder
    libBuilder.SetFeatureLevel(featureLevel);
    libBuilder.SetDebug(isDebugging);

    fs::path outPath;

    // Validate output path configuration based on merging status and number of inputs
    if (!outputDir.empty()) 
    {
        outPath = fs::path(outputDir);

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

    Stopwatch timer;
    timer.Start();

    // Process each input file
    for (const string& inputFileStr : inputFiles)
    {
        fs::path inFile(inputFileStr);
        LOG_INFO() << "Processing input file: " << inFile;

        // Read input file content into the buffer
        GetInput(inFile, streamBuf);

        string baseName = inFile.stem().string(); // Base name for library/variable naming
        string inputPathString = inFile.string(); // Full path string for builder context

        // Preprocess and add to library builder
        libBuilder.AddRepo(baseName, inputPathString, streamBuf.view());

        // If not merging, write out a separate library file for each input
        if (!isMerging)
        {
            fs::path currentOutFile;

            if (outputDir.empty()) 
            {
                // No output dir specified, place output next to input
                currentOutFile = inFile;
            }
            else 
            {
                // Output dir specified
                if (fs::is_directory(outPath)) 
                {
                    // Write into the directory using the input filename
                    currentOutFile = outPath / inFile.filename();
                }
                else 
                {
                    // Output path is a file (only allowed for single input implicitly)
                    currentOutFile = outPath;
                }
            }

            // Set the correct output file extension
            if (isHeaderLib)
                currentOutFile.replace_extension(".hpp");
            else
                currentOutFile.replace_extension(".bin");
           
            LOG_INFO() << "Output path for this file: " << currentOutFile;
            WriteLibrary(baseName, libBuilder, streamBuf, currentOutFile);
        }
    }

    // If merging, write the single combined library after processing all inputs
    if (isMerging)
    {
        if (outPath.empty())
            FX_THROW("Output path (--output <filepath>) is required when merging.");
        
        // Set the correct extension for the merged file
        if (isHeaderLib)
            outPath.replace_extension(".hpp");
        else
            outPath.replace_extension(".bin");

        string mergedName = outPath.stem().string(); // Use output filename stem for name
        LOG_INFO() << "Writing merged library: " << outPath;
        WriteLibrary(mergedName, libBuilder, streamBuf, outPath);
    }

    timer.Stop();
    LOG_INFO() << "Total processing time: " << timer.GetElapsedMS() << " ms";
}

/**
 * @brief Parses command-line arguments using the defined flag and option maps.
 * @throws EffectParseException If an unknown option or flag is encountered, or if arguments are malformed.
 */
static void HandleOptions(const IDynamicArray<string_view>& args)
{
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
}

static bool TryInitLog(const fs::path& workingDir)
{
    try
    {
        // Initialize logging to file and console
        const string logFilePath = (workingDir / "rpfx.log").string();
        Logger::InitToFile(logFilePath);
        Logger::AddStream(std::cout);
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "FATAL: Failed to initialize logger: " << e.what() << std::endl;
        return false;
    }
}

int main(int argc, char* argv[])
{
    int exitCode = 0;
    const fs::path workingDir = fs::current_path();
  
    if (!TryInitLog(workingDir))
        return 10;

    try
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
                "--input", "*.rpfx",
                "--output", "output_debug",
                "--feature-level", "5_0",
                 "-m",
            };
        }
        else
        {
            // Use actual command line args if provided
            for (int i = 0; i < argc; ++i)
                args[i] = string_view(argv[i]);
        }

#endif
        if (args.GetLength() < 2)
        {
            std::cerr << "Error: No arguments provided";
            shouldShowHelp = true;
        }
        else
        {
            LOG_INFO() << "RPFX Preprocessor Initializing...";
            LOG_INFO() << "Working Directory: " << workingDir.string();

            // Log the command line used to invoke the program
            std::stringstream cmdLine;
            for (int i = 0; i < argc; ++i) { cmdLine << argv[i] << " "; }
            LOG_INFO() << "Command Line: " << cmdLine.str();

            HandleOptions(args);

            if (!shouldShowHelp)
            {
                CreateLibrary();
                LOG_INFO() << "Processing completed successfully.";
            }
        }

        if (shouldShowHelp)
            std::cout << g_FX_HelpText;
    }
    catch (const EffectParseException& err)
    {
        LOG_ERROR() << "[" << err.GetType() << "] " << err.GetDescription();
        exitCode = 5;
    }
#ifdef NDEBUG
    catch (const RepException& err)
    {
        LOG_ERROR() << "[" << err.GetType() << "] " << err.GetDescription();
        exitCode = 4;
    }
    catch (const fs::filesystem_error& e)
    {
        LOG_ERROR() << "Filesystem Error: " << e.what();
        // Log paths involved if available (e.g., e.path1(), e.path2())
        if (!e.path1().empty()) LOG_ERROR() << "  Path1: " << e.path1();
        if (!e.path2().empty()) LOG_ERROR() << "  Path2: " << e.path2();
        exitCode = 3;
    }
    catch (const std::exception& err)
    {
        LOG_ERROR() << "Standard Exception: " << err.what();
        exitCode = 2;
    }
    catch (...)
    {
        LOG_ERROR() << "An unknown exception occurred.";
        exitCode = 1;
    }
#else
    (void)std::getchar();
#endif

    return exitCode;
}