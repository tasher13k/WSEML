// Возможность комбинировать ссылки, например, навесить стек на регистр (параметр "где":ссылка)? Или хотя бы конвертировать, например, регистр в прямой (т. е. получить адрес)?
// Проблема: передача параметров между контекстами --- регистры заводятся не в том кадре, где упоминаются (между блоком, в котором упоминается регистр, и тем, в котором он используется, возникает реализация команды "взять"; хуже того, ссылка на регистр копируется из команды "взять" на вершину стека данных). Решение: метки контекста в объектах (только ссылках), приписываются при занесении чего-либо в стек данных (передаче параметра в другой контекст?), если такой метки еще не было, и сохраняются, если они уже были. Также учитываются при извлечении подобъектов. Другое решение: а) команда "взять" проставляет "происхождение" (двоичный адрес) ссылки при копировании ее в стек данных, и б) ищется последний кадр, в котором исполняется блок, являющийся отцом для "происхождения" ссылки.

#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <cmath>
#include <algorithm>
#include <nlohmann/json.hpp>
#include "wseml7.hpp"

#include "parser.h"
#include "WSEML.h"


using namespace std;
using namespace nlohmann;


static json universe =
        R"({
        "программа":[],
        "стек":[
                {"текблк":[{"вид":"двоичный", "адрес":0}],
                "текком":0,
                "данные":[]}
                ],
        "таблицы":{
                "команды":{"...":{"арг":0, "тело":[]}},
                "виды":{"...":{"арг":0, "тело":[[2], "порядок", {"имя":"вернуть", "рез":1}]}},
                "режимы":{"...":{"взять":{"арг":0, "тело":[{"имя":"вернуть", "рез":1}]}, "положить":{"арг":0, "тело":[]}}}
                 }
        })"_json;

//static WSEML universe_WSEML = parse(LR"({`программа':[], `стек':[{`текблк':[{`вид':`двоичный', `адрес':0}], `текком':0, `данные':[]}], `таблицы':{`команды':{`...':{`арг':0, `тело':[]}}, `виды':{`...':{`арг':0, `тело':[[2], `порядок', {`имя':`вернуть', `рез':1}]}}, `режимы':{`...':{`взять':{`арг':0, `тело':[{`имя':`вернуть', `рез':1}]}, `положить':{`арг':0, `тело':[]}}} }})");
wstring txt = LR"({`my cтрока str':$})";
static WSEML universe_WSEML = parse(
        txt); //, `стек':[{`текблк':[{`вид':`двоичный', `адрес':0}], `текком':0, `данные':[]}]}

static int log_level;

static ofstream logst;

static int digit(int l, int n) {
    return (l >> n) & 1;
}

void init(const string &progname) {
    // отладочный вывод содержания программы .w
    string getcontent;
    ifstream openfile(progname);
    if (openfile.is_open()) {
        while (std::getline(openfile, getcontent)) {
            cout << getcontent << endl;
        }
    }

    std::cout << "universe DUMP = " << universe.dump() << "\n";

    std::wstring s = pack(universe_WSEML);
    std::wcout << "\nWSEML_json = " << s << "\n";


    ifstream st(progname);
    st >> universe["программа"];
    universe["стек"][0]["текблк"][0]["адрес"] = (long long int) &universe["программа"];
}

void init_log(const string &logfilename, int loglevel) {
    log_level = loglevel;
    logst.open(logfilename);
    if (logst.is_open()) {
        std::cout << "Log file = " << logfilename << "\n";
    }
}

class Stop {
}; // это исключение кидается после подготовки нового стекового кадра для выполнения блока команд тела реализованной команды для выхода из глубины вызовов встроенных функций

typedef void(*proc)(); // тип реализованной команды

static json &frm() // самый верхний кадр, в котором выполняется реализованная команда
{
    int i;
    for (i = universe["стек"].size() - 1; universe["стек"][i]["текблк"].is_string(); --i);
    return universe["стек"][i];
}

static json &frm2() // второй сверху кадр, в котором выполняется реализованная команда
{
    int i;
    for (i = universe["стек"].size() - 1; universe["стек"][i]["текблк"].is_string(); --i);
    for (--i; universe["стек"][i]["текблк"].is_string(); --i);
    return universe["стек"][i];
}

static json &lfrm() // самый верхний кадр
{
    return universe["стек"][universe["стек"].size() - 1];
}

static bool has_label(const json &j) {
    return j.is_object() && j.contains("контекст");
}

static json *find_frame(json ref) {
    string l = ref["контекст"].get<string>();
    int i;
    //for(i = universe["стек"].size()-1; i>=0 && universe["стек"][i] != f; --i);//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

}

static json &frmm(const json &ref) {
    /*if(has_mark(ref))
    {
        return *(json *)ref["контекст"].get<long long int>();
    }*/
    if (!has_label(ref))
        return frm();
    json *ff = find_frame(ref);
    if (ff == nullptr)
        throw new invalid_argument("Метка не найдена.");
    return *ff;
}

static json &frmm2(json ref) {
    json *f = &frmm(ref);
    int i;
    for (i = universe["стек"].size() - 1; &universe["стек"][i] != f; --i);
    for (--i; universe["стек"][i]["текблк"].is_string(); --i);
    return universe["стек"][i];
}

static json pop() {
    json &ds = frm()["данные"];
    json x = ds.back();
    ds.erase(ds.size() - 1);
    return x;
}

static void push(const json &x) {
    json &ds = frm()["данные"];
    ds.push_back(x);
}

/*static bool is_ref(json j)
{
    return j.is_object() && j.contains("режим");
}

static json mark(json j, json m)
{
    if(j.is_number()||j.is_string())
        return j;
    if(is_ref(j) && !has_mark(j))
        j["контекст"] = m;
    if(j.is_array())
        for(json &e: j)
            e = mark(e, m);
    if(j.is_object())
        for (auto& [key, val] : j.items())
            val = mark(val, m);
    return j;
}*/

static void next_cmd() {
    int i = frm()["текком"].get<int>();
    frm()["текком"] = i + 1;
}

static void new_frame(const string &s) {
    universe["стек"].push_back(R"({"текблк":"", "текком":0, "данные":[], "кмд":0})"_json);
    lfrm()["текблк"] = s;

}

static void new_frame(json *bp) {
    universe["стек"].push_back(R"({"текблк":[{"вид":"двоичный", "адрес":0}], "текком":0, "данные":[], "кмд":0})"_json);
    lfrm()["текблк"][0]["адрес"] = (long long int) bp;
}

static void clean() // удалить самый верхний кадр
{
    universe["стек"].erase(universe["стек"].size() - 1);
}

static void shift() // перейти к следующей команде в вызывавшем блоке
{
    int i = frm2()["текком"].get<int>();
    frm2()["текком"] = i + 1;
    next_cmd();
    clean();
}

static void input() {
    json x;
    cin >> x;
    push(x);
    cout << "Отладочный universe 1 = " << universe << "\n" << endl;
    next_cmd();
    cout << "Отладочный universe 2 = " << universe << "\n" << endl;
    clean();
    cout << "Отладочный universe 3 = " << universe << "\n" << endl;

}

static void output() {
    cout << pop();
    next_cmd();
    clean();
}

#define FUNC1(F) static void w##F()       \
{                                         \
    json x = pop();                       \
    double y;                             \
    if(x.is_number_float())               \
        y = x.get<double>();              \
    else                                  \
        y = x.get<int>();                 \
    push(F(y));                           \
    next_cmd();                           \
    clean();                              \
}

FUNC1(sin);

FUNC1(cos);

FUNC1(asin);

FUNC1(acos);

FUNC1(tan);

FUNC1(atan);

FUNC1(sinh);

FUNC1(cosh);

FUNC1(asinh);

FUNC1(acosh);

FUNC1(tanh);

FUNC1(atanh);

FUNC1(log);

FUNC1(exp);

#define OPER1(NAME, OP) static void NAME()                   \
{                                                           \
    json x = pop();                                         \
    if(!x.is_number_integer())                              \
        throw new invalid_argument("Нужно целое число.");   \
    int y = x.get<int>();                                   \
    push(OP y);                                             \
    next_cmd();                                             \
    clean();                                                \
}

OPER1(bnot, ~)

OPER1(lnot, !)

#define OPER2(NAME, OP) static void NAME()       \
{                                               \
    json y = pop();                             \
    if(y.is_number_integer())                   \
    {                                           \
        int yi = y.get<int>();                  \
        json x = pop();                         \
        if(x.is_number_integer())               \
        {                                       \
            int xi = x.get<int>();              \
            push(xi OP yi);                     \
        }                                       \
        else                                    \
        {                                       \
            double xd = x.get<double>();        \
            push(xd OP yi);                     \
        }                                       \
    }                                           \
    else                                        \
    {                                           \
        double yd = y.get<double>();            \
        json x = pop();                         \
        double xd;                              \
        if(x.is_number_integer())               \
            xd = x.get<int>();                  \
        else                                    \
            xd = x.get<double>();               \
        push(xd OP yd);                         \
    }                                           \
    next_cmd();                                 \
    clean();                                    \
}

#define OPER2i(NAME, OP) static void NAME()                  \
{                                                           \
    json y = pop();                                         \
    if(!y.is_number_integer())                              \
        throw new invalid_argument("Нужно целое число.");   \
    int yi = y.get<int>();                                  \
    json x = pop();                                         \
    if(!x.is_number_integer())                              \
        throw new invalid_argument("Нужно целое число.");   \
    int xi = x.get<int>();                                  \
    push(xi OP yi);                                         \
    next_cmd();                                             \
    clean();                                                \
}

OPER2(add, +)

OPER2(sub, -)

OPER2(mul, *)

OPER2(div, /)

OPER2i(mod, %)

OPER2(lor, ||)

OPER2(land, &&)

OPER2i(bor, |)

OPER2i(band, &)

OPER2i(bxor, ^)

OPER2(gt, >)

OPER2(ge, >=)

OPER2(lt, <)

OPER2(le, <=)

OPER2(eq, ==)

OPER2(neq, <=)

static void power() {
    json y = pop();
    if (y.is_number_integer()) {
        int yi = y.get<int>();
        json x = pop();
        if (x.is_number_integer()) {
            int xi = x.get<int>();
            push(pow(xi, yi));
        } else {
            double xd = x.get<double>();
            push(pow(xd, yi));
        }
    } else {
        double yd = y.get<double>();
        json x = pop();
        double xd;
        if (x.is_number_integer())
            xd = x.get<int>();
        else
            xd = x.get<double>();
        push(pow(xd, yd));
    }
    next_cmd();
    clean();
}

static void at_get() {
    json y = pop();
    json s = pop();
    if (y.is_number_integer()) {
        int ind = y.get<int>();
        if (ind < 0) ind = ind + s.size();
        push(s[ind]);
    } else
        push(s[y.get<string>()]);
    next_cmd();
    clean();
}

static void at_set() {
    json x = pop();
    json y = pop();
    json o = pop();
    json no = o;
    if (y.is_number_integer()) {
        int ind = y.get<int>();
        if (ind < 0) ind = ind + o.size();
        no[ind] = x;
    } else
        no[y.get<string>()] = x;
    push(no);
    next_cmd();
    clean();
}

static void at_remove() {
    json y = pop();
    json x = pop();
    if (y.is_number_integer()) {
        int ind = y.get<int>();
        if (ind < 0) ind = ind + x.size();
        x.erase(ind);
    } else
        x.erase(y.get<string>());
    push(x);
    next_cmd();
    clean();
}

static void push_back() {
    json x = pop();
    json y = pop();
    y.push_back(x);
    push(y);
    next_cmd();
    clean();
}

static void reorder() {
    json order = pop();
    if (!order.is_array())
        throw new invalid_argument("Нужен массив.");
    int m = max_element(order.begin(), order.end())->get<int>();
    json tmp = json::array();
    for (int i = 0; i < m; ++i)
        tmp.push_back(pop());
    for (const auto &i: order)
        push(tmp[i.get<int>() - 1]);
    next_cmd();
    clean();
}
// чтение из регистра целого числа
#define regr(NAME) lfrm()[#NAME].get<long long int>()
// чтение из регистра объекта
#define regrj(NAME) lfrm()[#NAME]
// чтение из регистра строки
#define regrs(NAME) lfrm()[#NAME].get<string>()
// запись в регистр
#define regw(NAME, VAL) lfrm()[#NAME] = VAL

#define LOG(LEVEL, TEXT) if(digit(log_level, LEVEL)) logst<<TEXT<<endl
#define STACK   <<endl<<"     стек="<<universe["стек"]<<endl
#define IP      <<endl<<"     текком="<<regr(текком)<<endl


static void
resolve()//может быть вызвана как команда, тогда нужен next_cmd, или как процедура, тогда не нужен --- решает регистр кмд
{
    json res, j, j1, *frmo;
    string key;
    int ind;
    LOG(RESOLVEL, "resolve_начало:" IP);
    if (regr(текком) == 1) goto p1;
    /*j = json({{"и", 0}, {"адрес", 0}, {"в", ""}, {"вход", 0}});// заводим регистры
    lfrm().insert(j.begin(), j.end());*/
    regw(и, 0);
    regw(адрес, 0);
    regw(вход, pop());
    loop:
    if (regr(и) >= regrj(вход).size()) goto end;
    regw(в, regrj(вход)[regr(и)]["вид"]);
    if (regrs(в) != "двоичный") goto n1;
    regw(адрес, regrj(вход)[regr(и)]["адрес"]);
    LOG(RESOLVEL, "resolve_двоичный шаг:" STACK);
    goto next;
    n1:
    if (regrs(в) != "ключ") goto n2;
    key = regrj(вход)[regr(и)]["ключ"].get<string>();
    if (!((json *) regr(адрес))->contains(key))
        (*(json *) regr(адрес))[key] = json();
    regw(адрес, (long long int) &(*(json *) regr(адрес))[key]);
    LOG(RESOLVEL, "resolve_шаг ключ:" STACK);
    goto next;
    n2:
    if (regrs(в) != "индекс") goto n3;
    ind = regrj(вход)[regr(и)]["индекс"].get<int>();
    if (ind < 0) ind = ind + regrj(вход)[regr(и)].size();
    if (ind >= ((json *) regr(адрес))->size())
        for (int k = ((json *) regr(адрес))->size(); k <= ind; ++k)
            ((json *) regr(адрес))->push_back(json());
    regw(адрес, (long long int) &(*(json *) regr(адрес))[ind]);
    LOG(RESOLVEL, "resolve_шаг индекс:" STACK);
    goto next;
    n3:
    if (regrs(в) != "все") goto n4;
    regw(адрес, (long long int) &universe);
    LOG(RESOLVEL, "resolve_шаг все:" STACK);
    goto next;
    n4:
    if (!universe["таблицы"]["виды"].contains(regrs(в)))
        regw(в, "...");
    regw(текком, 1);
    frmo = &frm();
    j = regrj(адрес);
    j1 = regrj(вход)[regr(и)];
    key = regrs(в);
    LOG(RESOLVEL,
        "resolve_перед заведением нового стекового кадра для пользовательского шага:" STACK << "   j=" << j << "   j1="
                                                                                            << j1);
    new_frame(&universe["таблицы"]["виды"][key]["тело"]);
    LOG(RESOLVEL, "resolve_после заведения нового стекового кадра для пользовательского шага:" STACK);
    push(j);
    push(j1);
    LOG(RESOLVEL, "resolve_середина параметров для пользовательского шага:" STACK);
    for (int i = 0; i < universe["таблицы"]["виды"][key]["арг"].get<int>(); ++i) {
        push((*frmo)["данные"][(*frmo)["данные"].size() - universe["таблицы"]["виды"][key]["арг"].get<int>() + i]);
        (*frmo)["данные"].erase((*frmo)["данные"].size() - universe["таблицы"]["виды"][key]["арг"].get<int>() + i);
    }
    LOG(RESOLVEL, "resolve_перед пользовательским шагом:" STACK);
    throw Stop();
    p1:
    LOG(RESOLVEL, "resolve_после пользовательского шага:" STACK);
    regw(адрес, pop());
    LOG(RESOLVEL, "resolve_пользовательский шаг:" STACK);
    next:
    regw(и, regr(и) + 1);
    LOG(RESOLVEL, "resolve_конец тела цикла:" STACK);
    goto loop;
    end:
    LOG(RESOLVEL, "resolve_после цикла:" STACK);
    res = R"([{"вид":"двоичный", "адрес":0}])"_json;
    res[0]["адрес"] = regr(адрес);
    push(res);
    if (regr(кмд)) next_cmd();
    clean();
    LOG(RESOLVEL, "resolve_выход:" STACK);
}

static void load() {
    json blk_ptr, *blk, *cmd, ref, addr, bin_addr, j;
    string name;
    switch (regr(текком)) {
        case 1:
            goto p1;
        case 2:
            goto p2;
        case 3:
            goto end;
        case 4:
            goto p4;
    }
    /*j = json({{"ссылка", json()}});// заводим регистр для ссылки
    lfrm().insert(j.begin(), j.end());*/
    push(frm()["текблк"]);
    regw(текком, 1);
    new_frame("упростить");
    resolve();
    p1:
    blk_ptr = pop();
    blk = (json *) blk_ptr[0]["адрес"].get<long long int>();
    cmd = &(*blk)[frm()["текком"].get<int>()];
    if (cmd->contains("что")) {
        push((*cmd)["что"]);
        next_cmd();
        clean();
        return;
    }
    regw(ссылка, pop());
    ref = regrj(ссылка);
    if (ref["режим"] != "прямой") goto n1;
    addr = ref["адрес"];
    push(addr);
    regw(текком, 2);
    new_frame("упростить");
    resolve();
    p2:
    bin_addr = pop();
    push(*(json *) bin_addr[0]["адрес"].get<long long int>());
    goto end;
    n1:
    if (ref["режим"] != "регистр") goto n2;
    name = ref["имя"].get<string>();
    if (!frmm(ref).contains(name)) {
        json tmp = {{name, json()}};
        frmm(ref).insert(tmp.begin(), tmp.end());
    }
    push(frmm(ref)[name]);
    goto end;
    n2:
    if (ref["режим"] != "команда") goto n3;
    name = ref["имя"].get<string>();
    push(frmm2(ref)["текблк"]);
    regw(текком, 4);
    new_frame("упростить");
    resolve();
    p4:
    blk_ptr = pop();
    blk = (json *) blk_ptr[0]["адрес"].get<long long int>();
    cmd = &(*blk)[frmm2(regrj(ссылка))["текком"].get<int>()];
    if (!cmd->contains(name)) {
        json tmp = {{name, json()}};
        cmd->insert(tmp.begin(), tmp.end());
    }
    push((*cmd)[name]);
    goto end;
    n3:
    name = ref["режим"].get<string>();
    if (!universe["таблицы"]["режимы"].contains(name))
        name = "...";
    regw(текком, 3);
    new_frame(&universe["таблицы"]["режимы"][name]["взять"]["тело"]);
    LOG(LOADSTOREL, "load_после заведения нового стекового кадра для пользовательского шага:" STACK);
    push(ref);
    LOG(LOADSTOREL, "load_перед пользовательским шагом:" STACK);
    throw Stop();
    end:
    next_cmd();
    clean();
}

static void store() {
    json blk_ptr, *blk, *cmd, ref, val, addr, bin_addr, j;
    string name;
    LOG(LOADSTOREL, "store_начало:" STACK IP);
    if (regr(текком) == 1) goto p1;
    if (regr(текком) == 2) goto p2;
    if (regr(текком) == 3) goto end;
    if (regr(текком) == 4) goto p4;
    regw(ссылка, pop());
    regw(значение, pop());
    LOG(LOADSTOREL, "store_после чтения параметров:" STACK);
    push(frm()["текблк"]);
    LOG(LOADSTOREL, "store_перед уплощением указателя 1:" STACK);
    regw(текком, 1);
    LOG(LOADSTOREL, "store_перед уплощением указателя 2:" STACK);
    new_frame("упростить");
    LOG(LOADSTOREL, "store_перед уплощением указателя 3:" STACK);
    resolve();
    p1:
    LOG(LOADSTOREL, "store_после уплощения указателя 1:" STACK);
    blk_ptr = pop();
    LOG(LOADSTOREL, "store_после уплощения указателя 2:" STACK);
    blk = (json *) blk_ptr[0]["адрес"].get<long long int>();
    LOG(LOADSTOREL, "store_после уплощения указателя 3:" STACK);
    cmd = &(*blk)[frm()["текком"].get<int>()];
    LOG(LOADSTOREL, "store_после уплощения указателя 4:" STACK);
    ref = regrj(ссылка);
    LOG(LOADSTOREL, "store_после уплощения указателя 5:" STACK);
    val = regrj(значение);
    LOG(LOADSTOREL, "store_после уплощения указателя 6:" STACK);
    if (ref["режим"] != "прямой") goto n1;
    addr = ref["адрес"];
    push(addr);
    regw(текком, 2);
    new_frame("упростить");
    resolve();
    p2:
    bin_addr = pop();
    *(json *) bin_addr[0]["адрес"].get<long long int>() = regrj(значение);
    goto end;
    n1:
    if (ref["режим"] != "регистр") goto n2;
    LOG(LOADSTOREL, "store_шаг регистр 1:" STACK);
    name = ref["имя"].get<string>();
    LOG(LOADSTOREL,
        "store_шаг регистр 2:" STACK << "  ref=" << ref << "   name=" << name << "    frmm(ref)="/*<<frmm(ref)*/
                                     << "    val=" << val);
    frmm(ref)[name] = val;
    LOG(LOADSTOREL, "store_шаг регистр 3:" STACK);
    goto end;
    n2:
    if (ref["режим"] != "команда") goto n3;
    name = ref["имя"].get<string>();
    push(frmm2(ref)["текблк"]);
    regw(текком, 4);
    new_frame("упростить");
    resolve();
    p4:
    blk_ptr = pop();
    blk = (json *) blk_ptr[0]["адрес"].get<long long int>();
    cmd = &(*blk)[frmm2(regrj(ссылка))["текком"].get<int>()];
    (*cmd)[name] = regrj(значение);
    goto end;
    n3:
    name = ref["режим"].get<string>();
    if (!universe["таблицы"]["режимы"].contains(name))
        name = "...";
    regw(текком, 3);
    new_frame(&universe["таблицы"]["режимы"][name]["положить"]["тело"]);
    LOG(LOADSTOREL, "resolve_после заведения нового стекового кадра для пользовательского шага:" STACK);
    push(val);
    push(ref);
    LOG(LOADSTOREL, "resolve_перед пользовательским шагом:" STACK);
    throw Stop();
    end:
    LOG(LOADSTOREL, "store_конец 1:" STACK);
    next_cmd();
    LOG(LOADSTOREL, "store_конец 2:" STACK);
    clean();
    LOG(LOADSTOREL, "store_конец 3:" STACK);
}

static void label() {
    next_cmd();
    clean();
}

static void goto_c(json target = json()) {
    json blk_ptr, *blk, *cmd, ref, val, addr, bin_addr;
    string name;
    if (regr(текком) == 1) goto p1;
    push(frm()["текблк"]);
    regw(текком, 1);
    new_frame("упростить");
    resolve();
    p1:
    blk_ptr = pop();
    blk = (json *) blk_ptr[0]["адрес"].get<long long int>();
    cmd = &(*blk)[frm()["текком"].get<int>()];
    if (target == json()) target = (*cmd)["куда"];
    for (int i = 0; i < blk->size(); ++i)
        if ((*blk)[i].is_object() && (*blk)[i].contains("имя")
            && (*blk)[i]["имя"] == "метка" && (*blk)[i]["метка"] == target) {
            frm()["текком"] = i;
            clean();
            return;
        }
    throw new invalid_argument("Метка не найдена.");
}

static void goto_cmd() {
    goto_c();
}

static void branch() {
    json blk_ptr, *blk, *cmd, cond, addr, bin_addr;
    string name;
    if (regr(текком) == 1) goto p1;
    if (regr(текком) == 2) goto end;
    push(frm()["текблк"]);
    regw(текком, 1);
    new_frame("упростить");
    resolve();
    p1:
    blk_ptr = pop();
    blk = (json *) blk_ptr[0]["адрес"].get<long long int>();
    cmd = &(*blk)[frm()["текком"].get<int>()];
    cond = pop();
    if (!cond.is_boolean())
        throw new invalid_argument("Нужно логическое значение.");
    regw(текком, 2);
    if (!cond) goto n2;
    new_frame("переход");
    goto_c((*cmd)["да"]);
    goto end;
    n2:
    if (!cmd->contains("нет")) goto n3;
    new_frame("переход");
    goto_c((*cmd)["нет"]);
    goto end;
    n3:
    next_cmd();
    end:
    clean();
}

static void exec_cmd();

static void return_c() {
    json blk_ptr, *blk, *cmd, cond, addr, bin_addr;
    string name;
    if (regr(текком) == 1) goto p1;
    push(frm()["текблк"]);
    regw(текком, 1);
    new_frame("упростить");
    resolve();
    p1:
    blk_ptr = pop();
    blk = (json *) blk_ptr[0]["адрес"].get<long long int>();
    cmd = &(*blk)[frm()["текком"].get<int>()];
    for (int i = 0; i < (*cmd)["рез"].get<int>(); ++i)
        frm2()["данные"].push_back(frm()["данные"][frm()["данные"].size() + i - (*cmd)["рез"].get<int>()]);
    frm()["текком"] = blk->size(); // подготовить остановку выполнения верхнего кадра, в котором выполняется блок команд
    clean();
}

static map<string, proc> exec_cmd_by_name = {
        {"+",              add},
        {"-",              sub},
        {"*",              mul},
        {"/",              div},
        {"%",              mod},
        {"**",             power},
        {"sin",            wsin},
        {"cos",            wcos},
        {"arcsin",         wasin},
        {"arccos",         wacos},
        {"tg",             wtan},
        {"arctg",          watan},
        {"sh",             wsinh},
        {"ch",             wcosh},
        {"arcsh",          wasinh},
        {"arcch",          wacosh},
        {"th",             wtanh},
        {"arcth",          watanh},
        {"ln",             wlog},
        {"exp",            wexp},
        {"||",             lor},
        {"&&",             land},
        {"!",              lnot},
        {"|",              bor},
        {"&",              band},
        {"^",              bxor},
        {"~",              bnot},
        {">",              gt},
        {">=",             ge},
        {"<",              lt},
        {"<=",             le},
        {"=",              eq},
        {"!=",             neq},
        {"порядок",        reorder},
        {"ввод",           input},
        {"вывод",          output},
        {"взять",          load},
        {"положить",       store},
        {"метка",          label},
        {"переход",        goto_cmd},
        {"если",           branch},
        {"выполнить",      exec_cmd},
        {"вернуть",        return_c},
        {"сдвинуть",       shift},
        {"упростить",      resolve},
        {"часть_взять",    at_get}, // стек объект индекс ---> стек объект[индекс]
        {"часть_положить", at_set}, // стек объект индекс новый_элемент ---> стек новый_объект
        {"часть_удалить",  at_remove}, // стек объект индекс ---> стек новый_объект
        {"добавить",       push_back}  // стек массив новый_элемент ---> стек новый_массив
};

static void exec_cmd() {
    json blk_ptr, *blk, *cmd, cond, *frmo;
    string cmd_name;
    LOG(EXECL, "exec_cmd_начало:" IP);
    if (regr(текком) == 1) goto p1;
    if (regr(текком) == 2) goto end;
    push(frm()["текблк"]);
    regw(текком, 1);
    new_frame("упростить");
    LOG(EXECL, "exec_cmd_перед вызовом resolve:" STACK);
    resolve();
    p1:
    LOG(EXECL, "exec_cmd_после вызова resolve:" STACK);
    blk_ptr = pop();
    LOG(EXECL, "exec_cmd_после получения адреса блока: blk_ptr=" << blk_ptr);
    blk = (json *) blk_ptr[0]["адрес"].get<long long int>();
    LOG(EXECL, "exec_cmd_после получения блока: *blk=" << *blk << "   frm()[текком]=" << frm()["текком"]);
    if (frm()["текком"] >= blk->size()) {
        LOG(EXECL, "exec_cmd_останов текущего блока, перед очисткой стека:" STACK);
        for (int i = universe["стек"].size() - 1; universe["стек"][i]["текблк"].is_string(); --i) clean();
        clean();
        LOG(EXECL, "exec_cmd_останов текущего блока, после очистки стека:" STACK);
        throw Stop();
    }
    cmd = &(*blk)[frm()["текком"].get<int>()];
    LOG(EXECL, "exec_cmd_после получения команды: *cmd=" << *cmd);
    frmo = &frm();
    if (!(cmd->is_number() || cmd->is_array())) goto other;
    LOG(EXECL, "exec_cmd_добавить в стек число или массив:" STACK);
    push(*cmd);
    LOG(EXECL, "exec_cmd_перейти к следующей команде:" STACK);
    next_cmd();
    LOG(EXECL, "exec_cmd_перешли к следующей команде:" STACK);
    goto end;
    other:
    regw(текком, 2);
    if (cmd->is_string()) cmd_name = cmd->get<string>();
    else cmd_name = (*cmd)["имя"].get<string>();
    LOG(EXECL, "exec_cmd_получено имя команды: cmd_name=" << cmd_name);
    if (exec_cmd_by_name.count(cmd_name) == 0) goto user;
    new_frame(cmd_name);
    regw(кмд, 1);
    LOG(EXECL, "exec_cmd_перед запуском встроенной команды:" STACK);
    exec_cmd_by_name[cmd_name]();
    goto end;
    user:
    if (!universe["таблицы"]["команды"].contains(cmd_name)) cmd_name = "...";
    new_frame(&universe["таблицы"]["команды"][cmd_name]["тело"]);
    LOG(EXECL, "exec_cmd_перед запуском пользовательской команды:" STACK << "   #args="
                                                                         << universe["таблицы"]["команды"][cmd_name]["арг"]
                                                                         << endl << "============================="
                                                                         << endl << universe);
    for (int i = 0; i < universe["таблицы"]["команды"][cmd_name]["арг"].get<int>(); ++i) {
        push((*frmo)["данные"][(*frmo)["данные"].size() - universe["таблицы"]["команды"][cmd_name]["арг"].get<int>() +
                               i]);
        (*frmo)["данные"].erase(
                (*frmo)["данные"].size() - universe["таблицы"]["команды"][cmd_name]["арг"].get<int>() + i);
    }
    LOG(EXECL, "exec_cmd_перед запуском пользовательской команды:" STACK);
    throw Stop();
    end:
    clean();
    LOG(EXECL, "exec_cmd_выход: стек=" STACK);
}

void run() {
    while (universe["стек"].size() > 0)
        try {
            LOG(RUNL, "run_начало:" STACK);
            if (!lfrm()["текблк"].is_string()) {
                new_frame("выполнить");
                LOG(RUNL, "run_середина:" STACK);
            }
            exec_cmd_by_name[lfrm()["текблк"].get<string>()]();
        }
        catch (Stop) {
            LOG(RUNL, "run_Stop:" STACK);
        }
}

void debug() {
    int c;
    string s;
    while (universe["стек"].size() > 0) {
        LOG(RUNL, "debug_начало:" STACK);
        cout << "0. Выход." << endl;
        cout << "1. Обыкновенный шаг." << endl;
        cout << "2. Выбранная команда." << endl;
        cout << "3. Выполнение до тех пор, пока размер стека больше указанного значения." << endl;
        cin >> c;
        switch (c) {
            case 0:
                return;
            case 1:
                cout << "============================ очередной шаг ==================================" << endl;
                try {
                    if (!lfrm()["текблк"].is_string()) {
                        new_frame("выполнить");
                        LOG(RUNL, "_середина:" STACK);
                    }
                    exec_cmd_by_name[lfrm()["текблк"].get<string>()]();
                }
                catch (Stop) {
                    LOG(RUNL, "_Stop:" STACK);
                }
                cout << universe << endl;
                cout << "Число элементов в стеке=" << universe["стек"].size() << endl;
                break;
            case 2:
                cout << "============================ выбранная команда ==================================" << endl;
                cin >> s;
                new_frame("выполнить");
                exec_cmd_by_name[s]();
                cout << universe << endl;
                cout << "Число элементов в стеке=" << universe["стек"].size() << endl;
                break;
            case 3:
                cout << "============================ продолжение ==================================" << endl;
                cin >> c;
                do
                    try {
                        if (!lfrm()["текблк"].is_string()) {
                            new_frame("выполнить");
                            LOG(RUNL, "_середина:" STACK);
                        }
                        exec_cmd_by_name[lfrm()["текблк"].get<string>()]();
                    }
                    catch (Stop) {
                        LOG(RUNL, "_Stop:" STACK);
                    }
                while (universe["стек"].size() > c);
                cout << universe << endl;
                break;
        }
    }
}