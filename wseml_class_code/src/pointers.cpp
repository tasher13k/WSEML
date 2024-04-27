#include "../include/pointers.h"
#include "../include/parser.h"
#include <sstream>


std::string getAddrStr(const WSEML* ptr){
    std::ostringstream s;
    s << (void const *)ptr;
    return s.str();
}

WSEML makePtr(const WSEML& object){
    std::string addr = getAddrStr(&object);
    WSEML ptr = parse("{addr:" + addr + "}");
    ptr.setType(WSEML("ptr"));
    return ptr;
}

WSEML calc(WSEML& expPtr) {
    std::list<Pair>& expList = dynamic_cast<List*>(expPtr.getObj())->get();
    if (expList.begin()->getKey() == WSEML("addr")){
        return expPtr;
    }
    auto listIt = expList.begin();
    WSEML* curObj = nullptr;
    if (equal(listIt->getData(), WSEML("r"))) {
        curObj = &expPtr;
        WSEML* upperListPtr = curObj->getList();
        while (upperListPtr != nullptr) {
            curObj = upperListPtr;
            upperListPtr = curObj->getList();
        }
    }
    if (equal(listIt->getData(), WSEML("s"))) {
        curObj = &expPtr;
        WSEML* upperListPtr = curObj->getList();
        while (upperListPtr->getPair()->getKey() != WSEML("stck")) {
            curObj = upperListPtr;
            upperListPtr = curObj->getList();
        }
    }
    else {
        curObj = extractObj(listIt->getData());
    }

    listIt++;
    bool byteFlag = false;
    int byteOffset = 0;

    while (listIt != expList.end()) {
        std::list<Pair>& curPS = dynamic_cast<List*>(listIt->getData().getObj())->get();
        auto psIt = curPS.begin();
        std::string step = dynamic_cast<ByteString*>(psIt->getData().getObj())->get();
        if (step == "i") {
            psIt++;
            int index = std::stoi(dynamic_cast<ByteString*>(psIt->getData().getObj())->get());
            if (curObj->typeInfo() == StringType) {
                if (index < 0) index = dynamic_cast<ByteString*>(curObj->getObj())->get().length() + index;
                byteFlag = true;
                byteOffset = index;
            }
            else{
                std::list<Pair>& curList = dynamic_cast<List*>(curObj->getObj())->get();
                auto itt = curList.begin();
                if (index < 0) index = curList.size() + index;
                std::advance(itt, index);
                curObj = &(itt->getData());
            }
        }
        if (step == "k") {
            psIt++;
            WSEML key = psIt->getData();
            std::list<Pair>& curList = dynamic_cast<List*>(curObj->getObj())->get();
            auto itt = curList.begin();
            while (!equal(itt->getKey(), key)) itt++;
            curObj = &(itt->getData());
        }
        if (step == "u") {
            curObj = (byteFlag) ? curObj : curObj->getList();
            byteFlag = false;
            byteOffset = 0;
        }
        if (step == "b") {
            psIt++;
            int offset =  std::stoi(dynamic_cast<ByteString*>(psIt->getData().getObj())->get());
            if (byteFlag)
                byteOffset += offset;
            else {
                std::list<Pair>& upperList = dynamic_cast<List*>(curObj->getList()->getObj())->get();
                auto itt = upperList.begin();
                while (!equal(itt->getData(), *curObj)) itt++;
                std::advance(itt, offset);
                curObj = &itt->getData();
            }
        }
        if (step == "r") {
            WSEML* upperListPtr = curObj->getList();
            while (upperListPtr != nullptr) {
                curObj = upperListPtr;
                upperListPtr = curObj->getList();
            }
        }
        listIt++;
    }

//    std::ostringstream s;
//    s << (void const *)curObj;
//    std::string address = s.str();
    std::list<Pair> newCompList;
    WSEML compPtr = WSEML();
    WSEML key = WSEML("addr");
    WSEML data = WSEML(getAddrStr(curObj));
    newCompList.emplace_back(&compPtr, key, data);
    if (byteFlag){
        WSEML sKey = WSEML("offset");
        WSEML sData = WSEML(std::to_string(byteOffset));
        newCompList.emplace_back(&compPtr, sKey, sData);
    }
    compPtr = std::move(WSEML(newCompList));
    return compPtr;
}

WSEML expand(WSEML& compPtr) {
    std::list<Pair>& compList = dynamic_cast<List*>(compPtr.getObj())->get();
    std::list<Pair> expList;
    WSEML expPtr = WSEML();
    auto listIt = compList.begin();
    unsigned long long int addr = std::stoi(dynamic_cast<ByteString*>(listIt->getData().getObj())->get(), 0, 16);
    WSEML* curObj = (WSEML*)addr;
    WSEML* upperListPtr = curObj->getList();
    int countPS = 0;
    listIt++;

    if (listIt !=  compList.end()) {
        WSEML curPsKey = WSEML(std::to_string(countPS++));
        WSEML type = WSEML("t");
        WSEML byInd = WSEML("i");
        WSEML ps = WSEML();
        std::list<Pair> psList;
        psList.emplace_back(&ps, type, byInd);
        psList.emplace_back(&ps, byInd, listIt->getData());
        ps = std::move(WSEML(psList));
        expList.emplace(expList.begin(), &expPtr, curPsKey, ps);
    }

    while (upperListPtr != nullptr) {
        std::list<Pair>& upperList = dynamic_cast<List*>(upperListPtr->getObj())->get();
        auto it = upperList.begin();
        while (!equal(*curObj, it->getData())) it++;
        WSEML curPsKey = WSEML(std::to_string(countPS++));
        WSEML type = WSEML("t");
        WSEML byKey = WSEML("k");
        WSEML ps = WSEML();
        std::list<Pair> psList;
        psList.emplace_back(&ps, type, byKey);
        psList.emplace_back(&ps, byKey, it->getKey());
        ps = std::move(WSEML(psList));
        expList.emplace(expList.begin(), &expPtr, curPsKey, ps);
        curObj = upperListPtr;
        upperListPtr = curObj->getList();
    }

//    std::ostringstream s;
//    s << (void const *)curObj;
    WSEML curPsKey = WSEML(std::to_string(countPS++));
    WSEML type = WSEML("addr");
//    WSEML address = WSEML(s.str());
    WSEML address = WSEML(getAddrStr(curObj));
    WSEML ps = WSEML();
    std::list<Pair> psList;
    psList.emplace_back(&ps, type, address);
    ps = std::move(WSEML(psList));
    expList.emplace(expList.begin(), &expPtr, curPsKey, ps);
    expPtr = std::move(WSEML(expList));
    return expPtr;
}

WSEML reduce(WSEML& expPtr) {
    std::list<Pair>& expList = dynamic_cast<List*>(expPtr.getObj())->get();
    auto listIt = expList.begin();
    WSEML* curObj = nullptr;
    if (equal(listIt->getData(), WSEML("r"))) {
        curObj = &expPtr;
        WSEML* upperListPtr = curObj->getList();
        while (upperListPtr != nullptr) {
            curObj = upperListPtr;
            upperListPtr = curObj->getList();
        }
    }
    else
        curObj = extractObj(listIt->getData());
    WSEML reduced = WSEML();
    std::list<Pair> reducedList;
    reducedList.emplace_back(&reduced, listIt->getKey(), listIt->getData());
    std::list<std::string> psStack = {"0"};
    std::list<WSEML*> objStack = {curObj};
    listIt++;

    while (listIt != expList.end()) {
        std::list<Pair>& curPS = dynamic_cast<List*>(listIt->getData().getObj())->get();
        auto psIt = curPS.begin();
        std::string step = dynamic_cast<ByteString*>(psIt->getData().getObj())->get();
        curObj = objStack.back();
        if (step == "i") {
            psIt++;
            int index = std::stoi(dynamic_cast<ByteString*>(psIt->getData().getObj())->get());

            if (psStack.back() == "i_str"){
                curPS.back().getData() = std::move(WSEML(std::to_string(index)));
                curPS.back().getData().getObj()->setPair(&curPS.back());
                reducedList.back().getData() = std::move(curPS);
                reducedList.back().getData().getObj()->setPair(&reducedList.back());
            }
            else if (psStack.back() == "u" && curObj->typeInfo() == ListType){
                std::list<Pair>& upperList = dynamic_cast<List*>(curObj->getObj())->get();
                auto itOnPrev = upperList.begin();
                objStack.pop_back();
                WSEML* prevObj = objStack.back();
                int prevInd = 0;
                while (!equal(itOnPrev->getData(), *prevObj)){
                    itOnPrev++;
                    prevInd++;
                }
                if (index < 0) index = upperList.size() + index;
                int offset = index - prevInd;
                auto itOnCur = upperList.begin();
                std::advance(itOnCur, index);
                WSEML newPs = WSEML();
                std::list<Pair> newPsList;
                WSEML type = WSEML("t");
                WSEML toB = WSEML("b");
                WSEML off = WSEML(std::to_string(offset));
                newPsList.emplace_back(&newPs, type, toB);
                newPsList.emplace_back(&newPs, toB, off);
                newPs = std::move(WSEML(newPsList));
                newPs.getObj()->setPair(&reducedList.back());
                reducedList.back().getData() = newPs;
                objStack.push_back(&itOnCur->getData());
            }
            else {
                if (curObj->typeInfo() == ListType) {
                    std::list<Pair> &upperList = dynamic_cast<List *>(curObj->getObj())->get();
                    auto itOnPrev = upperList.begin();
                    if (index < 0) index = upperList.size() + index;
                    std::advance(itOnPrev, index);
                    objStack.push_back(&itOnPrev->getData());
                    psStack.push_back(step);
                    reducedList.push_back(*listIt);
                }
                else {
                    objStack.push_back(curObj);
                    psStack.push_back("i_str");
                    reducedList.push_back(*listIt);
                }
            }

        }
        if (step == "k") {
            std::list<Pair>& upperList = dynamic_cast<List*>(curObj->getObj())->get();
            auto itOnNewKey = upperList.begin();
            WSEML key = curPS.back().getData();
            while (!equal(itOnNewKey->getKey(), key)) itOnNewKey++;

            if (psStack.back() == "u"){
                objStack.pop_back();
                WSEML* prevKey = objStack.back();
                auto itOnPrevKey = upperList.begin();
                while (!equal(itOnPrevKey->getData(), *prevKey)) itOnPrevKey++;
                int offset = std::distance(itOnPrevKey, itOnNewKey);
                if (offset == 0) {
                    psStack.pop_back();
                    reducedList.pop_back();
                }
                else {
                    WSEML newPs = WSEML();
                    std::list<Pair> newPsList;
                    WSEML type = WSEML("t");
                    WSEML toB = WSEML("b");
                    WSEML off = WSEML(std::to_string(offset));
                    newPsList.emplace_back(&newPs, type, toB);
                    newPsList.emplace_back(&newPs, toB, off);
                    newPs = std::move(WSEML(newPsList));
                    newPs.getObj()->setPair(&(reducedList.back()));
                    reducedList.back().getData() = std::move(newPs);
                    objStack.push_back(&itOnNewKey->getData());
                }

            }
            else {
                psStack.push_back(step);
                reducedList.push_back(*listIt);
                objStack.push_back(&itOnNewKey->getData());
            }
        }
        if (step == "u") {
            if (psStack.back() == "i" || psStack.back() == "k" || psStack.back() == "i_str") {
                psStack.pop_back();
                objStack.pop_back();
                reducedList.pop_back();
            }
            else{
                if (psStack.back() == "b"){
                    psStack.pop_back();
                    reducedList.pop_back();
                    objStack.pop_back();
                }
                psStack.push_back(step);
                reducedList.emplace_back(&reduced, listIt->getKey(), listIt->getData());
                objStack.push_back(curObj->getList());
            }
        }
        if (step == "b") {
            psIt++;
            int offset =  std::stoi(dynamic_cast<ByteString*>(psIt->getData().getObj())->get());
            if (psStack.back() == "b"){
                std::list<Pair>& prevPs = dynamic_cast<List*>(reducedList.back().getData().getObj())->get();
                int prevOffset = std::stoi(dynamic_cast<ByteString*>(prevPs.back().getData().getObj())->get());
                int newOffset = prevOffset+offset;
                if (newOffset != 0){
                    WSEML newOff = WSEML(std::to_string(newOffset));
                    newOff.getObj()->setPair(&(prevPs.back()));
                    prevPs.back().getData() = std::move(newOff);
                    objStack.pop_back();

                    std::list<Pair>& upperList = dynamic_cast<List*>(curObj->getList()->getObj())->get();
                    auto itt = upperList.begin();
                    while (!equal(itt->getData(), *curObj)) itt++;
                    std::advance(itt, offset);
                    objStack.push_back(&itt->getData());
                }
                else{
                    reducedList.pop_back();
                    objStack.pop_back();
                    psStack.pop_back();
                }
            }
            if (psStack.back() == "i_str"){
                std::list<Pair>& prevPs = dynamic_cast<List*>(reducedList.back().getData().getObj())->get();
                int prevOffset = std::stoi(dynamic_cast<ByteString*>(prevPs.back().getData().getObj())->get());
                int newOffset = prevOffset+offset;
                WSEML newOff = WSEML(std::to_string(newOffset));
                newOff.getObj()->setPair(&(prevPs.back()));
                prevPs.back().getData() = std::move(newOff);
            }
            if (psStack.back() == "i"){
                std::list<Pair>& prevPs = dynamic_cast<List*>(reducedList.back().getData().getObj())->get();
                int prevIndex = std::stoi(dynamic_cast<ByteString*>(prevPs.back().getData().getObj())->get());
                int newIndex = prevIndex+offset;
                WSEML newOff = WSEML(std::to_string(newIndex));
                newOff.getObj()->setPair(&(prevPs.back()));
                prevPs.back().getData() = std::move(newOff);

                std::list<Pair>& upperList = dynamic_cast<List*>(curObj->getList()->getObj())->get();
                auto itt = upperList.begin();
                if (newIndex < 0) newIndex = upperList.size() + newIndex;
                std::advance(itt,newIndex);
                objStack.pop_back();
                objStack.push_back(&(itt->getData()));
            }
            if (psStack.back() == "k"){
                std::list<Pair>& prevPs = dynamic_cast<List*>(reducedList.back().getData().getObj())->get();
                WSEML prevKey = prevPs.back().getData();
                std::list<Pair>& upperList = dynamic_cast<List*>(curObj->getList()->getObj())->get();
                auto itt = upperList.begin();
                while (!equal(itt->getKey(), prevKey)) itt++;
                std::advance(itt, offset);
                WSEML newKey = itt->getKey();
                newKey.getObj()->setPair(&(prevPs.back()));
                prevPs.back().getData() = std::move(newKey);

                objStack.pop_back();
                objStack.push_back(&(itt->getData()));
            }
        }
        if (step == "r") {
            if (psStack.back() != "r") {
                WSEML *upperListPtr = curObj->getList();
                while (upperListPtr != nullptr) {
                    curObj = upperListPtr;
                    upperListPtr = (curObj->getObj()->getPair() != nullptr) ? curObj->getList() : nullptr;
                }
                psStack.push_back("r");
                objStack.push_back(curObj);
                reducedList.emplace_back(&reduced, listIt->getKey(), listIt->getData());
            }
        }
        listIt++;
    }
    reduced = std::move(WSEML(reducedList));
    return reduced;
}

void to_i(WSEML& expPtr) {
    std::list<Pair>& expList = dynamic_cast<List*>(expPtr.getObj())->get();
    auto listIt = expList.begin();
    WSEML* curObj = nullptr;
    if (equal(listIt->getData(), WSEML("r"))) {
        curObj = &expPtr;
        WSEML* upperListPtr = curObj->getList();
        while (upperListPtr != nullptr) {
            curObj = upperListPtr;
            upperListPtr = curObj->getList();
        }
    }
    else {
        curObj = extractObj(listIt->getData());
    }
    listIt++;
    bool byteFlag = false;

    while (listIt != expList.end()) {
        std::list<Pair>& curPS = dynamic_cast<List*>(listIt->getData().getObj())->get();
        auto psIt = curPS.begin();
        std::string step = dynamic_cast<ByteString*>(psIt->getData().getObj())->get();
        if (step == "i") {
            psIt++;
            int index = std::stoi(dynamic_cast<ByteString*>(psIt->getData().getObj())->get());
            if (curObj->typeInfo() == ListType) {
                std::list<Pair>& curList = dynamic_cast<List*>(curObj->getObj())->get();
                if (index < 0) index = curList.size() + index;
                auto itt = curList.begin();
                std::advance(itt, index);
                curObj = &(itt->getData());
            }
            else byteFlag = true;
        }
        if (step == "k") {
            psIt++;
            WSEML key = psIt->getData();
            std::list<Pair>& curList = dynamic_cast<List*>(curObj->getObj())->get();
            auto itt = curList.begin();
            int pos = 0;
            while (!equal(itt->getKey(),key)) {
                pos++;
                itt++;
            }
            curObj = &(itt->getData());

            WSEML newPs = WSEML();
            std::list<Pair> newPsList;
            WSEML type = WSEML("t");
            WSEML byInd = WSEML("i");
            WSEML index = WSEML(std::to_string(pos));
            newPsList.emplace_back(&newPs, type, byInd);
            newPsList.emplace_back(&newPs, byInd, index);
            newPs = std::move(WSEML(newPsList));
            listIt->getData() = std::move(newPs);
            listIt->getData().getObj()->setPair(listIt->getKey().getObj()->getPair());
        }
        if (step == "u") {
            curObj = (byteFlag) ? curObj : curObj->getList();
            byteFlag = false;
        }
        if (step == "b") {
            psIt++;
            int offset =  std::stoi(dynamic_cast<ByteString*>(psIt->getData().getObj())->get());
            if (curObj->typeInfo() == ListType) {
                std::list<Pair>& upperList = dynamic_cast<List*>(curObj->getList()->getObj())->get();
                auto itt = upperList.begin();
                while (!equal(itt->getData(), *curObj)) itt++;
                std::advance(itt, offset);
                curObj = &itt->getData();
            }
        }
        if (step == "r") {
            WSEML* upperListPtr = curObj->getList();
            while (upperListPtr != nullptr) {
                curObj = upperListPtr;
                upperListPtr = curObj->getList();
            }
        }
        listIt++;
    }
}

void to_k(WSEML& expPtr) {
    std::list<Pair>& expList = dynamic_cast<List*>(expPtr.getObj())->get();
    auto listIt = expList.begin();
    WSEML* curObj = nullptr;
    if (equal(listIt->getData(), WSEML("r"))) {
        curObj = &expPtr;
        WSEML* upperListPtr = curObj->getList();
        while (upperListPtr != nullptr) {
            curObj = upperListPtr;
            upperListPtr = curObj->getList();
        }
    }
    else {
        curObj = extractObj(listIt->getData());
    }
    listIt++;
    bool byteFlag = false;

    while (listIt != expList.end()) {
        std::list<Pair>& curPS = dynamic_cast<List*>(listIt->getData().getObj())->get();
        auto psIt = curPS.begin();
        std::string step = dynamic_cast<ByteString*>(psIt->getData().getObj())->get();
        if (step == "i") {
            if (curObj->typeInfo() == ListType) {
                psIt++;
                int index = std::stoi(dynamic_cast<ByteString *>(psIt->getData().getObj())->get());
                std::list<Pair> &curList = dynamic_cast<List *>(curObj->getObj())->get();
                auto itt = curList.begin();
                if (index < 0) index = curList.size() + index;
                std::advance(itt, index);
                curObj = &(itt->getData());

                WSEML newPs = WSEML();
                std::list<Pair> newPsList;
                WSEML type = WSEML("t");
                WSEML byKey = WSEML("k");
                newPsList.emplace_back(&newPs, type, byKey);
                newPsList.emplace_back(&newPs, byKey, itt->getKey());
                newPs = std::move(WSEML(newPsList));
                listIt->getData() = std::move(newPs);
                listIt->getData().getObj()->setPair(listIt->getKey().getObj()->getPair());
            }
            else byteFlag = true;
        }
        if (step == "k") {
            psIt++;
            WSEML key = psIt->getData();
            std::list<Pair>& curList = dynamic_cast<List*>(curObj->getObj())->get();
            auto itt = curList.begin();
            while (!equal(itt->getKey(), key)) itt++;
            curObj = &(itt->getData());
        }
        if (step == "u") {
            curObj = (byteFlag) ? curObj : curObj->getList();
            byteFlag = false;
        }
        if (step == "b") {
            psIt++;
            int offset =  std::stoi(dynamic_cast<ByteString*>(psIt->getData().getObj())->get());
            if (curObj->typeInfo() == ListType) {
                std::list<Pair>& upperList = dynamic_cast<List*>(curObj->getList()->getObj())->get();
                auto itt = upperList.begin();
                while (!equal(itt->getData(), *curObj)) itt++;
                std::advance(itt, offset);
                curObj = &itt->getData();
            }
        }
        if (step == "r") {
            WSEML* upperListPtr = curObj->getList();
            while (upperListPtr != nullptr) {
                curObj = upperListPtr;
                upperListPtr = curObj->getList();
            }
        }
        listIt++;
    }
}

WSEML* extractObj(WSEML& ptr) {
    List* ptrList = dynamic_cast<List*>(ptr.getObj());
    WSEML compPtr = ptr;
    if (ptrList->find("addr") == NULLOBJ) compPtr = calc(ptr);
    std::list<Pair>& compList = dynamic_cast<List*>(compPtr.getObj())->get();
    unsigned long long int addr = std::stoi(dynamic_cast<ByteString*>(compList.begin()->getData().getObj())->get(), 0, 16);
    WSEML* obj = (WSEML*)addr;
    return obj;
}