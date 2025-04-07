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

// Options
static bool isDebugging = false;
static bool isHeaderLib = false;
static string featureLevel;
static string inputPath;
static string outputPath;
static string name;

// Option configuration
static void SetHeaderLib(const IDynamicArray<string_view>& args, int& pos) { isHeaderLib = true; }

static void SetDebug(const IDynamicArray<string_view>& args, int& pos) { isDebugging = true; }

static void SetFeatureLevel(const IDynamicArray<string_view>& args, int& pos)
{
    pos++;

    if (pos < args.GetLength() && args[pos][0] != '-')
    {
        if (featureLevel.empty())
            featureLevel = args[pos];
        else
            PARSE_ERR("Feature level specified twice");
    }
    else
        PARSE_ERR_FMT("Expected shader feature level after {}", args[pos - 1]);
}

static void SetInput(const IDynamicArray<string_view>& args, int& pos) 
{
    pos++;

    if (pos < args.GetLength() && args[pos][0] != '-')
    {
        if (inputPath.empty())
            inputPath = args[pos];
        else
            PARSE_ERR("Input path specified twice");
    }
    else
        PARSE_ERR_FMT("Expected source file after {}", args[pos - 1]);
}

static void SetOutput(const IDynamicArray<string_view>& args, int& pos)
{
    pos++;

    if (pos < args.GetLength() && args[pos][0] != '-')
    {
        if (outputPath.empty())
            outputPath = args[pos];
        else
            LOG_ERROR() << "Output path specified twice";
    }
    else
        PARSE_ERR_FMT("Expected output path after {}", args[pos - 1]);
}

static void SetName(const IDynamicArray<string_view>& args, int& pos)
{
    pos++;

    if (pos < args.GetLength() && args[pos][0] != '-')
    {
        if (name.empty())
            name = args[pos];
        else
            LOG_ERROR() << "Name specified twice";
    }
    else
        PARSE_ERR_FMT("Expected name after {}", args[pos - 1]);
}

typedef void (*OptionHandlerFunc)(const IDynamicArray<string_view>& args, int& pos);

static const std::unordered_map<string_view, OptionHandlerFunc> s_OptionMap 
{
    { "-h", SetHeaderLib },
    { "--header", SetHeaderLib },
    { "-fl", SetFeatureLevel },
    { "--feature-level", SetFeatureLevel },
    { "-d", SetDebug },
    { "--debug", SetDebug },
    { "-i", SetInput },
    { "--input", SetInput },
    { "-o", SetOutput },
    { "--output", SetOutput },
    { "-n", SetName },
    { "--name", SetName }
};

static std::ostream& WriteCharHex(char value, std::ostream& ss)
{
    return ss << "0x" << std::hex << (int)(unsigned char)value;
}

/// <summary>
/// Converts binary encoded as signed char in string_view to hexadecimal string in a C++ header
/// </summary>
static void ConvertBinaryToHeader(string_view name, string& src, std::stringstream& dst)
{
    src.append(dst.view());
    dst.clear();
    dst.str({});

    dst << "constexpr unsigned char " << name << "[" << src.size() << "] {\n";

    for (int i = 0; i < src.size() - 1; i++)
    {
        WriteCharHex(src[i], dst) << ",";
        
        if (i > 0 && i % 20 == 0)
            dst << endl;
    }

    WriteCharHex(src.back(), dst) << endl;
    dst << "};";
}

/// <summary>
/// Reads the library from the given path and writes it to the buffer
/// </summary>
static void GetInput(fs::path input, string_view libPath, std::stringstream& ss)
{
    if (!fs::exists(input))
        PARSE_ERR("Input path does not exist");

    if (!fs::is_regular_file(input))
        PARSE_ERR("Input path is not a file");

    std::ifstream inputStream(input);

    if (!inputStream.is_open())
        PARSE_ERR_FMT("Failed to open input file: {}", libPath);

    ss << inputStream.rdbuf();
}

/// <summary>
/// Writes the serialized library data to the given output
/// </summary>
static void WriteBinary(fs::path output, string_view binPath, const std::stringstream& ss)
{
    // Ensure ouput dir exists
    fs::path dstParent = output.parent_path();
    if (!dstParent.empty() && !fs::exists(dstParent))
    {
        if (fs::create_directories(dstParent))
            LOG_INFO() << "Created output directories: " << dstParent;
    }

    // Open output
    std::ofstream dstFile(output);

    if (!dstFile.is_open())
        PARSE_ERR_FMT("Failed to open output file: {}", binPath);

    dstFile << ss.rdbuf();
}

/// <summary>
/// Preprocesses and precompiles an RPFX shader library that is written out as a binary
/// </summary>
static void CreateLibrary()
{
    static ShaderLibBuilder libBuilder;
    static std::stringstream streamBuf;
    static string strBuf;

    libBuilder.Clear();
    streamBuf.clear();
    streamBuf.str({});
    strBuf.clear();

    fs::path input(inputPath);
    fs::path output;

    if (outputPath.empty())
    {
        output = input;

        if (isHeaderLib)
            output.replace_extension(".hpp");
        else
            output.replace_extension(".bin");

        outputPath = output.string();
    }
    else
        output = fs::path(outputPath);

    if (name.empty())
        name = input.stem().string();

    if (featureLevel.empty())
        featureLevel = "5_0";

    LOG_INFO() << "Input path: " << inputPath;
    LOG_INFO() << "Output path: " << outputPath;

    // Read input into buffer
    GetInput(input, inputPath, streamBuf);

    // Parse source and generate library
    Stopwatch timer;
    timer.Start();
    libBuilder.AddRepo(name, inputPath, streamBuf.view());
    ShaderLibDef shaderLib = libBuilder.ExportLibrary();

    LOG_INFO() << "Compiler: " << shaderLib.platform.compilerVersion;
    LOG_INFO() << "Shader Model: " << shaderLib.platform.featureLevel;

    streamBuf.clear();
    streamBuf.str({});

    // Write serialized library binary to ss
    Serializer libWriter(streamBuf);
    libWriter(shaderLib);

    name = "s_FX_" + name;

    // Convert output to hex string header
    if (isHeaderLib)
        ConvertBinaryToHeader(name, strBuf, streamBuf);

    WriteBinary(output, outputPath, streamBuf);
    timer.Stop();

    LOG_INFO() << "Wrote library to " << outputPath;
    LOG_INFO() << "Library Stats: Shaders: " << shaderLib.regData.shaders.GetLength()
        << " | Effects: " << shaderLib.regData.effects.GetLength()
        << " | Constants: " << shaderLib.regData.constants.GetLength()
        << " | Resources: " << shaderLib.regData.resources.GetLength();
    LOG_INFO() << "Time: " << timer.GetElapsedMS() << "ms";
}

/// <summary>
/// Handles initial option configuration via CLI arguments
/// </summary>
static void HandleOptions(const IDynamicArray<string_view>& args)
{
    for (int i = 1; i < args.GetLength(); i++)
    {
        if (args[i].size() > 0)
        {
            if (args[i][0] == '-')
            {
                const auto& it = s_OptionMap.find(args[i]);

                if (it != s_OptionMap.end())
                    it->second(args, i);
                else
                    PARSE_ERR_FMT("Unexpected argument: {}", args[i]);
            }
            else
            {
                if (inputPath.empty())
                    inputPath = args[i];
                else if (outputPath.empty())
                    outputPath = args[i];
                else
                    PARSE_ERR_FMT("Unexpected argument: {}", args[i]);
            }
        }
    }

    if (inputPath.empty())
        PARSE_ERR_FMT("No input specified");
}

int main(int argc, char* argv[])
{
    int code = 0;
    const fs::path workingDir = fs::current_path();
    const string logFile = workingDir.string() + "\\rpfx.log";
    Logger::InitToFile(logFile);
    Logger::AddStream(cout);

    LOG_INFO() << "RPFX Init";
    LOG_INFO() << "Working Dir: " << argv[0];

#ifndef _DEBUG
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
                "-i", "ShaderTest.rpfx", 
                "--header"
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
#ifndef _DEBUG
    catch (const RepException& err)
    {
        LOG_ERROR() << "[" << err.GetType() << "] " << err.what();
        code = 1;
    }
    catch (const WaveException& err)
    {
        LOG_ERROR() << err.description();
        code = 1;
    }
    catch (const std::exception& err)
    {
        LOG_ERROR() << err.what();
        code = 1;
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