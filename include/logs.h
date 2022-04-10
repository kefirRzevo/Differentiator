#ifndef LOGS_H
#define LOGS_H


#define CALLOC_CHECK(PTR)                                       \
    if(!PTR)                                                    \
    {                                                           \
        fprintf(stderr, "Calloc error: %s\n", strerror(errno)); \
        return 0;                                               \
    }

#define FILE_CHECK(FILE)                                        \
    if(!FILE)                                                   \
    {                                                           \
        fprintf(stderr, "File error: %s\n", strerror(errno));   \
        return 0;                                               \
    }

#endif