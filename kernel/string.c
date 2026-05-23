/* =============================================================================
 * MajesticOS - String Utilities
 * Standard C string functions for the kernel (no libc)
 * ============================================================================= */

#include "../include/string.h"
#include "../include/types.h"

size_t strlen(const char *str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

char *strcpy(char *dest, const char *src) {
    char *d = dest;
    while ((*d++ = *src++));
    return dest;
}

char *strncpy(char *dest, const char *src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i]; i++)
        dest[i] = src[i];
    for (; i < n; i++)
        dest[i] = '\0';
    return dest;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    while (n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) return 0;
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

char *strcat(char *dest, const char *src) {
    char *d = dest + strlen(dest);
    while ((*d++ = *src++));
    return dest;
}

char *strchr(const char *str, int c) {
    while (*str) {
        if (*str == (char)c) return (char *)str;
        str++;
    }
    return NULL;
}

void *memset(void *ptr, int value, size_t num) {
    unsigned char *p = (unsigned char *)ptr;
    while (num--) *p++ = (unsigned char)value;
    return ptr;
}

void *memcpy(void *dest, const void *src, size_t num) {
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;
    while (num--) *d++ = *s++;
    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const unsigned char *a = s1, *b = s2;
    while (n--) {
        if (*a != *b) return *a - *b;
        a++; b++;
    }
    return 0;
}

/* Convert integer to string */
void itoa(int value, char *str, int base) {
    char tmp[32];
    int  i = 0, neg = 0;

    if (value == 0) { str[0] = '0'; str[1] = '\0'; return; }

    if (value < 0 && base == 10) {
        neg = 1;
        value = -value;
    }

    while (value > 0) {
        int rem = value % base;
        tmp[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        value /= base;
    }

    if (neg) tmp[i++] = '-';

    /* Reverse */
    int j = 0;
    while (i > 0) str[j++] = tmp[--i];
    str[j] = '\0';
}

/* Convert unsigned integer to string */
void utoa(uint32_t value, char *str, int base) {
    char tmp[32];
    int  i = 0;

    if (value == 0) { str[0] = '0'; str[1] = '\0'; return; }

    while (value > 0) {
        int rem = value % base;
        tmp[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        value /= base;
    }

    int j = 0;
    while (i > 0) str[j++] = tmp[--i];
    str[j] = '\0';
}

/* Convert string to integer */
int atoi(const char *str) {
    int result = 0, sign = 1;
    if (*str == '-') { sign = -1; str++; }
    while (*str >= '0' && *str <= '9')
        result = result * 10 + (*str++ - '0');
    return sign * result;
}

/* Trim leading/trailing whitespace in-place */
char *strtrim(char *str) {
    /* Trim leading */
    while (*str == ' ' || *str == '\t') str++;
    if (!*str) return str;
    /* Trim trailing */
    char *end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n')) end--;
    *(end + 1) = '\0';
    return str;
}

/* Check if str starts with prefix */
int str_startswith(const char *str, const char *prefix) {
    return strncmp(str, prefix, strlen(prefix)) == 0;
}
