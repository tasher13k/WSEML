#ifndef W_H
#define W_H

#include <string>
#include <nlohmann/json.hpp>

#define RUNL 0
#define EXECL 1
#define RESOLVEL 2
#define LOADSTOREL 3

void init_log(const std::string &logfilename, int loglevel);
void init(const std::string &progname);

void run();

void debug();

#endif //W_H
