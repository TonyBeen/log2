/*************************************************************************
    > File Name: unwind_stacktrace.hpp
    > Author: hsz
    > Brief:
    > Created Time: 2025年08月13日 星期三 09时44分24秒
 ************************************************************************/

#ifndef __UNWIND_STACKTRACE_HPP__
#define __UNWIND_STACKTRACE_HPP__

#if defined(OS_LINUX) 
#include <vector>
#include <string>

#include <cxxabi.h>
#include <libunwind/libunwind.h>

namespace detail {

std::vector<std::string> unwind_stacktrace(uint16_t ignoreDepth = 1, uint16_t ignoreEnd = 2)
{
    std::vector<std::string> stackVec;
    unw_cursor_t cursor;
    unw_context_t context;

    unw_getcontext(&context);
    unw_init_local(&cursor, &context);
    char buf[1024] = {0};

    while (unw_step(&cursor) > 0) {
        unw_word_t offset, funcPointer;
        unw_get_reg(&cursor, UNW_REG_IP, &funcPointer);
        if (funcPointer == 0) {
            break;
        }

        char sym[256] = {0};
        if (unw_get_proc_name(&cursor, sym, sizeof(sym), &offset) == 0) {
            char *nameptr = sym;
            int status = -1;
            char *demangled = abi::__cxa_demangle(sym, nullptr, nullptr, &status);
            if (status == 0 && demangled != nullptr) {
                nameptr = demangled;
            }

            snprintf(buf, sizeof(buf), "-0x%012lx: (%s + 0x%lx)", funcPointer, nameptr, offset);
            stackVec.push_back(std::string(buf));
            if (demangled) {
                free(demangled);
            }
        } else {
            stackVec.push_back(std::string("(unable to obtain symbol name for this frame)"));
        }
    }

    stackVec.erase(stackVec.begin(), stackVec.begin() + ignoreDepth);
    stackVec.erase(stackVec.end() - ignoreEnd, stackVec.end());
    return stackVec;
}

} // namespace detail

#endif // OS_LINUX
#endif // __UNWIND_STACKTRACE_HPP__
