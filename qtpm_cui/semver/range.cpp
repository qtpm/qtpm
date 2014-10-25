#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include "semver.h"


Range::Range() :
    _next(nullptr), _hasLower(false), _hasUpper(false),
    _includesLower(false), _includesUpper(false),
    _lower(nullptr), _upper(nullptr), _invalid(false)
{
}

Range::Range(const char *str) :
    _next(nullptr), _hasLower(false), _hasUpper(false),
    _includesLower(false), _includesUpper(false),
    _lower(nullptr), _upper(nullptr), _invalid(false)
{
    int offset = 0;

    Range *current = nullptr;
    Range *tail    = nullptr;

    int tidle, caret, star_ok;

    /**
     * 1: major
     * 2: minor
     * 3: patch
     */
    char         have[3] = {0, 0, 0};
    unsigned int bits[3] = {0, 0, 0};
    char         star[3] = {0, 0, 0};

    int len = strlen(str);
    int i, j = 0;

    while (offset < len) {
        tidle = 0;
        caret = 0;
        star_ok = 1;

        if (current == nullptr) {
            current = this;
        } else {
            current = new Range();
        }

        /* skip whitespace */
        if (str[offset] == ' ') {
            while (str[offset] == ' ') {
                offset++;
            }
        }

        if (str[offset] == '>') {
            offset++;

            star_ok = 0;

            current->_hasLower = true;
            current->_hasUpper = false;

            if (str[offset] == '=') {
              current->_includesLower = true;
              offset++;
            } else {
              current->_includesLower = false;
            }
        } else if (str[offset] == '<') {
            offset++;

            star_ok = 0;

            current->_hasLower = false;
            current->_hasUpper = true;

            current->_includesLower = 0;

            if (str[offset] == '=') {
                current->_includesUpper = true;

                offset++;
            } else {
                current->_includesUpper = false;
            }
        } else if (str[offset] == '~') {
            offset++;

            star_ok = 0;
            tidle = 1;

            current->_hasLower = true;
            current->_hasUpper = true;

            current->_includesLower = true;
            current->_includesUpper = false;
        } else if (str[offset] == '^') {
            offset++;

            star_ok = 0;
            caret = 1;

            current->_hasLower = true;
            current->_hasUpper = true;

            current->_includesLower = true;
            current->_includesUpper = false;
        } else {
            if (str[offset] == '=') {
              offset++;

              star_ok = 0;
            }

            current->_hasLower = true;
            current->_hasUpper = true;

            current->_includesLower = true;
            current->_includesUpper = true;
        }

        if (current->_hasLower) {
            current->_lower = new Spec();
        }

        if (current->_hasUpper) {
            current->_upper = new Spec();
        }

        /* skip whitespace */
        if (str[offset] == ' ') {
            while (str[offset] == ' ') {
                offset++;
            }
        }

        for (i=offset;i<=len;++i) {
            if (str[i] == '.' || str[i] == ' ' || i == len) {
                have[j] = 1;

                if (str[offset] == 'x' || str[offset] == '*') {
                    if (!star_ok) {
                        this->_invalid = true;
                        return;
                    }

                    star[j] = 1;
                    current->_includesUpper = 0;
                } else {
                    bits[j] = strtol(str + offset, NULL, 10);
                }

                j++;

                /* skip "." if that's why we're here */
                if (i < len && str[i] == '.') {
                    i++;
                }

                offset = i;

                /* we're done */
                if (j == 3) {
                    break;
                    continue;
                }
            }
        }

        if (current->_hasLower && have[2]) {
            current->_lower->_patch = bits[2];
        }
        if (current->_hasUpper && have[2]) {
            current->_upper->_patch = bits[2];
        }
        if (current->_hasLower && have[1]) {
            current->_lower->_minor = bits[1];
        }
        if (current->_hasUpper && have[1]) {
            current->_upper->_minor = bits[1];
        }
        if (current->_hasLower && have[0]) {
            current->_lower->_major = bits[0];
        }
        if (current->_hasUpper && have[0]) {
            current->_upper->_major = bits[0];
        }

        if (tidle) {
            if (have[2]) {
                current->_upper->_patch = 0;
                current->_upper->_minor++;
            } else if (have[1]) {
                current->_upper->_patch = 0;
                current->_upper->_minor++;
            } else {
                current->_upper->_major++;
            }
        } else if (caret) {
            if (current->_upper->_major > 0) {
                current->_upper->_patch = 0;
                current->_upper->_minor = 0;
                current->_upper->_major++;
            } else if (current->_upper->_minor > 0) {
                current->_upper->_patch = 0;
                current->_upper->_minor++;
            }
        } else if (star[0]) {
            current->_hasUpper = 0;
            current->_hasLower = 0;
        } else if (star[1]) {
            current->_upper->_patch = 0;
            current->_upper->_minor = 0;
            current->_upper->_major++;
        } else if (star[2]) {
            current->_upper->_patch = 0;
            current->_upper->_minor++;
        }

        if (tail != nullptr) {
            tail->_next = current;
        }

        tail = current;
    }
}

Range::~Range()
{
    if (this->_next) {
        delete this->_next;
        this->_next = nullptr;
    }
    if (this->_lower) {
        delete this->_lower;
        this->_lower = nullptr;
    }
    if (this->_upper) {
        delete this->_upper;
        this->_upper = nullptr;
    }
}

bool Range::operator <(const Spec &other) const
{
    return Range::compare(this, &other) < 0;
}

bool Range::operator ==(const Spec &other) const
{
    return Range::compare(this, &other) < 0;
}

bool Range::compare(const Spec *spec) const
{
    const Range* current = this;

    do {
        if (Range::compare(current, spec) != 0) {
            return false;
        }

        current = current->_next;
    } while (current != nullptr);

    return true;
}

int Range::compare(const Range *a, const Spec *b)
{
    if (a->_hasLower) {
        int bound = a->_includesLower ? 0 : -1;
        if (Spec::compare(a->_lower, b) > bound) {
            return -1;
        }
   }

    if (a->_hasUpper) {
        int bound = a->_includesUpper ? 0 : -1;
        if (Spec::compare(b, a->_upper) > bound) {
            return 1;
        }
    }
    return 0;
}


bool Range::invalid() const
{
    return _invalid;
}
