#pragma once

#include <string>
#include <unordered_map>

enum class LogSeverity : int
{
    INFO = 0,
    ERROR = 1,
    CRITICAL = 2,
    WARNING = 3,
    DEBUG = 4
};