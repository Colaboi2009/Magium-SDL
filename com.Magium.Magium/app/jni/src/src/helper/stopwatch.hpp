#pragma once

#include <chrono>
#include <functional>

namespace MagiumSDL::Helper {
    class Stopwatch {
        using clock = std::chrono::high_resolution_clock;

        clock::time_point m_startTime;

    public:
        Stopwatch() {
            m_startTime = clock::now();
        }

        float elapsed() {
            return std::chrono::duration<float, std::chrono::seconds::period>(clock::now() - m_startTime).count();
        }
    };

    class DelayedFunction {
    private:
        float m_s;
        std::function<void(void)> m_func;
        Stopwatch m_sw;
    public:
        DelayedFunction(float s, std::function<void(void)> func) : m_s{s}, m_func{func}, m_sw{} {}

        bool checkCall() {
            if (m_s < m_sw.elapsed()){
                m_func();
                return true;
            }
            return false;
        }
    };
}