#pragma once
#include <string_view>

constexpr std::string_view g_FX_HelpText = R"HELPTEXT(
WFX Preprocessor - Shader Library Compiler (v0.1.0)
----------------------------------------------------
Preprocesses and compiles Weave Effects (.wfx) shader source files into
optimized, deployable shader libraries. These libraries can be output as
binary files (.bin) for runtime loading or as C++ header files (.hpp) for
embedding directly into application code.

USAGE:
    wfxc --input <file|pattern> [--output <path>] [options]
    wfxc --help

OPTIONS:
    --input <file|pattern>
                      Specifies the input WFX file(s).
                      - Use a specific filename (e.g., 'myshader.wfx').
                      - Use a wildcard pattern (e.g., '*.wfx') to process
                        all matching files in the current directory or the
                        specified path's directory (e.g., 'shaders/*.wfx').
                      This option is REQUIRED.

    --output <path>
                      Specifies the output destination file or directory.
                      Behavior depends on the --merge option:
                      - Without --merge:
                          * If <path> is a directory, output files (matching input
                            names) are placed inside it. The directory will be
                            created if it doesn't exist.
                          * If <path> is a file path, it's only valid when processing
                            a single input file.
                          * If omitted, output files (.bin or .hpp) are placed
                            alongside their respective input files.
                      - With --merge:
                          * <path> MUST be specified and MUST be a single output
                            filename (e.g., 'output/merged_library.bin').

    --feature-level <level>
                      Sets the target shader feature level (e.g., '5_0', '6_0').
                      [Default: '5_0']

-m, --merge           Merge all processed input files into a single output library
                      file specified by --output. If not set (default), each
                      input file produces a separate output file.

-h, --header          Output the library as a C++ header file (.hpp) containing
                      a 'constexpr unsigned char' array, instead of a raw
                      binary file (.bin). The C++ variable name is generated
                      based on the output filename (e.g., 's_FX_MyLibrary').
                      [Default: Outputs binary .bin file]

-d, --debug           Enable debug information during shader compilation. This
                      may include shader symbols for debugging tools but can
                      increase file size and potentially impact runtime performance.
                      [Default: Disabled]

    --help            Display this help message and exit.

EXAMPLES:
    # Compile all .wfx files in the current directory into separate .bin files
    wfxc --input "*.wfx"

    # Compile a single file into a specific output directory as a C++ header
    wfxc --input myshaders/common.wfx --output compiled/ --header

    # Compile all .wfx files in 'src_shaders/' and merge them into a single
    # binary library for Shader Model 6.0
    wfxc --input "src_shaders/*.wfx" --output libs/shader_bundle.bin --merge --feature-level 6_0

    # Compile a single file with debugging enabled to a specific output file
    wfxc --input test.wfx --output test_debug.bin --debug

EXIT CODES:
     0: Success
     1: Unknown error occurred
     2: Standard C++ exception caught
     3: Filesystem error (I/O, permissions, path not found)
     4: Weave framework internal error
     5: Effect parsing/compilation error (check logs for details)
    10: Critical initialization error (e.g., cannot initialize logging)

NOTES:
    - Detailed logs are written to 'wfxc.log' in the directory where the
      preprocessor is executed. Check this file for detailed error messages.

)HELPTEXT";