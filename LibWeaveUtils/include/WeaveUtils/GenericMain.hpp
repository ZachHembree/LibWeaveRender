#pragma once
#include <concepts>
#include <type_traits>
#include <filesystem>
#include "WeaveException.hpp"
#include "Logger.hpp"

namespace Weave
{
    namespace fs = std::filesystem;

    // Generic main function pointer template used with GenericMain()
    template<typename... FuncArgsT>
    using GenericMainT = void(*)(FuncArgsT...);

    /**
     * @brief Executes an entrypoint function within a standardized exception handling wrapper.
     *
     * This utility function invokes a given main-style function and catches a variety of common
     * exception types, returning a standardized exit code depending on the failure.
     *
     * @tparam ErrStreamT Type of the error output stream (e.g., std::ostream or custom logger).
     * @tparam FuncArgsT  Argument types forwarded to the main function.
     * @param errStream   Output stream to report errors.
     * @param MainFunc    Function pointer to the main routine to invoke.
     * @param funcArgs    Arguments to forward to the main function.
     * @return int        Exit code: 0 on success, or a failure code based on the exception type.
     */
    template<typename ErrStreamT, typename... FuncArgsT>
    requires IsStreamLike<ErrStreamT>
    int GenericMain(ErrStreamT& errStream, GenericMainT<FuncArgsT...> MainFunc, FuncArgsT&&... funcArgs)
    {
        int exitCode = 0;

#ifndef WV_TRACE_EXCEPT
        try
        {
#endif
            MainFunc(std::forward<FuncArgsT>(funcArgs)...);
#ifndef WV_TRACE_EXCEPT
        }
        catch (const WeaveException& err)
        {
            errStream << "[" << err.GetType() << "] " << err.GetDescription() << '\n';
            exitCode = 3;
        }
        catch (const fs::filesystem_error& e)
        {
            errStream << "Filesystem Error: " << e.what() << '\n';
            // Log paths involved if available (e.g., e.path1(), e.path2())
            if (!e.path1().empty()) errStream << "  Path1: " << e.path1() << '\n';
            if (!e.path2().empty()) errStream << "  Path2: " << e.path2() << '\n';
            exitCode = 3;
        }
        catch (const std::exception& err)
        {
            errStream << "Standard Exception: " << err.what() << '\n';
            exitCode = 2;
        }
        catch (...)
        {
            errStream << "An unknown exception occurred." << '\n';
            exitCode = 1;
        }
#endif
        return exitCode;
    }
}