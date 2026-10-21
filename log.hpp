#include <chrono>
#include <format>
#include <iostream>

class Log {
public:
    enum Level {
        NONE = 0,
        INFO = 1,
    };

    static void init(Level level) {
        _logger = new Log(level);
    }

    template< class... Args >
    static void info(std::string_view fmt, Args&&... args) {
        if (_logger && _logger->log_level >= INFO) {
            _logger->_print("INFO", fmt, args...);
        }
    }

private:

    inline static Log* _logger = nullptr;

    const Level log_level;
    const std::chrono::steady_clock::time_point start_time;

    Log(Level level): log_level(level), start_time(std::chrono::steady_clock::now()) {};

    template< class... Args >
    void _print(std::string_view log_type, std::string_view fmt, Args&&... args) {
        using namespace std::chrono;
        double ms_elapsed = duration_cast<milliseconds>(steady_clock::now() - start_time).count();
        std::cout << std::vformat(std::format("[{}] {:8.2f}ms {}\n", log_type, ms_elapsed, fmt), std::make_format_args(args...));
    }
};
