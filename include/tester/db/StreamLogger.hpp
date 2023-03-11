#pragma once

#include <sstream>
#include <string>

#include "Logger.hpp"
#include "LogSeverity.hpp"
#include "MessageLogger.hpp"

class StreamLogger
{
    using CoutType = std::basic_ostream<char, std::char_traits<char>>;
    using StandardEndline = CoutType & (*)(CoutType&);
public:
    explicit StreamLogger() : _severity(LogSeverity::INFO)
    {
        
    }

    ~StreamLogger() = default;

    template<typename T>
    StreamLogger& operator<< (const T& value)
    {
        _buffer << value;
        return *this;
    }

    StreamLogger& operator<< (const StandardEndline& manipulator)
    {
        Logger::getInstance().push(MessageLogger(_severity, _buffer.str()));
        _buffer.str("");
        return *this;
    }

    StreamLogger& info()
    {
        _severity = LogSeverity::INFO;
        return *this;
    }

    StreamLogger& error()
    {
        _severity = LogSeverity::ERROR;
        return *this;
    }

    StreamLogger& critical()
    {
        _severity = LogSeverity::CRITICAL;
        return *this;
    }

    StreamLogger& warning()
    {
        _severity = LogSeverity::WARNING;
        return *this;
    }

    StreamLogger& debug()
    {
        _severity = LogSeverity::DEBUG;
        return *this;
    }

    StreamLogger& notice()
    {
        _severity = LogSeverity::NOTICE;
        return *this;
    }

private:
    std::stringstream _buffer;
    LogSeverity _severity;
};

#define LOG_INFO StreamLogger().info()
#define LOG_ERORR StreamLogger().error()
#define LOG_CRITICAL StreamLogger().critical()
#define LOG_WARNING StreamLogger().warning()
#define LOG_DEBUG StreamLogger().debug()
#define LOG_NOTICE StreamLogger().notice()