#pragma once

#include <atomic>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>

#include "bsoncxx/builder/stream/document.hpp"
#include "bsoncxx/builder/stream/array.hpp"
#include "bsoncxx/json.hpp"

class Logger
{
public:
    Logger() :
        _open(false),
        _document(bsoncxx::builder::stream::document{}),
        _data(bsoncxx::builder::stream::array{})
    {
        
    }

    ~Logger() = default;

    void openLog()
    {
        if (!_open)
        {
            _document << "creation-time" << getDate();
            _open = true;
        }
    }

    void write(const std::string &type ,const std::string &description)
    {
        if (_open)
        {
            _data << bsoncxx::builder::stream::open_document
                << "type" << type
                << "description-time" << getTime()
                << "description" << description
                << bsoncxx::builder::stream::close_document;
        }
    }

    bsoncxx::builder::stream::document closeLog()
    {
        _document << "data" << _data;
        // _document << bsoncxx::builder::stream::finalize;
        std::cout << bsoncxx::to_json(_document.view()) << std::endl;
        _open = false;
        return std::move(_document);
    }

private:
    std::string getDate()
    {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm local_tm = *std::localtime(&now_c);
        std::stringstream stream;
        stream << std::put_time(&local_tm, "%Y-%m-%d %H-%M-%S");
        return stream.str();
    }

    std::string getTime()
    {
        auto now = std::chrono::system_clock::now();
        auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        auto value = now_ms.time_since_epoch();
        auto sec = std::chrono::duration_cast<std::chrono::seconds>(value);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(value - sec);

        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm local_tm = *std::localtime(&now_c);
        std::stringstream stream;
        stream << std::setfill('0') << std::setw(2) << local_tm.tm_hour << '-'
            << std::setfill('0') << std::setw(2) << local_tm.tm_min << '-'
            << std::setfill('0') << std::setw(2) << local_tm.tm_sec << ':'
            << std::setfill('0') << std::setw(3) << ms.count();

        return stream.str();
    }

    std::atomic<bool> _open;
    bsoncxx::builder::stream::document _document;
    bsoncxx::builder::stream::array _data;
};
