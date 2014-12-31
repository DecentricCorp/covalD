#ifndef RIBBITCOIN_CRYPTO_SCRYPT_H
#define RIBBITCOIN_CRYPTO_SCRYPT_H
#include <stdlib.h>
#include <stdint.h>
static const int SCRYPT_SCRATCHPAD_SIZE = 131072 + 63;

void scrypt_1024_1_1_256(const char *input, char *output);
void scrypt_1024_1_1_256_sp_generic(const char *input, char *output, char *scratchpad);

#if defined(USE_SSE2)
extern void scrypt_detect_sse2(unsigned int cpuid_edx);
void scrypt_1024_1_1_256_sp_sse2(const char *input, char *output, char *scratchpad);
extern void (*scrypt_1024_1_1_256_sp)(const char *input, char *output, char *scratchpad);
#endif

void
PBKDF2_SHA256(const uint8_t *passwd, size_t passwdlen, const uint8_t *salt,
    size_t saltlen, uint64_t c, uint8_t *buf, size_t dkLen);

static inline uint32_t le32dec(const void *pp)
{
        const uint8_t *p = (uint8_t const *)pp;
        return ((uint32_t)(p[0]) + ((uint32_t)(p[1]) << 8) +
            ((uint32_t)(p[2]) << 16) + ((uint32_t)(p[3]) << 24));
}

static inline void le32enc(void *pp, uint32_t x)
{
        uint8_t *p = (uint8_t *)pp;
        p[0] = x & 0xff;
        p[1] = (x >> 8) & 0xff;
        p[2] = (x >> 16) & 0xff;
        p[3] = (x >> 24) & 0xff;
}

class CSCrypt
{
private:
    uint32_t s[8];
    unsigned char buf[64];
    char scratchpad[SCRYPT_SCRATCHPAD_SIZE];
    size_t bytes;

public:
    static const size_t OUTPUT_SIZE = 32;

    CSCrypt();
    CSCrypt& Write(const unsigned char* data, size_t len);
    void Finalize(unsigned char hash[OUTPUT_SIZE]);
    CSCrypt& Reset();
};


#endif // RIBBBITCOIN_CRYPTO_SCRYPT_H
