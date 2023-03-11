#pragma once

#include "LogSeverity.hpp"

class MessageLogger
{
public:
    MessageLogger()
    {

    }

    MessageLogger(LogSeverity severity, const std::string& description):
        _severity(severity), _description(description)
    {

    }

    ~MessageLogger() = default;

    std::string& getDescription()
    {
        return _description;
    }

    void setDescription(std::string& description)
    {
        _description = description;
    }

    LogSeverity getSeverity()
    {
        return _severity;
    }

    void setSeverity(LogSeverity severity)
    {
        _severity = severity;
    }

private:
    LogSeverity _severity;
    std::string _description;

};
