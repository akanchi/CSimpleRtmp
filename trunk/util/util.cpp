#include "util.h"

#include <vector>
#ifdef USE_OPENSSL
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#endif

std::string generate_random_string(int size)
{
    std::string ret;
    srand((unsigned int)time(nullptr));
    for (int i = 0; i < size; i++) {
        ret.push_back((char)rand());
    }

    return ret;
}

int hmac_sha265(const void *key, int key_len, const unsigned char *data, size_t data_len, unsigned char *md)
{
#ifdef USE_OPENSSL
    std::vector<unsigned char> hash(SHA256_DIGEST_LENGTH, 0);
    HMAC_CTX *ctx = HMAC_CTX_new();
    HMAC_Init_ex(ctx, key, key_len, EVP_sha256(), NULL);
    HMAC_Update(ctx, data, data_len);
    unsigned  int mdSize = 0;
    HMAC_Final(ctx, (unsigned char*)&md[0], &mdSize);
    HMAC_CTX_free(ctx);
#endif

    return 0;
}