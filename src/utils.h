#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

inline size_t mbToBytes(size_t size_in_mb) { return size_in_mb * 1048576; }

template <typename T>
inline T charsToUnsignedNum(const char* buf) {
    T ret = 0;
    for (size_t i = 0; i < sizeof(T); i++) {
        ret |= (T)(unsigned char)buf[i]
               << (8 * i);  // TODO: confirm the convertion is correct
    }
    return ret;
}

template <typename T>
inline void unsignedNumToChars(char* buf, const T number) {
    // note that this is little endian
    for (size_t i = 0; i < sizeof(T); i++) {
        buf[i] = (unsigned char)(number >> (8 * i));
    }
}

#endif
