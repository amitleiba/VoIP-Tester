#pragma once

#include <atomic>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <thread>
#include <queue>
#include <iostream>
#include <mutex>
#include <condition_variable>

#include "bsoncxx/builder/stream/document.hpp"
#include "bsoncxx/builder/stream/array.hpp"
#include "bsoncxx/json.hpp"

#include "LogSeverity.hpp"
#include "MessageLogger.hpp"

class Logger
{
public:
    
    static Logger& getInstance()
    {
        static Logger instance;
        return instance;
    }

    void openDocument()
    {
        if (!_open)
        {
            _document << "creation-time" << getDate();
            _open = true;
        }
    }


    bsoncxx::builder::stream::document closeDocument()
    {
        _document << "data" << _data;
        // _document << bsoncxx::builder::stream::finalize;
        // std::cout << bsoncxx::to_json(_document.view()) << std::endl;
        _open = false;
        return std::move(_document);
    }

    void push(MessageLogger item)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _logsQueue.emplace(std::move(item));
        lock.unlock();
        _cv.notify_one();
    }

private:
    Logger() :
        _open(false),
        _document(bsoncxx::builder::stream::document{}),
        _data(bsoncxx::builder::stream::array{})
    {
        start();
    }

    ~Logger()
    {
        stop();
    }

    void start()
    {
        std::lock_guard<std::mutex> lock(_startMutex);
        if(!_active)
        {
            _active = true;
            _logThread = std::thread(&Logger::run, this);
        }
    }

    void stop()
    {
        std::lock_guard<std::mutex> lock(_startMutex);
        _active = false;
        if (_logThread.joinable())
        {
            _logThread.join();
        }
    }

    void run()
    {
        while(_active)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _cv.wait(lock);
            while(!_logsQueue.empty())
            {
                auto item = _logsQueue.front();
                _logsQueue.pop();
                write(item.getSeverity(), item.getDescription());
            }
            lock.unlock();
        }
    }

    void write(LogSeverity type, const std::string &description)
    {
        if (_open)
        {
            _data << bsoncxx::builder::stream::open_document
                << "type" << convertor.at(type)
                << "description-time" << getTime()
                << "description" << description
                << bsoncxx::builder::stream::close_document;
        }
    }

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


    std::unordered_map <LogSeverity, std::string> convertor = 
    {
        {LogSeverity::INFO,"INFO"},
        {LogSeverity::ERROR,"ERROR"},
        {LogSeverity::CRITICAL,"CRITICAL"},
        {LogSeverity::WARNING,"WARNING"},
        {LogSeverity::DEBUG,"DEBUG"},
        {LogSeverity::NOTICE,"NOTICE"}
    };

    std::atomic<bool> _open, _active;
    bsoncxx::builder::stream::document _document;
    bsoncxx::builder::stream::array _data;
    std::mutex _mutex, _startMutex;
    std::condition_variable _cv;
    std::queue<MessageLogger> _logsQueue;
    std::thread _logThread;
};