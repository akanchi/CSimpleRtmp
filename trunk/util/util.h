#ifndef TRUNK_UTIL_H
#define TRUNK_UTIL_H

#include <string>

extern std::string generate_random_string(int size);

extern int hmac_sha265(const void *key, int key_len, const unsigned char *data, size_t data_len, unsigned char *md);

#endif //TRUNK_UTIL_H
