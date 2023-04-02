#pragma once

#include "LogSeverity.hpp"

class LogMessage
{
public:
    LogMessage()
    {

    }

    LogMessage(LogSeverity severity, std::string description):
        _severity(severity), _description(description)
    {

    }

    ~LogMessage() = default;

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
