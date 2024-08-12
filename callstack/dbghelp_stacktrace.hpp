#if defined(OS_WIN32) || defined(OS_WIN64)

#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>

#include "StackWalker.h"

namespace detail {
	class DbgStackWalker : public StackWalker
	{
	public:
		DbgStackWalker(int32_t options = OptionsAll,
			LPCSTR szSymPath = NULL,
			DWORD dwProcessId = GetCurrentProcessId(),
			HANDLE hProcess = GetCurrentProcess()) :
			StackWalker(options, szSymPath, dwProcessId, hProcess)
		{
		}

        void OnShowCallstack() override
        {
            m_stackFrameVec.clear();
        }

		void OnCallstackEntry(CallstackEntryType eType, CallstackEntry& entry) override
		{
            CHAR buffer[STACKWALK_MAX_NAMELEN] = { 0 };
            size_t maxLen = STACKWALK_MAX_NAMELEN;
#if _MSC_VER >= 1400
            maxLen = _TRUNCATE;
#endif
            if ((eType != lastEntry) && (entry.offset != 0))
            {
                if (entry.name[0] == 0)
                {
                    strcpy_s(entry.name, STACKWALK_MAX_NAMELEN, "(function-name not available)");
                }
                if (entry.undName[0] != 0)
                {
                    strcpy_s(entry.name, STACKWALK_MAX_NAMELEN, entry.undName);
                }
                if (entry.undFullName[0] != 0)
                {
                    strcpy_s(entry.name, STACKWALK_MAX_NAMELEN, entry.undFullName);
                }
                
                _snprintf_s(buffer, maxLen, "%p: (%s)\n", (LPVOID)entry.offset, entry.name);
                buffer[STACKWALK_MAX_NAMELEN - 1] = '\0';
                m_stackFrameVec.push_back(buffer);
            }
		}

        std::vector<std::string> m_stackFrameVec;
	};
}
#endif