#include "../include/WSEML.h"
#include "../include/pointers.h"
#include "../include/parser.h"
#include "../include/dllconfig.h"

WSEML NULLOBJ = WSEML();
WSEML EMPTYLIST = parse("{}");

WSEML::WSEML(Object* obj): obj(obj){}

WSEML::WSEML(): obj(nullptr){}

WSEML::WSEML(std::string str, WSEML &type, Pair *p) {
    obj = new ByteString(str, type, p);
}

WSEML::WSEML(std::list<Pair> l, WSEML &type, Pair *p) {
    List* lst = new List(l, type, p);
    obj = lst;
    for (Pair& pair : lst->get())
        pair.setList(this);
}

WSEML::WSEML(const WSEML& wseml) {
    if (wseml.obj != nullptr) {
        obj = wseml.obj->clone();
        if (obj->typeInfo() == ListType) {
            std::list<Pair>& list = dynamic_cast<List*>(obj)->get();
            for (Pair &pair : list) {
                pair.setList(this);
            }
        }
    }
    else
        obj = nullptr;
}

WSEML::WSEML(WSEML&& wseml) noexcept {
    obj = wseml.obj;
    if (obj != nullptr && obj->typeInfo() == ListType) {
        std::list<Pair>& list = dynamic_cast<List *>(obj)->get();
        for (Pair &pair : list) {
            pair.setList(this);
        }
    }
    wseml.obj = nullptr;
}

WSEML::~WSEML() {
    delete obj;
}

WSEML& WSEML::operator=(const WSEML& wseml) {
    if (this != &wseml){
        delete obj;
        if (wseml.obj != nullptr) {
            obj = wseml.obj->clone();
            if (obj->typeInfo() == ListType) {
                std::list<Pair>& list = dynamic_cast<List*>(obj)->get();
                for (Pair& pair : list) {
                    pair.setList(this);
                }
            }
        }
        else
            obj = nullptr;
    }
    return *this;
}

WSEML& WSEML::operator=(WSEML&& wseml) noexcept {
    if (this != &wseml) {
        delete obj;
        obj = wseml.obj;
        if (obj != nullptr && obj->typeInfo() == ListType) {
            std::list<Pair>& list = dynamic_cast<List*>(obj)->get();
            for (Pair& pair : list) {
                pair.setList(this);
            }
        }
        wseml.obj = nullptr;
    }
    return *this;
}

bool equal(const WSEML& first, const WSEML& second) {
    if (first.getObj() == nullptr && second.getObj() == nullptr) return true;
    if (first.getObj() != nullptr && second.getObj() != nullptr)
        return (first.getObj()->equal(second.getObj()) && equal(first.getObj()->getType(), second.getObj()->getType()));
    else return false;
}

bool WSEML::operator==(const WSEML &wseml) {
    return equal(*this, wseml);
}

bool WSEML::operator!=(const WSEML &wseml) {
    return !equal(*this, wseml);
}

Object* WSEML::getObj() const {
    return obj;
}

void WSEML::setType(const WSEML &newType) {
    this->getObj()->setType(newType);
}

Types WSEML::typeInfo() const {
    return this->getObj()->typeInfo();
}

WSEML &WSEML::getType() {
    return this->getObj()->getType();
}

WSEML* WSEML::getList() {
    return (this->getObj()->getPair() != nullptr) ? this->getObj()->getPair()->getList() : nullptr;
}

Pair *WSEML::getPair() {
    return this->getObj()->getPair();
}

Object::Object(WSEML& type, Pair* pair): type(type), pair(pair){}

Object::~Object() = default;

void Object::setPair(Pair* p) {
    pair = p;
}

Pair* Object::getPair() {
    return pair;
}

WSEML& Object::getType() {
    return type;
}

void Object::setType(const WSEML& newType) {
    type = newType;
}

ByteString::ByteString(std::string str, WSEML& type, Pair* p): Object(type, p), bytes(std::move(str)){}

ByteString::~ByteString() = default;

ByteString* ByteString::clone() const {
    return new ByteString(*this);
}

std::string& ByteString::get() {
    return bytes;
}

Types ByteString::typeInfo() const {
    return StringType;
}

bool ByteString::equal(Object *obj) {
    return obj->equalTo(this);
}

bool ByteString::equalTo(ByteString *obj) {
    return (this->bytes == obj->bytes) && ::equal(this->getType(), obj->getType());
}

bool ByteString::equalTo(List *obj) {
    return false;
}

List::List(std::list<Pair> l, WSEML& type, Pair* p): Object(type, p), pairList(std::move(l)){}

List::~List() = default;

List* List::clone() const {
    return new List(*this);
}

std::list<Pair>& List::get() {
    return pairList;
}

Types List::typeInfo() const {
    return ListType;
}

bool List::equal(Object *obj) {
    return obj->equalTo(this);
}

bool List::equalTo(ByteString *obj) {
    return false;
}

bool List::equalTo(List *obj) {
    bool flag = true;
    auto firstIt = this->pairList.begin();
    auto secondIt = obj->pairList.begin();
    for (; firstIt != this->pairList.end() && secondIt != obj->pairList.end() && flag; firstIt++, secondIt++) {
        flag = (::equal(firstIt->getKey(), secondIt->getKey()) && ::equal(firstIt->getData(), secondIt->getData()) &&
                ::equal(firstIt->getKeyRole(), secondIt->getKeyRole()) && ::equal(firstIt->getDataRole(), secondIt->getDataRole()));
    }
    if (firstIt != this->pairList.end() || secondIt != obj->pairList.end()) flag = false;
    return flag;
}

WSEML List::genKey() {
    WSEML key = WSEML(std::to_string(this->nextKey));
    this->nextKey+=2;
    return key;
}

unsigned int &List::getCurMaxKey() {
    return this->nextKey;
}

WSEML &List::find(const WSEML &key) {
    auto it = pairList.begin();
    while (it->getKey() != key) it++;
    if (it != pairList.end())
        return it->getData();
    else return NULLOBJ;
}

WSEML &List::find(const std::string &key) {
    WSEML Key = WSEML(key);
    auto it = pairList.begin();
    while (it->getKey() != Key) it++;
    if (it != pairList.end())
        return it->getData();
    else return NULLOBJ;
}

void List::erase(const WSEML &key) {
    auto it = pairList.begin();
    while (it->getKey() != key) it++;
    if (it != pairList.end())
        this->pairList.erase(it);
}

void List::erase(const std::string &key) {
    WSEML Key = WSEML(key);
    auto it = pairList.begin();
    while (it->getKey() != Key) it++;
    if (it != pairList.end())
        this->pairList.erase(it);
}

WSEML &List::front() {
    return pairList.front().getData();
}

WSEML List::append(WSEML* listPtr, WSEML data, WSEML key, WSEML keyRole, WSEML dataRole) {
    if (key == NULLOBJ) key = genKey();
    pairList.emplace_back(listPtr, key, data, keyRole, dataRole);
    return key;
}

WSEML List::insert(size_t index, WSEML* listPtr, WSEML data, WSEML key, WSEML keyRole, WSEML dataRole) {
    if (key == NULLOBJ) key = genKey();
    auto it = pairList.begin();
    if (index < 0) index = pairList.size() + index;
    std::advance(it, index);
    pairList.emplace(it, listPtr, key, data, keyRole, dataRole);
    return key;
}

WSEML List::append_front(WSEML* listPtr, WSEML data, WSEML key, WSEML keyRole, WSEML dataRole) {
    if (key == NULLOBJ) key = genKey();
    pairList.emplace_front(listPtr, key, data, keyRole, dataRole);
    return key;
}

WSEML &List::back() {
    return pairList.back().getData();
}


Pair::Pair(WSEML* listPtr, WSEML key, WSEML data, WSEML keyRole, WSEML dataRole):
        key(std::move(key)), data(std::move(data)), keyRole(std::move(keyRole)), dataRole(std::move(dataRole)), listPtr(listPtr) {
    if (this->key.getObj())
        this->key.getObj()->setPair(this);
    if (this->data.getObj())
        this->data.getObj()->setPair(this);
    if (this->keyRole.getObj())
        this->keyRole.getObj()->setPair(this);
    if (this->dataRole.getObj())
        this->dataRole.getObj()->setPair(this);
}

WSEML& Pair::getKey() {
    return key;
}

WSEML& Pair::getData() {
    return data;
}

WSEML& Pair::getKeyRole() {
    return keyRole;
}

WSEML& Pair::getDataRole() {
    return dataRole;
}

WSEML* Pair::getList() {
    return listPtr;
}

void Pair::setList(WSEML *lst) {
    this->listPtr = lst;
}

Pair::Pair(const Pair& p): key(p.key), data(p.data), keyRole(p.keyRole), dataRole(p.dataRole) {
    if (this->key.getObj())
        this->key.getObj()->setPair(this);
    if (this->data.getObj())
        this->data.getObj()->setPair(this);
    if (this->keyRole.getObj())
        this->keyRole.getObj()->setPair(this);
    if (this->dataRole.getObj())
        this->dataRole.getObj()->setPair(this);
}

bool WSEML::one_step() {
    List* procList = dynamic_cast<List*>(this->obj);
    WSEML& stck = procList->find("stck");
    if (stck == NULLOBJ) {
        WSEML initStack = parse("{info:{wlist:{1:1}, rot:true}, 1:{info:{wfrm:{1:1}, rot:true, pred:{}, next:{}, origin:first, disp:{}, child:{}, parent:$},"
                                "1:$}}");
        List* stckList = dynamic_cast<List*>(initStack.getObj());
        List* stackList = dynamic_cast<List*>(stckList->find("1").getObj());
        stackList->find("1") = procList->find("prog");
        WSEML stckKey = WSEML("stck");
        procList->append(this, initStack, stckKey);
        return true;
    }
    else {
        List* stckList = dynamic_cast<List*>(stck.getObj());
        List* infoList = dynamic_cast<List*>(stckList->find("info").getObj());
        List* wlist = dynamic_cast<List*>(infoList->find("wlist").getObj());

        auto wlistIt = wlist->get().begin();
        WSEML& curStackId = wlistIt->getData();
        List* curStackList = dynamic_cast<List*>(stckList->find(curStackId).getObj());
        List* curStackInfo = dynamic_cast<List*>(curStackList->find("info").getObj());
        List* curStackNext = dynamic_cast<List*>(curStackInfo->find("next").getObj());
        List* wfrm = dynamic_cast<List*>(curStackList->find("wfrm").getObj());

        WSEML& curFrmKey = wfrm->front();
        WSEML& curFrm = curStackList->find(curFrmKey);
        WSEML curFrmType = curFrm.getType();

        List* tables = dynamic_cast<List*>(procList->find("tables").getObj());
        List* disp = dynamic_cast<List*>(tables->find("disp").getObj());
        WSEML& startFrm = disp->find(curFrmType);

        WSEML newDisp = parse("{info:{wfrm:{1:1}, rot:true, pred:{}, next:{}, origin:nd, disp:{}, child:{}, parent:$}, 1:$");
        List* newDispList = dynamic_cast<List*>(newDisp.getObj());
        List* newDispInfo = dynamic_cast<List*>(newDispList->find("info").getObj());
        List* newDispPred = dynamic_cast<List*>(newDispInfo->find("pred").getObj());
        newDispPred->append(&newDispInfo->find("pred"), wlistIt->getData(), wlistIt->getKey(), wlistIt->getKeyRole(), wlistIt->getDataRole());
        WSEML equivFrm = parse("{ip:$[1:$[t:r]ps, 2:$[t:k, k:data]ps, 3:$[t:k, k:one_step]ps, 4:$[t:k, k:START]ps]ptr, "
                                "pred:{}, next:{}, origin:nd}");
        equivFrm.setType(WSEML("frm"));
        newDispList->find("1") = equivFrm;
        wlist->get().pop_front();
        WSEML equivKey = stckList->append(&stck, newDisp);
        WSEML wlistEquivKey = wlist->append_front(&infoList->find("wlist"), equivKey);
        curStackNext->append(&curStackInfo->find("next"), wlistEquivKey, equivKey);

        if (startFrm == WSEML("func")) {
            List* frmList = dynamic_cast<List*>(startFrm.getObj());
            std::string dllName = dynamic_cast<ByteString*>(frmList->find("dllName").getObj())->get();
            std::string funcName = dynamic_cast<ByteString*>(frmList->find("funcName").getObj())->get();
            WSEML res = callFunc(dllName.c_str(), funcName.c_str(), NULLOBJ);
            if (res == WSEML("completed")) {
                auto predStackPair = newDispPred->get().begin();
                wlist->erase(wlistEquivKey);
                wlist->append_front(&infoList->find("wlist"), predStackPair->getData(), predStackPair->getKey(), predStackPair->getKeyRole(), predStackPair->getDataRole());
                curStackNext->erase(wlistEquivKey);
                stckList->erase(equivKey);
                return (infoList->find("wlist") != EMPTYLIST);
            }
            else return true;
        }
        else {
            equivFrm = parse("{ip:$[1:$[t:r]ps, 2:$[t:k, k:data]ps, 3:$[t:k, k:one_step]ps, 4:$[t:k, k:2_START]ps]ptr, "
                                   "pred:{}, next:{}, origin:nd}");
            equivFrm.setType(WSEML("frm"));
            List* newDispNext = dynamic_cast<List*>(newDispInfo->find("next").getObj());
            WSEML newDisp2 = parse("{info:{wfrm:{1:1}, rot:true, pred:{}, next:{}, origin:nd, disp:{}, child:{}, parent:$}, 1:$");
            List* newDisp2List = dynamic_cast<List*>(newDisp2.getObj());
            List* newDisp2Info = dynamic_cast<List*>(newDisp2List->find("info").getObj());
            List* newDisp2Pred = dynamic_cast<List*>(newDisp2Info->find("pred").getObj());
            newDisp2Pred->append(&newDisp2Info->find("pred"), wlistIt->getData(), wlistIt->getKey(), wlistIt->getKeyRole(), wlistIt->getDataRole());
            newDisp2List->find("1") = equivFrm;
            wlist->get().pop_front();
            equivKey = stckList->append(&stck, newDisp2);
            wlistEquivKey = wlist->append_front(&infoList->find("wlist"), equivKey);
            newDispNext->append(&newDispInfo->find("next"), wlistEquivKey, equivKey);

            WSEML dispKey = newDisp2List->append(&stckList->find(equivKey), startFrm);
            List* newDisp2Wfrm = dynamic_cast<List*>(newDisp2Info->find("wfrm").getObj());
            newDisp2Wfrm->append(&newDisp2Info->find("wfrm"), dispKey);
            return true;
        }
    }
    return true;
}