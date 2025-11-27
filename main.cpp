#include <iostream>
#include <deque>
#include <vector>
#include <functional>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <cmath>

using namespace std;

void EnableUkrainian() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);
}

class Coroutine {
public:
    function<void()> body;
    bool finished = false;
    Coroutine(function<void()> f) : body(move(f)) {}
    void resume() { if (!finished && body) body(); }
};

class Scheduler {
    vector<Coroutine*> coroutines;
    Coroutine* current = nullptr;
public:
    void spawn(Coroutine* co) { coroutines.push_back(co); }
    static void yield() {}
    void run() {
        while (true) {
            bool done = true;
            for (auto* co : coroutines) {
                if (!co->finished) {
                    done = false;
                    current = co;
                    co->resume();
                }
            }
            if (done) break;
        }
    }
    static Coroutine* get_current() { return instance->current; }
    static Scheduler* instance;
};

Scheduler* Scheduler::instance = nullptr;

void average_coroutine() {
    deque<double> window;
    vector<double> last_three;

    wcout << L"Вводьте числа:\n\n";

    while (true) {
        double x;
        wcout << L"> ";
        wcout.flush();

        if (!(wcin >> x)) {
            wcout << L"\nПомилка вводу. Програма завершена.\n";
            Scheduler::get_current()->finished = true;
            return;
        }

        window.push_back(x);
        if (window.size() > 10) window.pop_front();

        last_three.push_back(x);
        if (last_three.size() > 3) last_three.erase(last_three.begin());

        if (window.size() == 10) {
            double sum = 0;
            for (double v : window) sum += v;
            wcout << L"Середнє значення останніх 10 чисел: " << sum / 10.0 << L"\n";
        }

        if (last_three.size() == 3 &&
            abs(last_three[0] - last_three[1]) < 1e-9 &&
            abs(last_three[1] - last_three[2]) < 1e-9) {

            wcout << L"\nТри однакові значення (" << last_three[0] << L") поспіль — сопрограма призупиняється.\n";
            wcout << L"Натисніть Enter...\n";
            Scheduler::yield();
            wcout << L"Сопрограма відновлена після призупинки.\n\n";
            last_three.clear();
        }
    }
}

int main() {
    EnableUkrainian();

    Scheduler scheduler;
    Scheduler::instance = &scheduler;

    auto* co = new Coroutine(average_coroutine);
    scheduler.spawn(co);
    scheduler.run();

    delete co;
    return 0;
}