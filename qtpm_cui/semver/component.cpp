#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

#include "semver.h"
#include "semver-private.h"

Component::Component()
    : _next(nullptr), _numeric(false), _dataLen(0), _dataRaw(nullptr), _dataInt(0)
{

}

Component::Component(const char *str, size_t len)
    : _next(nullptr), _numeric(false), _dataLen(0), _dataRaw(nullptr), _dataInt(0)
{
    Component *tail = this;

    size_t offset = 0, i = 0, l = 0;
    while (offset < len) {
        if (tail != this) {
            tail->_next = new Component();
            tail = tail->_next;
        }

        for (i = offset; i < len; ++i) {
            if (str[i] == '.') {
                break;
            }
        }

        tail->_dataLen = i - offset;
        tail->_dataRaw = str + offset;

        if (str[i] == '.') {
            i++;
        }

        offset = i;

        l = tail->_dataLen;
        for (i = 0; i < l; i++) {
            if (tail->_dataRaw[i] < '0' || tail->_dataRaw[i] > '9') {
                tail->_numeric = false;
                 break;
            }
        }
        if (tail->_numeric) {
            tail->_dataInt = semver_private_strntol(tail->_dataRaw, tail->_dataLen);
        }
    }
}

Component::~Component()
{
    if (this->_next) {
        delete this->_next;
        this->_next = nullptr;
    }
}

bool Component::isNumeric() const
{
    return _numeric;
}

size_t Component::dataLen() const
{
    return _dataLen;
}

const char *Component::dataRaw() const
{
    return _dataRaw;
}

int Component::dataInt() const
{
    return _dataInt;
}

Component *Component::next() const
{
    return _next;
}

void Component::toString(std::stringstream &out) const
{
    if (this->_numeric == 1) {
        out << this->_numeric;
    } else {
        std::string str(this->_dataRaw, this->_dataLen);
        out << str;
    }

    if (this->_next) {
        out << ".";
        this->_next->toString(out);
    }
}

bool Component::operator <(const Component &other) const
{
    return Component::compare(this, &other) < 0;
}

bool Component::operator ==(const Component &other) const
{
    return Component::compare(this, &other) == 0;
}

int Component::compare(const Component *a, const Component *b)
{
    if (a == NULL && b != NULL) {
        return 1;
    } else if (a != NULL && b == NULL) {
        return -1;
    } else if (a == NULL && b == NULL) {
        return 0;
    }

    if (a->_numeric && b->_numeric) {
        if (a->_dataInt > b->_dataInt) {
            return 1;
        } else if (a->_dataInt < b->_dataInt) {
            return -1;
        }
    }

    int s = memcmp(a->_dataRaw, b->_dataRaw, a->_dataLen > b->_dataLen ? a->_dataLen : b->_dataLen);

    if (s != 0) {
        return s;
    }

    return Component::compare(a->_next, b->_next);
}
