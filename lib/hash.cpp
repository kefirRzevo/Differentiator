#include "../include/hash.h"

hash_t qhash(const void *const data, const size_t size)
{
    const char       *byte     = (char *) data;
    const char *const lastByte = byte + size;
    hash_t            hash     = 0;
    
    while (byte < lastByte)
        hash = ((hash << 0x8) + (hash >> 0x8)) ^ *byte++;
    
    return hash;
}