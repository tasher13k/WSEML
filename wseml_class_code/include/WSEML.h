#ifndef WSEML_H
#define WSEML_H
#include <list>
#include <string>

class Pair;
class WSEML;
class Object;
class ByteString;
class List;

enum Types{ StringType, ListType };

extern WSEML NULLOBJ;

bool equal(const WSEML& first, const WSEML& second);

class WSEML{
public:
    WSEML();
    explicit WSEML(Object* obj);
    WSEML(std::wstring str, WSEML& type = NULLOBJ, Pair* p = nullptr);
    WSEML(std::list<Pair> l, WSEML& type = NULLOBJ, Pair* p = nullptr);
    WSEML(const WSEML& wseml);
    WSEML(WSEML&& wseml) noexcept;
    ~WSEML();
    WSEML& operator=(const WSEML& wseml);
    WSEML& operator=(WSEML&& wseml) noexcept;
    bool operator==(const WSEML& wseml) const;
    bool operator!=(const WSEML& wseml) const;
    void setType(const WSEML& newType) const;
    Types typeInfo() const;
    WSEML& getType() const;
    WSEML* getList() const;
    Pair* getPair() const;
    Object* getObj() const;
    bool one_step();
private:
    Object* obj = nullptr;
};

class Object{
public:
    Object(WSEML& type, Pair* pair);
    virtual Object* clone() const = 0;
    virtual ~Object();
    void setPair(Pair* p);
    void setType(const WSEML& newType);
    virtual Types typeInfo() const = 0;
    Pair* getPair();
    WSEML& getType();
    virtual bool equal(Object* obj) = 0;
    virtual bool equalTo(ByteString* obj) = 0;
    virtual bool equalTo(List* obj) = 0;
private:
    WSEML type;
    Pair* pair = nullptr;
};

class ByteString: public Object{
public:
    ByteString(std::wstring str, WSEML& type = NULLOBJ, Pair* p = nullptr);
    ~ByteString() override;
    ByteString* clone() const override;
    Types typeInfo() const override;
    std::wstring& get();
    bool equal(Object* obj) override;
    bool equalTo(ByteString* obj) override;
    bool equalTo(List* obj) override;
private:
    std::wstring bytes;
};

class List: public Object{
public:
    List(std::list<Pair> l, WSEML& type= NULLOBJ, Pair* p = nullptr);
    ~List() override;
    List* clone() const override;
    Types typeInfo() const override;
    std::list<Pair>& get();
    WSEML genKey();
    unsigned int& getCurMaxKey();
    bool equal(Object* obj) override;
    bool equalTo(ByteString* obj) override;
    bool equalTo(List* obj) override;
    WSEML& find(const WSEML& key);
    WSEML& find(const std::wstring& key);
    void erase(const WSEML& key);
    void erase(const std::wstring& key);
    WSEML append(WSEML* listPtr, const WSEML& data, WSEML key = NULLOBJ, const WSEML& keyRole = NULLOBJ, const WSEML& dataRole = NULLOBJ);
    WSEML insert(size_t index, WSEML* listPtr, const WSEML& data, WSEML key = NULLOBJ, const WSEML& keyRole = NULLOBJ, const WSEML& dataRole = NULLOBJ);
    WSEML append_front(WSEML* listPtr, const WSEML& data, WSEML key = NULLOBJ, const WSEML& keyRole = NULLOBJ, const WSEML& dataRole = NULLOBJ);
    WSEML& front();
    WSEML& back();
private:
    std::list<Pair> pairList;
    unsigned int nextKey = 1;
};

class Pair{
public:
    Pair(WSEML* listPtr, WSEML key, WSEML data, WSEML keyRole = NULLOBJ, WSEML dataRole = NULLOBJ);
    Pair(const Pair& p);
    ~Pair() = default;
    WSEML& getKey();
    WSEML& getData();
    WSEML& getKeyRole();
    WSEML& getDataRole();
    WSEML* getList();
    void setList(WSEML* lst);
private:
    WSEML key;
    WSEML data;
    WSEML keyRole;
    WSEML dataRole;
    WSEML* listPtr = nullptr;
};

#endif