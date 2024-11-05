#include "WSEML.h"
#include "pointers.h"
#include "dllconfig.h"
#include "parser.h"
#include <sstream>
#include <gmpxx.h>

#include "stringconverter.h"

std::wstring periodToFrac(std::wstring &s) {
    size_t dotPos = s.find(L'.');
    size_t start = s.find(L"(L");
    size_t end = s.find(L')');
    std::wstring real = s.substr(0, start);
    size_t l = start - dotPos - 1;
    size_t r = end - dotPos - 2;

    mpz_t pow1, pow2, ten;
    mpz_init(pow1);
    mpz_init(pow2);
    mpz_init(ten);
    mpz_set_str(ten, "10", 10);
    mpz_pow_ui(pow1, ten, r);
    mpz_pow_ui(pow2, ten, l);

    mpz_class pow1_c(pow1), pow2_c(pow2), den, num, n1, n2;
    den = pow1_c - pow2_c;

    real.erase(dotPos, 1);
    std::wstring period = real + s.substr(start + 1, end - start - 1);
    n1 = strconverter.to_bytes(period);
    n2 = {real.begin(), real.end()};
    num = n1 - n2;
    mpz_clear(pow1);
    mpz_clear(pow2);
    mpz_clear(ten);
    mpq_class d(num.get_str() + "/" + den.get_str());
    d.canonicalize();
    return strconverter.from_bytes(d.get_str());
}

bool isNum(std::wstring &s) {
    size_t i = 0;
    size_t len = s.size();
    if (s[i] == '+' || s[i] == '-') i++;

    bool dot{false}, frac{false};
    while (i < len && (std::isdigit(s[i]) || (s[i] == '.') || (s[i] == '/'))) {
        if (dot && (s[i] == '.')) return false;
        if (frac && (s[i] == '/')) return false;
        dot += (s[i] == '.');
        frac += (s[i] == '/');
        i++;
    }
    if (dot && frac) return false;
    if (i < len && s[i] == 'e' && !frac) {
        if (++i < len && (s[i] == '+' || s[i] == '-'))
            i++;
        if (i == len || (!isdigit(s[i])))
            return false;
        while (i < len && (isdigit(s[i])))
            i++;
    }
    if (!isdigit(s[len - 1])) return false;
    return (i == len);
}

WSEML *extract(WSEML &ref) {
    List *resList = dynamic_cast<List *>(ref.getObj());
    if (ref == NULLOBJ) return nullptr;
    if (resList->find(L"type") == WSEML(L"i")) {
        return &resList->find(L"1");
    } else {
        WSEML &subRef = resList->find(L"1");
        if (subRef.getType() == WSEML(L"ref"))
            return extract(subRef);
        else extractObj(subRef);
    }
    return nullptr;
}

WSEML getLength(const WSEML &Args) {
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *list;
    list = extract(args->find(L"list"));
    size_t length = dynamic_cast<List *>(list->getObj())->get().size();
    return {std::to_wstring(length)};
}

WSEML getKeyByData(const WSEML &Args) {
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *list, *data;
    list = extract(args->find(L"list"));
    data = extract(args->find(L"data"));
    std::list<Pair> &listList = dynamic_cast<List *>(list->getObj())->get();
    auto it = listList.begin();
    WSEML key;
    while (it != listList.end()) {
        if (it->getData() == *data)
            key = it->getKey();
        it++;
    }
    return key;
}

WSEML insertPair(const WSEML &Args) {
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *list, *pairData, *I;
    list = extract(args->find(L"list"));
    pairData = extract(args->find(L"pair"));
    I = extract(args->find(L"I"));

    List *listList = dynamic_cast<List *>(list->getObj());

    if (I == nullptr) {
        listList->append(list, *pairData, pairData->getPair()->getKey(), pairData->getPair()->getKeyRole(),
                         pairData->getPair()->getData());
    } else {
        size_t ind = std::stoi(dynamic_cast<ByteString *>(I->getObj())->get());
        listList->insert(ind, list, *pairData, pairData->getPair()->getKey(), pairData->getPair()->getKeyRole(),
                         pairData->getPair()->getData());
    }
    return {};
}

WSEML isKeyExists(const WSEML &Args) {
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *list, *key;
    list = extract(args->find(L"list"));
    key = extract(args->find(L"key"));
    std::list<Pair> &listList = dynamic_cast<List *>(list->getObj())->get();
    auto it = listList.begin();
    std::wstring res = L"0";
    while (it != listList.end()) {
        if (it->getKey() == *key) {
            res = L"1";
            break;
        }
        it++;
    }
    return {L"res"};
}

bool compare(WSEML *O1, WSEML *O2, const std::wstring &type) {
    bool comp = (type == L"less");
    if (*O1 == NULLOBJ) return comp;
    if (*O2 == NULLOBJ) return !comp;
    if (O1->typeInfo() == StringType && O2->typeInfo() == ListType)
        return comp;
    if (O1->typeInfo() == ListType && O2->typeInfo() == StringType)
        return !comp;
    if (O1->typeInfo() == StringType && O2->typeInfo() == StringType) {
        std::wstring O1_str = dynamic_cast<ByteString *>(O1->getObj())->get();
        std::wstring O2_str = dynamic_cast<ByteString *>(O2->getObj())->get();
        if (isNum(O1_str) && isNum(O2_str)) {
            if (O1_str.find('/')) {
                mpq_class o1({O1_str.begin(), O1_str.end()});
                if (O2_str.find('/')) {
                    mpq_class o2({O2_str.begin(), O2_str.end()});
                    return (comp) ? (o1 < o2) : (o1 > o2);
                }
                mpf_class o2({O2_str.begin(), O2_str.end()});
                return (comp) ? (o1 < o2) : (o1 > o2);
            } else {
                mpf_class o1({O1_str.begin(), O1_str.end()});
                if (O2_str.find('/')) {
                    mpq_class o2({O2_str.begin(), O2_str.end()});
                    return (comp) ? (o1 < o2) : (o1 > o2);
                }
                mpf_class o2({O2_str.begin(), O2_str.end()});
                return (comp) ? (o1 < o2) : (o1 > o2);
            }
        } else return (comp) ? (O1_str < O2_str) : (O1_str > O2_str);
    }
    if (O1->typeInfo() == ListType && O2->typeInfo() == ListType) {
        List *O1_list = dynamic_cast<List *>(O1->getObj());
        List *O2_list = dynamic_cast<List *>(O2->getObj());
        auto O1_it = O1_list->get().begin();
        auto O2_it = O2_list->get().begin();
        while (O1_it != O1_list->get().end() && O2_it != O1_list->get().end()) {
            if (O1_it->getData() == O2_it->getData()) {
                O1_it++;
                O2_it++;
            } else return compare(&O1_it->getData(), &O2_it->getData(), type);
        }
    }
    return true;
}

WSEML createEquiv(WSEML *stack, WSEML *wfrm, WSEML *frm, const std::wstring &cmdName, const std::wstring &cmdInd) {
    WSEML new_frm = parse(
            L"{ip:$[1:$[t:r]ps, 2:$[t:k, k:data]ps, 3:$[t:k, k:" + cmdName + L"]ps, 4:$[t:k, k:" + cmdInd +
            L" ]ps]ptr, pred:{}, next:{}, origin:nd}");
    new_frm.setType(WSEML(L"frm"));
    List *stack_list = dynamic_cast<List *>(stack->getObj());
    List *new_frm_list = dynamic_cast<List *>(new_frm.getObj());
    List *wfrm_list = dynamic_cast<List *>(wfrm->getObj());
    WSEML &pred = new_frm_list->find(L"pred");
    List *pred_list = dynamic_cast<List *>(pred.getObj());

    WSEML frm_key = frm->getPair()->getKey();
    auto it = wfrm_list->get().begin();
    while (it != wfrm_list->get().begin() || it->getData() != frm_key) it++;
    pred_list->append(&pred, it->getData(), it->getKey(), it->getKeyRole(), it->getDataRole());
    wfrm_list->get().erase(it);
    WSEML equivKey = stack_list->append(stack, new_frm);
    wfrm_list->append(wfrm, equivKey);
    return equivKey;
}

void changeCommand(List *stack, const WSEML &equivKey, const std::wstring &newCmdInd) {
    List *equiv = dynamic_cast<List *>(stack->find(equivKey).getObj());
    List *ip = dynamic_cast<List *>(equiv->find(L"ip").getObj());
    List *cmdIndPs = dynamic_cast<List *>(ip->find(L"4").getObj());
    cmdIndPs->find(L"k") = WSEML(newCmdInd);
}

void clear(List *stack, List *data, WSEML *wfrm, const WSEML &equivKey, const WSEML &DataKeys) {
    List *equivFrm = dynamic_cast<List *>(stack->find(equivKey).getObj());
    List *keys = dynamic_cast<List *>(DataKeys.getObj());
    List *pred = dynamic_cast<List *>(equivFrm->find(L"pred").getObj());
    List *wfrmList = dynamic_cast<List *>(wfrm->getObj());
    {
        auto it = wfrmList->get().begin();
        while (it->getData() != equivKey) it++;
        wfrmList->get().erase(it);
    }
    for (auto it = pred->get().rbegin(); it != pred->get().rend(); ++it) {
        wfrmList->append_front(wfrm, it->getData(), it->getKey(), it->getKeyRole(), it->getDataRole());
    }
    stack->erase(equivKey);
    for (auto &it: keys->get())
        data->erase(it.getData());
}

WSEML safeSum(const WSEML &Args) { /// Args = {O1:ref, O2:ref, res:ref}
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *O1, *O2, *res;
    O1 = extract(args->find(L"O1"));
    O2 = extract(args->find(L"O2"));
    res = extract(args->find(L"res"));

    std::wstring O1_str = dynamic_cast<ByteString *>(O1->getObj())->get();
    std::wstring O2_str = dynamic_cast<ByteString *>(O2->getObj())->get();
    if ((O1_str.find('/') != std::wstring::npos && O2_str.find('.') == std::wstring::npos) ||
        (O2_str.find('/') != std::wstring::npos && O1_str.find('.') == std::wstring::npos)) {
        mpq_class O1_t({O1_str.begin(), O1_str.end()}), O2_t({O2_str.begin(), O2_str.end()}), res_t;
        res_t = O1_t + O2_t;
        res_t.canonicalize();
        *res = WSEML(strconverter.from_bytes(res_t.get_str()));
        return *res;
    } else {
        mpf_class O1_t, O2_t, res_t;
        if (O1_str.find('/') != std::wstring::npos) {
            mpq_class q({O1_str.begin(), O1_str.end()});
            mpf_class num(q.get_num()), den(q.get_den());
            O1_t = num / den;
            O2_t = {O2_str.begin(), O2_str.end()};
        } else {
            if (O2_str.find('/') != std::wstring::npos) {
                mpq_class q({O2_str.begin(), O2_str.end()});
                mpf_class num(q.get_num()), den(q.get_den());
                O2_t = num / den;
                O1_t = {O1_str.begin(), O1_str.end()};
            } else {
                O1_t = {O1_str.begin(), O1_str.end()};
                O2_t = {O2_str.begin(), O2_str.end()};
            }
        }
        res_t = O1_t + O2_t;
        std::stringstream ss;
        ss.precision(32);
        ss << res_t;
        *res = WSEML(strconverter.from_bytes(ss.str()));
        return *res;
    }
}

WSEML safeSub(const WSEML &Args) {
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *O1, *O2, *res;
    O1 = extract(args->find(L"O1"));
    O2 = extract(args->find(L"O2"));
    res = extract(args->find(L"O2"));

    std::wstring O1_str = dynamic_cast<ByteString *>(O1->getObj())->get();
    std::wstring O2_str = dynamic_cast<ByteString *>(O2->getObj())->get();
    if ((O1_str.find('/') != std::wstring::npos && O2_str.find('.') == std::wstring::npos) ||
        (O2_str.find('/') != std::wstring::npos && O1_str.find('.') == std::wstring::npos)) {
        mpq_class O1_t({O1_str.begin(), O1_str.end()}), O2_t({O2_str.begin(), O2_str.end()}), res_t;
        res_t = O1_t - O2_t;
        res_t.canonicalize();
        *res = WSEML(strconverter.from_bytes(res_t.get_str()));
        return *res;
    } else {
        mpf_class O1_t, O2_t, res_t;
        if (O1_str.find('/') != std::wstring::npos) {
            mpq_class q({O1_str.begin(), O1_str.end()});
            mpf_class num(q.get_num()), den(q.get_den());
            O1_t = num / den;
            O2_t = {O2_str.begin(), O2_str.end()};
        } else {
            if (O2_str.find('/') != std::wstring::npos) {
                mpq_class q({O2_str.begin(), O2_str.end()});
                mpf_class num(q.get_num()), den(q.get_den());
                O2_t = num / den;
                O1_t = {O1_str.begin(), O1_str.end()};
            } else {
                O1_t = {O1_str.begin(), O1_str.end()};
                O2_t = {O2_str.begin(), O2_str.end()};
            }
        }
        res_t = O1_t - O2_t;
        std::stringstream ss;
        ss.precision(32);
        ss << res_t;
        *res = WSEML(strconverter.from_bytes(ss.str()));
        return *res;
    }
}

WSEML safeMult(const WSEML &Args) {
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *O1, *O2, *res;
    O1 = extract(args->find(L"O1"));
    O2 = extract(args->find(L"O2"));
    res = extract(args->find(L"O2"));

    std::wstring O1_str = dynamic_cast<ByteString *>(O1->getObj())->get();
    std::wstring O2_str = dynamic_cast<ByteString *>(O2->getObj())->get();
    if ((O1_str.find('/') != std::wstring::npos && O2_str.find('.') == std::wstring::npos) ||
        (O2_str.find('/') != std::wstring::npos && O1_str.find('.') == std::wstring::npos)) {
        mpq_class O1_t({O1_str.begin(), O1_str.end()}), O2_t({O2_str.begin(), O2_str.end()}), res_t;
        res_t = O1_t * O2_t;
        res_t.canonicalize();
        *res = WSEML(strconverter.from_bytes(res_t.get_str()));
        return *res;
    } else {
        mpf_class O1_t, O2_t, res_t;
        if (O1_str.find('/') != std::wstring::npos) {
            mpq_class q({O1_str.begin(), O1_str.end()});
            mpf_class num(q.get_num()), den(q.get_den());
            O1_t = num / den;
            O2_t = {O2_str.begin(), O2_str.end()};
        } else {
            if (O2_str.find('/') != std::wstring::npos) {
                mpq_class q({O2_str.begin(), O2_str.end()});
                mpf_class num(q.get_num()), den(q.get_den());
                O2_t = num / den;
                O1_t = {O1_str.begin(), O1_str.end()};
            } else {
                O1_t = {O1_str.begin(), O1_str.end()};
                O2_t = {O2_str.begin(), O2_str.end()};
            }
        }
        res_t = O1_t * O2_t;
        std::stringstream ss;
        ss.precision(32);
        ss << res_t;
        *res = WSEML(strconverter.from_bytes(ss.str()));
        return *res;
    }
}

WSEML safeDiv(const WSEML &Args) {
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *O1, *O2, *res;
    O1 = extract(args->find(L"O1"));
    O2 = extract(args->find(L"O2"));
    res = extract(args->find(L"O2"));

    std::wstring O1_str = dynamic_cast<ByteString *>(O1->getObj())->get();
    std::wstring O2_str = dynamic_cast<ByteString *>(O2->getObj())->get();
    if ((O1_str.find('/') != std::wstring::npos && O2_str.find('.') == std::wstring::npos) ||
        (O2_str.find('/') != std::wstring::npos && O1_str.find('.') == std::wstring::npos)) {
        mpq_class O1_t({O1_str.begin(), O1_str.end()}), O2_t({O2_str.begin(), O2_str.end()}), res_t;
        res_t = O1_t + O2_t;
        res_t.canonicalize();
        *res = WSEML(strconverter.from_bytes(res_t.get_str()));
        return *res;
    } else {
        mpf_class O1_t, O2_t, res_t;
        if (O1_str.find('/') != std::wstring::npos) {
            mpq_class q({O1_str.begin(), O1_str.end()});
            mpf_class num(q.get_num()), den(q.get_den());
            O1_t = num / den;
            O2_t = {O2_str.begin(), O2_str.end()};
        } else {
            if (O2_str.find('/') != std::wstring::npos) {
                mpq_class q({O2_str.begin(), O2_str.end()});
                mpf_class num(q.get_num()), den(q.get_den());
                O2_t = num / den;
                O1_t = {O1_str.begin(), O1_str.end()};
            } else {
                O1_t = {O1_str.begin(), O1_str.end()};
                O2_t = {O2_str.begin(), O2_str.end()};
            }
        }
        res_t = O1_t + O2_t;
        std::stringstream ss;
        ss.precision(32);
        ss << res_t;
        *res = WSEML(strconverter.from_bytes(ss.str()));
        return *res;
    }
}

WSEML safeMod(const WSEML &Args) {
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *O1, *O2, *res;
    O1 = extract(args->find(L"O1"));
    O2 = extract(args->find(L"O2"));
    res = extract(args->find(L"O2"));

    std::wstring O1_str = dynamic_cast<ByteString *>(O1->getObj())->get();
    std::wstring O2_str = dynamic_cast<ByteString *>(O2->getObj())->get();
    mpz_class O1_t({O1_str.begin(), O1_str.end()}), O2_t({O2_str.begin(), O2_str.end()}), res_t;
    res_t = O1_t % O2_t;
    *res = WSEML(strconverter.from_bytes(res_t.get_str()));
    return *res;
}

WSEML safePow(const WSEML &Args) {
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *O1, *O2, *res;
    O1 = extract(args->find(L"O1"));
    O2 = extract(args->find(L"O2"));
    res = extract(args->find(L"O2"));

    std::wstring O1_str = dynamic_cast<ByteString *>(O1->getObj())->get();
    std::wstring O2_str = dynamic_cast<ByteString *>(O2->getObj())->get();
    if (O1_str.find('(') != std::wstring::npos) O1_str = periodToFrac(O1_str);
    unsigned long int o2_ui = std::stoul(O2_str);
    if (O1_str.find('/') != std::wstring::npos) {
        mpq_class O1_t({O1_str.begin(), O1_str.end()}), res_t;
        mpz_class num(O1_t.get_num()), den(O1_t.get_den()), pnum, pden;
        mpz_pow_ui(pnum.get_mpz_t(), num.get_mpz_t(), o2_ui);
        mpz_pow_ui(pden.get_mpz_t(), den.get_mpz_t(), o2_ui);
        res_t.get_num() = pnum;
        res_t.get_den() = pden;
        res_t.canonicalize();
        return strconverter.from_bytes(res_t.get_str());
    } else {
        mpf_class O1_t({O1_str.begin(), O1_str.end()}), res_t;
        mpf_pow_ui(res_t.get_mpf_t(), O1_t.get_mpf_t(), o2_ui);
        std::stringstream ss;
        ss.precision(32);
        ss << res_t;
        return strconverter.from_bytes(ss.str());
    }
}

WSEML safeConcat(const WSEML &Args) {
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *O1, *O2, *res;
    O1 = extract(args->find(L"O1"));
    O2 = extract(args->find(L"O2"));
    res = extract(args->find(L"O2"));

    if (O1->typeInfo() == StringType) {
        std::wstring O1_str = dynamic_cast<ByteString *>(O1->getObj())->get();
        std::wstring O2_str = dynamic_cast<ByteString *>(O2->getObj())->get();
        *res = WSEML(O1_str + O2_str);
        return *res;
    } else {
        List *O1List = dynamic_cast<List *>(O1->getObj());
        List *O2List = dynamic_cast<List *>(O2->getObj());
        std::list<Pair> tmp;
        *res = WSEML(tmp);
        List *resList = dynamic_cast<List *>(res->getObj());
        auto O1It = O1List->get().begin();
        while (O1It != O1List->get().end())
            resList->append(res, O1It->getData(), O1It->getKey(), O1It->getKeyRole(), O1It->getDataRole());
        auto O2It = O2List->get().begin();
        while (O2It != O2List->get().end())
            resList->append(res, O2It->getData(), O2It->getKey(), O2It->getKeyRole(), O2It->getDataRole());
        return *res;
    }
}

WSEML safeEq(const WSEML &Args) {
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *O1, *O2, *res;
    O1 = extract(args->find(L"O1"));
    O2 = extract(args->find(L"O2"));
    res = extract(args->find(L"O2"));

    *res = (*O1 == *O2) ? WSEML(L"1") : WSEML(L"0");
    return *res;
}

WSEML safeNeq(const WSEML &Args) {
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *O1, *O2, *res;
    O1 = extract(args->find(L"O1"));
    O2 = extract(args->find(L"O2"));
    res = extract(args->find(L"O2"));

    *res = (*O1 != *O2) ? WSEML(L"1") : WSEML(L"0");
    return *res;
}

WSEML safeLess(const WSEML &Args) {
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *O1, *O2, *res;
    O1 = extract(args->find(L"O1"));
    O2 = extract(args->find(L"O2"));
    res = extract(args->find(L"O2"));

    *res = (compare(O1, O2, L"less")) ? WSEML(L"1") : WSEML(L"0");
    return *res;
}

WSEML safeGreater(const WSEML &Args) {
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *O1, *O2, *res;
    O1 = extract(args->find(L"O1"));
    O2 = extract(args->find(L"O2"));
    res = extract(args->find(L"O2"));

    *res = (compare(O1, O2, L"greater")) ? WSEML(L"1") : WSEML(L"0");
    return *res;
}

WSEML safeLeq(const WSEML &Args) {
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *O1, *O2, *res;
    O1 = extract(args->find(L"O1"));
    O2 = extract(args->find(L"O2"));
    res = extract(args->find(L"O2"));

    *res = ((*O1 == *O2) || compare(O1, O2, L"less")) ? WSEML(L"1") : WSEML(L"0");
    return *res;
}

WSEML safeGeq(const WSEML &Args) {
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *O1, *O2, *res;
    O1 = extract(args->find(L"O1"));
    O2 = extract(args->find(L"O2"));
    res = extract(args->find(L"O2"));

    *res = ((*O1 == *O2) || compare(O1, O2, L"greater")) ? WSEML(L"1") : WSEML(L"0");
    return *res;
}

WSEML safeAnd(const WSEML &Args) {
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *O1, *O2, *res;
    O1 = extract(args->find(L"O1"));
    O2 = extract(args->find(L"O2"));
    res = extract(args->find(L"O2"));

    bool O1_bool = ((*O1 == WSEML(L"1")) || (*O1 == WSEML(L"true")));
    bool O2_bool = ((*O2 == WSEML(L"1")) || (*O2 == WSEML(L"true")));
    *res = (O1_bool && O2_bool) ? WSEML(L"true") : WSEML(L"false");
    return *res;
}

WSEML safeOr(const WSEML &Args) {
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *O1, *O2, *res;
    O1 = extract(args->find(L"O1"));
    O2 = extract(args->find(L"O2"));
    res = extract(args->find(L"O2"));

    bool O1_bool = ((*O1 == WSEML(L"1")) || (*O1 == WSEML(L"true")));
    bool O2_bool = ((*O2 == WSEML(L"1")) || (*O2 == WSEML(L"true")));
    *res = (O1_bool || O2_bool) ? WSEML(L"true") : WSEML(L"false");
    return *res;
}

WSEML safeNot(const WSEML &Args) {
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *O, *res;
    O = extract(args->find(L"O1"));
    res = extract(args->find(L"res"));

    bool O1_bool = ((*O == WSEML(L"1")) || (*O == WSEML(L"true")));
    *res = (O1_bool) ? WSEML(L"false") : WSEML(L"true");
    return *res;
}

WSEML safeInsert(const WSEML &Args) {
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *R, *L, *RK, *K, *RD, *D, *I;
    R = extract(args->find(L"res"));
    L = extract(args->find(L"L"));
    RK = extract(args->find(L"RK"));
    K = extract(args->find(L"K"));
    RD = extract(args->find(L"RD"));
    D = extract(args->find(L"D"));
    I = extract(args->find(L"I"));

    if (L->typeInfo() == StringType) {
        std::wstring D_str = dynamic_cast<ByteString *>(D->getObj())->get();
        std::wstring L_str = dynamic_cast<ByteString *>(L->getObj())->get();
        size_t I_int = std::stoi(dynamic_cast<ByteString *>(L->getObj())->get());
        L_str.insert(I_int, D_str);
    } else {
        List *L_list = dynamic_cast<List *>(L->getObj());
        if (*I != NULLOBJ) {
            auto it = L_list->get().begin();
            size_t index = std::stoi(dynamic_cast<ByteString *>(I->getObj())->get());
            *R = L_list->insert(index, L, *D, *K, *RK, *RD);
        } else *R = L_list->append(L, *D, *K, *RK, *RD);
    }
    return *L;
}

WSEML safeErase(const WSEML &Args) {
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *O;
    O = extract(args->find(L"O"));
    Pair *O_pair = O->getPair();
    List *O_list = dynamic_cast<List *>(O->getList()->getObj());
    WSEML &O_key = O_pair->getKey();
    O_list->erase(O_key);
    return NULLOBJ;
}

WSEML safeIsDeref(const WSEML &Args) {
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *P, *res;
    P = extract(args->find(L"P"));
    res = extract(args->find(L"res"));

    *res = WSEML(L"true");
    List *ptr_list = dynamic_cast<List *>(P->getObj());
    auto listIt = ptr_list->get().begin();
    if (listIt->getKey() == WSEML(L"comp")) listIt++;
    while (listIt != ptr_list->get().end()) {
        List *ps = dynamic_cast<List *>(listIt->getData().getObj());
        WSEML &type = ps->find(L"t");
        std::wstring type_str = dynamic_cast<ByteString *>(type.getObj())->get();
        if (type_str == L"r" || type_str == L"i" || type_str == L"k" || type_str == L"u" || type_str == L"o")
            listIt++;
        else {
            *res = WSEML(L"false");
            break;
        }
    }
    return *res;
}

WSEML safeCall(const WSEML &Args) {
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *F, *A, *res;
    F = extract(args->find(L"F"));
    A = extract(args->find(L"A"));
    res = extract(args->find(L"res"));
    List *F_list = dynamic_cast<List *>(F->getObj());
    std::wstring dllName = dynamic_cast<ByteString *>(F_list->find(L"dllName").getObj())->get();
    std::wstring funcName = dynamic_cast<ByteString *>(F_list->find(L"dllName").getObj())->get();
    *res = callFunc(dllName.c_str(), funcName.c_str(), *A);
    return *res;
}

WSEML safeToI(const WSEML &Args) {
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *O, *obj, *res;
    O = extract(args->find(L"O"));
    obj = extract(args->find(L"obj"));
    res = extract(args->find(L"res"));

    List *O_list = dynamic_cast<List *>(O->getObj());
    List *lastPs = dynamic_cast<List *>(O_list->get().rbegin()->getData().getObj());
    WSEML lastPsKey = O_list->get().rbegin()->getKey();
    WSEML objKey = lastPs->get().rbegin()->getData();
    O_list->get().pop_back();
    List *upperList = dynamic_cast<List *>(obj->getList()->getObj()); // TODO: возможно проблемы, если при чтении не сохраняется укзатель
    auto uIt = upperList->get().begin();
    size_t index = 0;
    while (uIt->getKey() != objKey) {
        uIt++;
        index++;
    }
    WSEML newPs = parse(L"{t:i, i:" + std::to_wstring(index) + L"}");
    newPs.setType(WSEML(L"ps"));
    O_list->append(O, newPs, lastPsKey);
    return NULLOBJ;
}

WSEML safeToK(const WSEML &Args) {
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *O, *obj, *res;
    O = extract(args->find(L"O"));
    obj = extract(args->find(L"obj"));
    res = extract(args->find(L"res"));

    List *O_list = dynamic_cast<List *>(O->getObj());
    List *lastPs = dynamic_cast<List *>(O_list->get().rbegin()->getData().getObj());
    WSEML lastPsKey = O_list->get().rbegin()->getKey();
    WSEML objIndex = lastPs->get().rbegin()->getData();
    O_list->get().pop_back();
    List *upperList = dynamic_cast<List *>(obj->getList()->getObj()); // TODO: возможно проблемы, если при чтении не сохраняется укзатель
    auto uIt = upperList->get().begin();
    size_t index = std::stoi(dynamic_cast<ByteString *>(objIndex.getObj())->get());
    std::advance(uIt, index);
    std::wstring key = pack(uIt->getKey());
    WSEML newPs = parse(L"{t:k, k:" + key + L"}");
    newPs.setType(WSEML(L"ps"));
    O_list->append(O, newPs, lastPsKey);
    return NULLOBJ;
}

WSEML safeCallPrevDisp(const WSEML &Args) {
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *stck, *stack, *data, *res;
    stck = extract(args->find(L"stck"));
    stack = extract(args->find(L"stack"));
    data = extract(args->find(L"D"));
    res = extract(args->find(L"res"));

    WSEML newDisp = parse(
            L"{info:{wfrm:{1:1}, rot:true, pred:{}, next:{}, origin:pv, disp:{t:$, k:$}, child:{}, parent:$}, 1:$}");
    List *newDispList = dynamic_cast<List *>(newDisp.getObj());
    List *newInfo = dynamic_cast<List *>(newDispList->find(L"info").getObj());
    List *newNext = dynamic_cast<List *>(newInfo->find(L"next").getObj());
    newDispList->find(L"1") = *data;

    List *stckList = dynamic_cast<List *>(stck->getObj());
    List *stckInfo = dynamic_cast<List *>(stckList->find(L"info").getObj());
    WSEML &wlist = stckInfo->find(L"wlist");
    List *wlistList = dynamic_cast<List *>(wlist.getObj());
    List *curStackList = dynamic_cast<List *>(stack->getObj());
    List *curInfo = dynamic_cast<List *>(curStackList->find(L"info").getObj());
    WSEML &curPredObj = curInfo->find(L"pred");
    List *curPred = dynamic_cast<List *>(curPredObj.getObj());

    auto wlistIt = wlistList->get().begin();
    WSEML curWlistKey = wlistIt->getData();
    newNext->append(&newInfo->find(L"next"), curWlistKey, wlistIt->getKey(), wlistIt->getKeyRole(),
                    wlistIt->getDataRole());
    newInfo->find(L"pred") = curPredObj;

    wlistList->erase(curWlistKey);
    WSEML newDispKey = stckList->append(stack, newDisp);
    wlistList->insert(0, &wlist, newDispKey);

    wlistIt = wlistList->get().begin();
    auto predIt = curPred->get().begin();
    while (predIt != curPred->get().begin()) {
        WSEML predKey = predIt->getData();
        List *predStack = dynamic_cast<List *>(stckList->find(predKey).getObj());
        List *predInfo = dynamic_cast<List *>(predStack->find(L"info").getObj());
        List *predNext = dynamic_cast<List *>(predInfo->find(L"next").getObj());
        predNext->erase(curWlistKey);
        predNext->append(&predInfo->find(L"next"), wlistIt->getData(), wlistIt->getKey(), wlistIt->getKeyRole(),
                         wlistIt->getDataRole());
    }
    curPredObj = WSEML(L"{}");
    curPred->append(&curPredObj, wlistIt->getData(), wlistIt->getKey(), wlistIt->getKeyRole(), wlistIt->getDataRole());
    *res = newDispKey;
    return newDispKey;
}

WSEML safeCallPrevProg(const WSEML &Args) {
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *stack, *frm, *data, *res;
    stack = extract(args->find(L"stack"));
    frm = extract(args->find(L"frm"));
    data = extract(args->find(L"D"));
    res = extract(args->find(L"res"));

    List *newFrmList = dynamic_cast<List *>(data->getObj());
    List *newNext = dynamic_cast<List *>(newFrmList->find(L"next").getObj());

    List *stackList = dynamic_cast<List *>(stack->getObj());
    List *stackInfo = dynamic_cast<List *>(stackList->find(L"info").getObj());
    WSEML &wfrm = stackInfo->find(L"wfrm");

    List *wfrmList = dynamic_cast<List *>(wfrm.getObj());
    List *curFrmList = dynamic_cast<List *>(frm->getObj());
    WSEML &curPredObj = curFrmList->find(L"pred");
    List *curPred = dynamic_cast<List *>(curPredObj.getObj());

    auto wfrmIt = wfrmList->get().begin();
    WSEML curWfrmKey = wfrmIt->getData();
    newNext->append(&newFrmList->find(L"next"), curWfrmKey, wfrmIt->getKey(), wfrmIt->getKeyRole(),
                    wfrmIt->getDataRole());
    newFrmList->find(L"pred") = curPredObj;

    wfrmList->erase(curWfrmKey);
    WSEML newFrmKey = stackList->append(stack, *data);
    wfrmList->insert(0, &wfrm, newFrmKey);

    wfrmIt = wfrmList->get().begin();
    auto predIt = curPred->get().begin();
    while (predIt != curPred->get().begin()) {
        WSEML predKey = predIt->getData();
        List *predStack = dynamic_cast<List *>(stackList->find(predKey).getObj());
        List *predNext = dynamic_cast<List *>(newFrmList->find(L"next").getObj());
        predNext->erase(curWfrmKey);
        predNext->append(&newFrmList->find(L"next"), wfrmIt->getData(), wfrmIt->getKey(), wfrmIt->getKeyRole(),
                         wfrmIt->getDataRole());
    }
    curPredObj = WSEML(L"{}");
    curPred->append(&curPredObj, wfrmIt->getData(), wfrmIt->getKey(), wfrmIt->getKeyRole(), wfrmIt->getDataRole());
    *res = newFrmKey;
    return newFrmKey;
}

WSEML safeReadType(const WSEML &Args) {
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *O, *O_type;
    O = extract(args->find(L"O"));
    O_type = extract(args->find(L"res"));
    *O_type = O->getType();
    return *O_type;
}

WSEML safeSetType(const WSEML &Args) {
    List *args = dynamic_cast<List *>(Args.getObj());
    WSEML *O, *type;
    O = extract(args->find(L"O"));
    type = extract(args->find(L"T"));
    O->setType(*type);
    return NULLOBJ;
}