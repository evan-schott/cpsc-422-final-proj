#ifndef _USER_STDIO_H_
#define _USER_STDIO_H_

#include <stdarg.h>
#include <syscall.h>

#define MAX_BUF 512

#define getc()         sys_getc()
#define puts(str, len) sys_puts((str), (len))
#define readline(str, len) sys_readline((str), (len))

/*
 * standard c formatted output
 * supports the following flags:
 *     - . * l c s d u o p x %
 */
int printf(const char *fmt, ...);
int vcprintf(const char *fmt, va_list ap);

void printfmt(void (*f)(int, void *), void *buf, const char *fmt, ...);
void vprintfmt(void (*f)(int, void *), void *buf, const char *fmt,
               va_list ap);

#endif  /* _USER_STDIO_H_ */
