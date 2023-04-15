#pragma once

#define PROFILER 0
#if PROFILER

#include <chrono>
#include <string>
#include "Utils/Log.h"

class Profiler
{
public:
    Profiler(const std::string& _context = "")
        : m_context(_context)
    {
        m_context.append(" ");
        m_t = std::chrono::high_resolution_clock::now();
    }

    ~Profiler()
    {
        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration_ms = t2 - m_t;

        LOG(m_context + tstr(duration_ms.count()));
    }

private:
    std::string m_context = "";
    std::chrono::system_clock::time_point m_t = {};
};

#define PROFILE_POINT(context) Profiler p(context);

#endif // PROFILER
