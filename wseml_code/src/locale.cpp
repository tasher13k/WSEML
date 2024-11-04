#include <iostream>
#include <clocale>

int setup_locale()
{
    std::setlocale(LC_ALL, "");
    std::locale loc("ru_RU.UTF-8");	// Указываем локализацию
    std::locale::global(loc);
    std::cout.imbue(loc);
    std::cerr.imbue(loc);
    std::clog.imbue(loc);
    std::wcout.imbue(loc);
    std::wcerr.imbue(loc);
    std::wclog.imbue(loc);
    std::ios::sync_with_stdio(false);

    std::cout << "DORS! Я работаю here!" << std::endl;
    std::wcout << L"ДОРС is my место работы:" << std::endl;
}