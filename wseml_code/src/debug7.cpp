#include <iostream>
#include <fstream>
#include <exception>
#include <cstdlib>
#include "wseml7.hpp"


using namespace std;
using nlohmann::json;

int main(int argc, char *argv[]) {
    std::locale system("");
    std::locale::global(system);

    wstring str = L"hello, world. привет, МКС";
    wcout << str << endl;
    wcout << str.c_str() << endl;
    try {
        //"Использование: wseml имя_программы.w имя_журнала.l"
        init_log(argv[2], 15);
        init(argv[1]);
        debug();
        return EXIT_SUCCESS;
    }
    catch (exception *e) {
        cerr << "Ошибка: " << e->what() << endl;
        delete e;
        return EXIT_FAILURE;
    }
}
