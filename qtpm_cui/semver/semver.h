#ifndef SEMVER_H
#define SEMVER_H

#include <stddef.h>
#include <string>
#include <sstream>

class Component {

public:
    Component();
    Component(const char* str, size_t len);
    ~Component();

    bool isNumeric() const;
    size_t dataLen() const;
    const char *dataRaw() const;
    int dataInt() const;
    Component *next() const;

    void toString(std::stringstream& out) const;

    bool operator < (const Component& other) const;
    bool operator == (const Component& other) const;

    static int compare(const Component* a, const Component* b);

private:
    Component *_next;
    bool _numeric;
    size_t _dataLen;
    const char *_dataRaw;
    int _dataInt;

    friend class Spec;
};

class Spec {

public:
    Spec();
    Spec(const char* str);
    ~Spec();

    std::string toString() const;

    bool operator < (const Spec& other) const;
    bool operator == (const Spec& other) const;

    static int compare(const Spec* a, const Spec* b);

    int major() const;
    int minor() const;
    int patch() const;
    Component *release() const;
    Component *build() const;

    bool invalid() const;

private:
    int _major;
    int _minor;
    int _patch;
    Component* _release;
    Component* _build;
    bool _invalid;

    friend class Range;
};

class Range {

public:
    Range();
    Range(const char* str);
    ~Range();

    bool operator < (const Spec& other) const;
    bool operator == (const Spec &other) const;
    bool compare(const Spec* spec) const;

    static int compare(const Range *a, const Spec* b);

    bool invalid() const;

private:
    Range* _next;
    bool _hasLower;
    bool _hasUpper;
    bool _includesLower;
    bool _includesUpper;
    Spec* _lower;
    Spec* _upper;
    bool _invalid;

};

#endif
