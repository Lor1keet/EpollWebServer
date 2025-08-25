#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <memory>
#include <unordered_map>
#include <format>

#ifndef PROJECT_NAME
#define PROJECT_NAME "Unknown"
#endif

namespace utils
{
    class Log
    {
    public:
        static Log* Init()
        {
            static Log* instance = new Log(PROJECT_NAME);
            return instance;
        }

        spdlog::logger* get() const
        {
            return m_logger.get();
        }

    private:
        Log(std::string name)
        {
            std::vector<spdlog::sink_ptr> sinks;

            auto sink1 = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            sinks.push_back(sink1);
            auto sink2 = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(name + ".log", 1024 * 1024 * 10, 10, false);
            sinks.push_back(sink2);

            m_logger = std::make_shared<spdlog::logger>("global", begin(sinks), end(sinks));

            m_logger->set_pattern("[%D %S %T] [%s:%#] [%^%l%$] : %v");
            m_logger->set_level(spdlog::level::trace);

            SPDLOG_LOGGER_INFO(m_logger, "Logger init successful");
        }

    private:
        std::shared_ptr<spdlog::logger> m_logger;
    };

    inline Log* const Logger = Log::Init();

#define LOG_INFO(...) (SPDLOG_LOGGER_INFO(::utils::Logger->get(), std::format(__VA_ARGS__)));
#define LOG_TRACE(...) (SPDLOG_LOGGER_TRACE(::utils::Logger->get(), std::format(__VA_ARGS__)));
#define LOG_DEBUG(...) (SPDLOG_LOGGER_DEBUG(::utils::Logger->get(), std::format(__VA_ARGS__)));
#define LOG_WARN(...) (SPDLOG_LOGGER_WARN(::utils::Logger->get(), std::format(__VA_ARGS__)));
#define LOG_ERROR(...) (SPDLOG_LOGGER_ERROR(::utils::Logger->get(), std::format(__VA_ARGS__)));
}