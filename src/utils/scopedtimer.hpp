#ifndef ACCOUNTANCY_SCOPEDTIMER_HPP
#define ACCOUNTANCY_SCOPEDTIMER_HPP

#include <chrono>
#include <iostream>

class ScopedTimer
{
public:
    explicit ScopedTimer(const char* name)
        : m_Name(name)
    {
        m_StartTimePoint = std::chrono::high_resolution_clock::now();
    }

    ~ScopedTimer()
    {
        if (!m_Stopped)
            Stop();
    }

    void Stop()
    {
        auto endTimePoint = std::chrono::high_resolution_clock::now();

        long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimePoint).time_since_epoch().count();
        long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();

        long long duration = end - start;
        double ms = static_cast<double>(duration) * 0.001;

        std::cout << m_Name << ": " << duration << "us (" << ms << "ms \n";

        m_Stopped = true;
    }


private:
    const char* m_Name;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimePoint;
    bool m_Stopped = false;
};

#endif//ACCOUNTANCY_SCOPEDTIMER_HPP
