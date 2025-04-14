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
    FX_CHECK_MSG(pos < args.GetLength() && args[pos][0] != '-',
        "Expected argument after {}", args[pos - 1]);

    param = args[pos];
}

static void SetStringParam(const IDynamicArray<string_view>& args, int& pos, string& param)
{
    pos++;
    FX_CHECK_MSG(pos < args.GetLength() && args[pos][0] != '-', 
        "Expected argument after {}", args[pos - 1]);
    FX_CHECK_MSG(param.empty(), 
        "'{}' specified twice", args[pos - 1]);

    param = args[pos];
}

static void SetHeaderLib(const IDynamicArray<string_view>& args, int& pos) { isHeaderLib = true; }

static void SetDebug(const IDynamicArray<string_view>& args, int& pos) { isDebugging = true; }

static void SetMerge(const IDynamicArray<string_view>& args, int& pos) { isMerging = true; }

static void SetFeatureLevel(const IDynamicArray<string_view>& args, int& pos) { SetStringParam(args, pos, featureLevel); }

static void SetOutput(const IDynamicArray<string_view>& args, int& pos) { SetStringParam(args, pos, outputDir); }

static void SetInput(const IDynamicArray<string_view>& args, int& pos) 
{
    pos++;
    FX_CHECK_MSG(pos < args.GetLength() && args[pos][0] != '-', "Expected argument after {}", args[pos - 1]);

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
        const char* pExt = pWild + 1;
        const char* pLast = &inPath[inPath.length() - 1];
        FX_CHECK_MSG(pLast >= pExt, "Expected file extension after wildcard.");

        fs::path inDir;
        string_view ext(pExt);

        if (pWild > inPath.data())
            inDir = string_view(inPath.data(), std::distance(inPath.data(), pWild - 1));
        else
            inDir = fs::current_path();

        GetFilesByExtension(inDir, ext, inputFiles);
    }
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

    FX_CHECK_MSG(fs::exists(input), "Input path does not exist");
    FX_CHECK_MSG(fs::is_regular_file(input), "Input path is not a file");

    std::ifstream inputStream(input);

    FX_CHECK_MSG(inputStream.is_open(), "Failed to open input file: {}");
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
    FX_CHECK_MSG(dstFile.is_open(), "Failed to open output file: {}");

    dstFile << ss.rdbuf();
}

static void WriteLibrary(string_view name, ShaderLibBuilder& libBuilder, std::stringstream& streamBuf, const fs::path& output)
{
    ShaderLibDef::Handle shaderLib = libBuilder.GetDefinition();

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
    LOG_INFO() << "Library Stats: Shaders: " << shaderLib.regHandle.pShaders->GetLength()
        << " | Effects: " << shaderLib.regHandle.pEffects->GetLength()
        << " | Constants: " << shaderLib.regHandle.pConstants->GetLength()
        << " | Resources: " << shaderLib.regHandle.pResources->GetLength();
    LOG_INFO() << "Compiler: " << shaderLib.pPlatform->compilerVersion;
    LOG_INFO() << "Shader Model: " << shaderLib.pPlatform->featureLevel;

    libBuilder.Clear();
}

/// <summary>
/// Preprocesses and precompiles an RPFX shader library that is written out as a binary
/// </summary>
static void CreateLibrary()
{
    FX_CHECK_MSG(!inputFiles.empty(), "No input specified");

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
                FX_CHECK_MSG(fs::is_directory(outPath), "Output path must be a directory.");
            else
                FX_CHECK_MSG(!fs::is_directory(outPath), "Output path must be a file.");
        }
    }
    else if (isMerging)
        FX_THROW("Output file must be specified for merged libraries.");

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
                    FX_CHECK_MSG(it != s_OptionMap.end(), "Unexpected argument: {}", arg);

                    it->second(args, i);
                }
                else
                {
                    string_view flags = arg.substr(1);

                    for (char flag : flags)
                    {
                        const auto& it = s_FlagMap.find(flag);
                        FX_CHECK_MSG(it != s_FlagMap.end(), "Unexpected flag: {}", arg);

                        it->second(args, i);
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
                    FX_THROW("Unexpected argument: {}", arg);
            }
        }
        else
            FX_THROW("Unexpected argument: {}", arg);
    }
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

    try
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
                "-hm"
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
    catch (const ParseException& err)
    {
        LOG_ERROR() << "[" << err.GetType() << "] " << err.GetDescription();
        code = 6;
    }
#ifndef _DEBUG_VS
    catch (const RepException& err)
    {
        LOG_ERROR() << "[" << err.GetType() << "] " << err.GetDescription();
        code = 5;
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