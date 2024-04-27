#include <thread>
#include <mutex>
#include <iostream>
#include <deque>
#include <vector>
#include <condition_variable>

std::mutex g_mutex;
std::deque<uint8_t> g_deque;
std::condition_variable g_condition;

namespace LHSpace{
    class LHCout{
    public:
        template <typename T>
        typename std::enable_if<std::is_same<T, uint8_t>::value, LHCout&>::type operator<<(const T& num)
        {
            std::cout << static_cast<int> (num);
            return *this;
        }

        LHCout& operator<<(std::ostream& (*manipulator)(std::ostream&))
        {
            std::cout << manipulator;
            return *this;
        }
    };
}

void SetPrint(const uint8_t val)
{
    LHSpace::LHCout lhCout;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        std::cout << "Write Thread ID: " << std::this_thread::get_id() << std::endl;
        g_deque.push_back(val);
        std::cout << "deque push: ";
        lhCout << val << std::endl;
    }
    g_condition.notify_one();
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void GetPrint()
{
    LHSpace::LHCout lhCout;
    while(true)
    {
        std::unique_lock<std::mutex> lock(g_mutex);
        g_condition.wait(lock, [] {return !g_deque.empty();});
        const uint8_t FRONTNUM = g_deque.front();
        g_deque.pop_front();
        lock.unlock();
        std::cout << "Read Thread ID: " << std::this_thread::get_id() << std::endl;
        std::cout << "deque pop: ";
        lhCout << FRONTNUM << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int main(int argc, const char** argv)
{
    LHSpace::LHCout lhCout;
    std::vector<std::thread> writeThreads;
    for(uint8_t i = 0U; i < 10U; i++)
    {
        writeThreads.push_back(std::thread(SetPrint, i));
    }

    std::thread getThread(GetPrint);
    for(uint8_t i = 0u; i < 10U; i++)
    {
        writeThreads[i].join();
    }

    getThread.join();
    return 0;
}