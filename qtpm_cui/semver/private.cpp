#include "semver-private.h"

#include <stdio.h>

long semver_private_strntol(const char* str, size_t len) {
  long r = 0;

  while (str[0] >= '0' && str[0] <= '9' && len--) {
    r = r * 10 + ((str++)[0] - '0');
  }

  return r;
}
