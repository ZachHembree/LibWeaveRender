#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include "ReplicaEffects/ParseExcept.hpp"
#include "ReplicaUtils/Logger.hpp"
#include "ReplicaEffects/ShaderLibBuilder.hpp"

using std::cout;
using std::cin;
using std::endl;
namespace fs = std::filesystem;

using namespace Replica;
using namespace Replica::Effects;

// Settings
static bool isDebugging = false;
static bool isHeaderLib = false;
static bool isMerging = false;
static string featureLevel;
static string outputDir;
static std::unordered_set<string> inputFiles;

// Option configuration
static void GetFilesByExtension(const fs::path& dir, string_view ext, std::unordered_set<string>& files)
{
    const auto& dirIt = fs::directory_iterator(dir);

    for (const fs::directory_entry& entry : dirIt)
    {
        if (entry.is_regular_file() && entry.path().extension() == ext)
            files.emplace(entry.path().string());
    }
}

template<typename T>
static void SetParam(const IDynamicArray<string_view>& args, int& pos, T& param)
{
    pos++;

    if (pos < args.GetLength() && args[pos][0] != '-')
    {
        param = args[pos];
    }
    else
        PARSE_ERR_FMT("Expected argument after {}", args[pos - 1]);
}

static void SetStringParam(const IDynamicArray<string_view>& args, int& pos, string& param)
{
    pos++;

    if (pos < args.GetLength() && args[pos][0] != '-')
    {
        if (param.empty())
            param = args[pos];
        else
            PARSE_ERR_FMT("{} specified twice", args[pos - 1]);
    }
    else
        PARSE_ERR_FMT("Expected argument after {}", args[pos - 1]);
}

static void SetHeaderLib(const IDynamicArray<string_view>& args, int& pos) { isHeaderLib = true; }

static void SetDebug(const IDynamicArray<string_view>& args, int& pos) { isDebugging = true; }

static void SetMerge(const IDynamicArray<string_view>& args, int& pos) { isMerging = true; }

static void SetFeatureLevel(const IDynamicArray<string_view>& args, int& pos) { SetStringParam(args, pos, featureLevel); }

static void SetOutput(const IDynamicArray<string_view>& args, int& pos) { SetStringParam(args, pos, outputDir); }

static void SetInput(const IDynamicArray<string_view>& args, int& pos) 
{
    pos++;

    if (pos < args.GetLength() && args[pos][0] != '-')
    {
        string_view inPath = args[pos];
        const char* pWild = nullptr;

        for (int i = (int)inPath.length() - 1; i >= 0; i--)
        {
            if (inPath[i] == '\\' || inPath[i] == '/')
                break;
            else if (inPath[i] == '*')
            {
                pWild = &inPath[i];
                break;
            }
        }

        if (pWild == nullptr)
            inputFiles.emplace(inPath);
        else
        {
            const char *pExt = pWild + 1;
            const char *pLast = &inPath[inPath.length() - 1];
            PARSE_ASSERT_MSG(pLast >= pExt, "Expected file extension after wildcard.")

            fs::path inDir;
            string_view ext(pExt);

            if (pWild > inPath.data())
                inDir = string_view(inPath.data(), std::distance(inPath.data(), pWild - 1));
            else
                inDir = fs::current_path();

            GetFilesByExtension(inDir, ext, inputFiles);
        }
    }
    else
        PARSE_ERR_FMT("Expected argument after {}", args[pos - 1]);
}

typedef void (*OptionHandlerFunc)(const IDynamicArray<string_view>& args, int& pos);

static const std::unordered_map<char, OptionHandlerFunc> s_FlagMap
{
    { 'd', SetDebug },
    { 'h', SetHeaderLib },
    { 'm', SetMerge },
};

static const std::unordered_map<string_view, OptionHandlerFunc> s_OptionMap 
{
    { "debug", SetDebug },
    { "header", SetHeaderLib },
    { "merge", SetMerge },
    { "feature-level", SetFeatureLevel },
    { "input", SetInput },
    { "output", SetOutput }
};

static std::ostream& WriteCharHex(char value, std::ostream& ss)
{
    return ss << "0x" << std::hex << (int)(unsigned char)value;
}

/// <summary>
/// Converts binary encoded as signed char in string_view to hexadecimal string in a C++ header
/// </summary>
static void ConvertBinaryToHeader(string_view name, std::stringstream& binStream)
{
    static string strBuf;
    strBuf.clear();
    strBuf.append(binStream.view());
    binStream.clear();
    binStream.str({});

    binStream << "constexpr unsigned char " << name << "[" << strBuf.size() << "] {\n";

    for (int i = 0; i < strBuf.size() - 1; i++)
    {
        WriteCharHex(strBuf[i], binStream) << ",";
        
        if (i > 0 && i % 20 == 0)
            binStream << endl;
    }

    WriteCharHex(strBuf.back(), binStream) << endl;
    binStream << "};";
}

/// <summary>
/// Reads the library from the given path and writes it to the buffer
/// </summary>
static void GetInput(fs::path input, std::stringstream& ss)
{
    ss.clear();
    ss.str({});

    if (!fs::exists(input))
        PARSE_ERR("Input path does not exist");

    if (!fs::is_regular_file(input))
        PARSE_ERR("Input path is not a file");

    std::ifstream inputStream(input);

    if (!inputStream.is_open())
        PARSE_ERR_FMT("Failed to open input file: {}", input.string());

    ss << inputStream.rdbuf();
}

static void ValidateOutputDir(const fs::path& output)
{
    if (fs::is_directory(output))
    {
        if (!fs::exists(output))
        {
            if (fs::create_directories(output))
                LOG_INFO() << "Created output directories: " << output;
        }
    }
    else
    {
        fs::path dstParent = output.parent_path();

        if (!dstParent.empty() && !fs::exists(dstParent))
        {
            if (fs::create_directories(dstParent))
                LOG_INFO() << "Created output directories: " << dstParent;
        }
    }
}

/// <summary>
/// Writes the serialized library data to the given output
/// </summary>
static void WriteBinary(const fs::path& output, const std::stringstream& ss)
{
    ValidateOutputDir(output);

    // Open output
    std::ofstream dstFile(output);

    if (!dstFile.is_open())
        PARSE_ERR_FMT("Failed to open output file: {}", output.string());

    dstFile << ss.rdbuf();
}

static void WriteLibrary(string_view name, ShaderLibBuilder& libBuilder, std::stringstream& streamBuf, const fs::path& output)
{
    ShaderLibDef shaderLib = libBuilder.ExportLibrary();

    streamBuf.clear();
    streamBuf.str({});

    // Write serialized library binary to ss
    Serializer libWriter(streamBuf);
    libWriter(shaderLib);

    // Convert output to hex string header
    if (isHeaderLib)
    {
        string varName = "s_FX_";
        varName += name;
        ConvertBinaryToHeader(varName, streamBuf);
    }

    WriteBinary(output, streamBuf);

    LOG_INFO() << "Wrote library to " << output;
    LOG_INFO() << "Library Stats: Shaders: " << shaderLib.regData.shaders.GetLength()
        << " | Effects: " << shaderLib.regData.effects.GetLength()
        << " | Constants: " << shaderLib.regData.constants.GetLength()
        << " | Resources: " << shaderLib.regData.resources.GetLength();
    LOG_INFO() << "Compiler: " << shaderLib.platform.compilerVersion;
    LOG_INFO() << "Shader Model: " << shaderLib.platform.featureLevel;
}

/// <summary>
/// Preprocesses and precompiles an RPFX shader library that is written out as a binary
/// </summary>
static void CreateLibrary()
{
    static ShaderLibBuilder libBuilder;
    static std::stringstream streamBuf;

    libBuilder.Clear();
    streamBuf.clear();
    streamBuf.str({});

    if (featureLevel.empty())
        featureLevel = "5_0";

    libBuilder.SetFeatureLevel(featureLevel);
    libBuilder.SetDebug(isDebugging);

    fs::path outPath;

    if (inputFiles.size() == 1 && isMerging)
        isMerging = false;

    if (!outputDir.empty())
    {
        outPath = fs::path(outputDir);

        if (inputFiles.size() > 1)
        { 
            if (!isMerging)
                REP_ASSERT_MSG(fs::is_directory(outPath), "Output path must be a directory.")
            else
                REP_ASSERT_MSG(!fs::is_directory(outPath), "Output path must be a file.")
        }
    }
    else if (isMerging)
        REP_THROW_MSG("Output file must be specified for merged libraries.")

    // Parse source and generate library
    Stopwatch timer;
    timer.Start();

    for (const string& input : inputFiles)
    {
        fs::path inFile(input);
        
        // Read input into buffer
        GetInput(inFile, streamBuf);

        string name = inFile.stem().string();
        const string inString = inFile.string();
        libBuilder.AddRepo(name, inString, streamBuf.view());

        if (!isMerging)
        {
            // If an output folder was specified, write all libraries to it
            fs::path outFile = outputDir.empty() ? inFile.parent_path() : outPath;

            // Use original file name in new path, but with different extension
            if (fs::is_directory(outFile))
                outFile = fs::path(outFile.string() + "\\" + inFile.filename().string());
            
            if (isHeaderLib)
                outFile.replace_extension(".hpp");
            else
                outFile.replace_extension(".bin");

            LOG_INFO() << "Input path: " << inFile;
            LOG_INFO() << "Output path: " << outFile;

            WriteLibrary(name, libBuilder, streamBuf, outFile);
            libBuilder.Clear();
        }
    }

    if (isMerging)
    {
        string name = outPath.stem().string();
        WriteLibrary(name, libBuilder, streamBuf, outPath);
    }

    timer.Stop();
    LOG_INFO() << "Time: " << timer.GetElapsedMS() << "ms";
}

/// <summary>
/// Handles initial option configuration via CLI arguments
/// </summary>
static void HandleOptions(const IDynamicArray<string_view>& args)
{
    for (int i = 1; i < args.GetLength(); i++)
    {
        const string_view& arg = args[i];

        if (arg.size() > 1)
        {
            if (arg[0] == '-')
            {
                if (arg[1] == '-')
                {
                    string_view opt = arg.substr(2);
                    const auto& it = s_OptionMap.find(opt);

                    if (it != s_OptionMap.end())
                        it->second(args, i);
                    else
                        PARSE_ERR_FMT("Unexpected argument: {}", arg);
                }
                else
                {
                    string_view flags = arg.substr(1);

                    for (char flag : flags)
                    {
                        const auto& it = s_FlagMap.find(flag);

                        if (it != s_FlagMap.end())
                            it->second(args, i);
                        else
                            PARSE_ERR_FMT("Unexpected flag: {}", flag);
                    }
                }
            }
            else
            {
                if (inputFiles.empty())
                    inputFiles.emplace(arg);
                else if (outputDir.empty())
                    outputDir = arg;
                else
                    PARSE_ERR_FMT("Unexpected argument: {}", arg);
            }
        }
        else
            PARSE_ERR_FMT("Unexpected argument: {}", arg);
    }

    if (inputFiles.empty())
        PARSE_ERR_FMT("No input specified");
}

int main(int argc, char* argv[])
{
    int code = 0;
    const fs::path workingDir = fs::current_path();
    const string logFile = workingDir.string() + "\\rpfx.log";
    Logger::InitToFile(logFile);
    Logger::AddStream(cout);

    LOG_INFO() << "RPFX Preprocessor";
    LOG_INFO() << "Working Dir: " << argv[0];

#ifndef _DEBUG_VS
    try
#endif
    {
#ifndef _DEBUG
        DynamicArray<string_view> args(argc);

        for (int i = 0; i < argc; i++)
            args[i] = string_view(argv[i]);
#else
        DynamicArray<string_view> args;

        if (argc == 1)
        {
            args =
            {
                string_view(argv[0]),
                "--input", "*.rpfx",
                "--output", "..\\EffectPreprocessor\\",
                "-h"
            };
        }
        else
        {
            args = DynamicArray<string_view>(argc);

            for (int i = 0; i < argc; i++)
                args[i] = string_view(argv[i]);
        }
#endif

        HandleOptions(args);
        CreateLibrary();
    }
#ifndef _DEBUG_VS
    catch (const RepException& err)
    {
        LOG_ERROR() << "[" << err.GetType() << "] " << err.what();
        code = 5;
    }
    catch (const WaveException& err)
    {
        LOG_ERROR() << err.description();
        code = 4;
    }
    catch (const fs::filesystem_error& e) 
    {
        LOG_ERROR() << "Filesystem error: " << e.what();
        code = 3;
    }
    catch (const std::exception& err)
    {
        LOG_ERROR() << err.what();
        code = 2;
    }
    catch (...)
    {
        LOG_ERROR() << "An unknown exception has occurred.";
        code = 1;
    }
#endif

    std::getchar();
    return code;
}