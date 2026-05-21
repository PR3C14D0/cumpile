#include "libuser.h"
#include <stdarg.h>

void sys_print(char* s) {
    asm volatile (
        "mov $1, %%eax\n"
        "int $0x80\n"
        :
        : "b"(s)
        : "eax"
    );
}

static void utoa(uint32_t val, char* buf) {
    char tmp[12];
    int i = 0;
    if (val == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }
    while (val > 0) {
        tmp[i++] = '0' + (val % 10);
        val /= 10;
    }
    int j = 0;
    while (i > 0) {
        buf[j++] = tmp[--i];
    }
    buf[j] = '\0';
}

static void itoa(int32_t val, char* buf) {
    if (val < 0) {
        *buf++ = '-';
        utoa((uint32_t)(-val), buf);
    } else {
        utoa((uint32_t)val, buf);
    }
}

static void xtoa(uint32_t val, char* buf) {
    char map[] = "0123456789abcdef";
    char tmp[10];
    int i = 0;
    if (val == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }
    while (val > 0) {
        tmp[i++] = map[val % 16];
        val /= 16;
    }
    int j = 0;
    while (i > 0) {
        buf[j++] = tmp[--i];
    }
    buf[j] = '\0';
}

int printf(const char* format, ...) {
    char buf[1024];
    int buf_idx = 0;
    
    va_list args;
    va_start(args, format);
    
    for (int i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%') {
            i++;
            if (format[i] == '\0') break;
            
            switch (format[i]) {
                case 's': {
                    char* s = va_arg(args, char*);
                    if (!s) s = "(null)";
                    for (int j = 0; s[j] != '\0'; j++) {
                        if (buf_idx < 1023) {
                            buf[buf_idx++] = s[j];
                        }
                    }
                    break;
                }
                case 'd': {
                    int val = va_arg(args, int);
                    char num_buf[32];
                    itoa(val, num_buf);
                    for (int j = 0; num_buf[j] != '\0'; j++) {
                        if (buf_idx < 1023) {
                            buf[buf_idx++] = num_buf[j];
                        }
                    }
                    break;
                }
                case 'u': {
                    unsigned int val = va_arg(args, unsigned int);
                    char num_buf[32];
                    utoa(val, num_buf);
                    for (int j = 0; num_buf[j] != '\0'; j++) {
                        if (buf_idx < 1023) {
                            buf[buf_idx++] = num_buf[j];
                        }
                    }
                    break;
                }
                case 'x': {
                    unsigned int val = va_arg(args, unsigned int);
                    char num_buf[32];
                    xtoa(val, num_buf);
                    for (int j = 0; num_buf[j] != '\0'; j++) {
                        if (buf_idx < 1023) {
                            buf[buf_idx++] = num_buf[j];
                        }
                    }
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    if (buf_idx < 1023) {
                        buf[buf_idx++] = c;
                    }
                    break;
                }
                case '%': {
                    if (buf_idx < 1023) {
                        buf[buf_idx++] = '%';
                    }
                    break;
                }
                default: {
                    if (buf_idx < 1023) {
                        buf[buf_idx++] = '%';
                    }
                    if (buf_idx < 1023) {
                        buf[buf_idx++] = format[i];
                    }
                    break;
                }
            }
        } else {
            if (buf_idx < 1023) {
                buf[buf_idx++] = format[i];
            }
        }
    }
    
    va_end(args);
    /* volatile prevents -O2 from eliminating the null terminator write */
    ((volatile char*)buf)[buf_idx] = '\0';
    sys_print(buf);
    return buf_idx;
}
