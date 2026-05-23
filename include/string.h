#ifndef STRING_H
#define STRING_H

#include "types.h"

size_t  strlen(const char *str);
char   *strcpy(char *dest, const char *src);
char   *strncpy(char *dest, const char *src, size_t n);
int     strcmp(const char *s1, const char *s2);
int     strncmp(const char *s1, const char *s2, size_t n);
char   *strcat(char *dest, const char *src);
char   *strchr(const char *str, int c);
void   *memset(void *ptr, int value, size_t num);
void   *memcpy(void *dest, const void *src, size_t num);
int     memcmp(const void *s1, const void *s2, size_t n);

/* Integer to string conversion */
void    itoa(int value, char *str, int base);
void    utoa(uint32_t value, char *str, int base);
int     atoi(const char *str);

/* String helpers */
char   *strtrim(char *str);
int     str_startswith(const char *str, const char *prefix);

#endif /* STRING_H */
