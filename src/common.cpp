#include "common.h"

namespace Common
{
    bool IsSameString(const char *strA, const char *strB)
    {
        if (!strA || !strB)
            return false;

        return _stricmp(strA, strB) == 0;
    }
}