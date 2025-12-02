#include <iostream>
#include <deque>
#include <coroutine>
#include <cmath>
#include <windows.h>
#include <io.h>
#include <fcntl.h>

using namespace std;

void SetupConsole() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);
}

struct InputAwaiter {
    static inline deque<double> window;
    static inline double prev1 = 0, prev2 = 0;
    static inline bool first = true, second = true;

    bool await_ready() const noexcept { return false; }

    void await_suspend(coroutine_handle<> h) noexcept {
        double value;
        wcout << L"> ";
        wcout.flush();

        if (!(wcin >> value)) {
            wcout << L"\nПомилка вводу. Програма завершена.\n";
            h.destroy();
            return;
        }

        window.push_back(value);
        if (window.size() > 10) window.pop_front();

        if (window.size() == 10) {
            double sum = 0.0;
            for (double x : window) sum += x;
            wcout << L"Середнє значення останніх 10 чисел: " << sum / 10.0 << L"\n";
        }

        if (!first && !second &&
            abs(prev1 - prev2) < 1e-9 && abs(prev2 - value) < 1e-9) {
            wcout << L"\nТри однакові (" << value << L") поспіль — сопрограма призупиняється.\n";
            wcout << L"Натисніть Enter...\n";
            wcin.ignore(10000, '\n');
            wcin.get();
            wcout << L"Сопрограма відновлена після зупинки.\n\n";
        }

        prev1 = prev2;
        prev2 = value;
        first = second = false;

        h.resume();
    }

    void await_resume() const noexcept {}
};

struct Task {
    struct promise_type {
        Task get_return_object() { return {}; }
        suspend_never initial_suspend() { return {}; }
        suspend_always final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() { terminate(); }

        InputAwaiter await_transform(int) const noexcept {
            return {};
        }
    };
};

Task average_coroutine() {
    wcout << L"Вводьте числа:\n\n";
    while (true) {
        co_await 0;
    }
}

int main() {
    SetupConsole();
    average_coroutine();
    wcout << L"\nПрограма завершена.\n";
    return 0;
}