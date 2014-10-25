#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>

#include "semver.h"
#include "semver-private.h"


Spec::Spec()
    : _major(0), _minor(0), _patch(0),
      _release(nullptr), _build(nullptr), _invalid(false)
{
}

Spec::Spec(const char *str)
    : _major(0), _minor(0), _patch(0),
      _release(nullptr), _build(nullptr), _invalid(false)
{
    int len = strlen(str);
    int i = 0, o = 0;

    for (;i<=len;++i) {
        if (str[i] == ' ') {
            i++;
        } else {
            break;
        }
    }

    if (str[i] == 'v') {
        i++;
    }

    for (;i<=len;++i) {
        if (str[i] == ' ') {
            i++;
        } else {
            break;
        }
    }

    for (;i<=len;++i) {
        if (str[i] == '.') {
            this->_major = semver_private_strntol(str + o, i - o);
            i++;
            o = i;
            break;
        }

        if (str[i] < '0' || str[i] > '9') {
            this->_invalid = true;
            return;
        }
    }

    if (i == len) {
        this->_invalid = true;
        return;
    }

    for (;i<=len;++i) {
        if (str[i] == '.') {
            this->_minor = semver_private_strntol(str + o, i - o);
            i++;
            o = i;
            break;
        }

        if (str[i] < '0' || str[i] > '9') {
            this->_invalid = true;
            return;
        }
    }

    if (i == len) {
        this->_invalid = true;
        return;
    }

    for (;i<=len;++i) {
        if (str[i] == '-' || str[i] == '+' || i == len) {
            this->_patch = semver_private_strntol(str + o, i - o);
            o = i;
            break;
        }

        if (str[i] < '0' || str[i] > '9') {
            this->_invalid = true;
            return;
        }
    }

    if (o == len) {
        return;
    }

    if (str[o] == '-') {
        for (;i<=len;++i) {
            if (str[i] == '+' || i == len) {
                size_t releaseLen = i - o - 1;
                const char* releaseRaw = str + o + 1;
                o = i;
                this->_release = new Component(releaseRaw, releaseLen);
                break;
            }

            if (!(str[i] >= '0' && str[i] <= '9') && !(str[i] >= 'a' && str[i] <= 'z') && !(str[i] >= 'A' && str[i] <= 'Z') && str[i] != '-' && str[i] != '.') {
                this->_invalid = true;
                return;
            }
        }
    }

    if (str[o] == '+') {
        size_t buildLen = len - o - 1;
        const char* buildRaw = str + o + 1;
        o = len;
        this->_build = new Component(buildRaw, buildLen);
    }

}

Spec::~Spec()
{
    if (this->_release) {
        delete this->_release;
        this->_release = nullptr;
    }

    if (this->_build) {
        delete this->_build;
        this->_build = nullptr;
    }
}

std::string Spec::toString() const
{
    std::stringstream out;
    out << this->_major << "." << this->_minor << "." << this->_patch;
    if (this->_release) {
        out << "-";
        this->_release->toString(out);
    }
    if (this->_build) {
        out << "+";
        this->_build->toString(out);
    }
    return out.str();
}

int Spec::major() const
{
    return _major;
}

int Spec::minor() const
{
    return _minor;
}

int Spec::patch() const
{
    return _patch;
}

Component *Spec::release() const
{
    return _release;
}

Component *Spec::build() const
{
    return _build;
}


bool Spec::invalid() const
{
    return _invalid;
}

bool Spec::operator < (const Spec &other) const
{
    return Spec::compare(this, &other) < 0;
}

bool Spec::operator == (const Spec &other) const
{
    return Spec::compare(this, &other) == 0;
}

int Spec::compare(const Spec *a, const Spec *b)
{
    if (a->_major != b->_major) {
        return a->_major > b->_major ? 1 : -1;
    }

    if (a->_minor != b->_minor) {
        return a->_minor > b->_minor ? 1 : -1;
    }

    if (a->_patch != b->_patch) {
        return a->_patch > b->_patch ? 1 : -1;
    }

    /* no release > new release > old release */
    if (a->_release == nullptr && b->_release != nullptr) {
        return 1;
    }

    if (a->_release != nullptr && b->_release == nullptr) {
        return -1;
    }

    if (a->_release != nullptr || b->_release != nullptr) {
        return Component::compare(a->_release, b->_release);
    }

    /* new build > old build > no build */
    if (a->_build != nullptr && b->_build == nullptr) {
        return 1;
    }

    if (a->_build == nullptr && b->_build != nullptr) {
        return -1;
    }

    if (a->_build != nullptr || b->_build != nullptr) {
        return Component::compare(a->_build, b->_build);
    }

    return 0;
}

