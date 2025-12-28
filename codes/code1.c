// Luogu_P9484

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define LOCAL 1

#ifndef LOCAL
#define USER_FAST_IO
#endif

#define MAX_N 1000086
#define IO_BUF_SIZE (1 << 20)

static char _in_buf[IO_BUF_SIZE], *_in_p1 = _in_buf, *_in_p2 = _in_buf;
static char _out_buf[IO_BUF_SIZE], *_out_pp = _out_buf;

static inline char _IO_gc(void) {
    return _in_p1 == _in_p2 && (_in_p2 = (_in_p1 = _in_buf) + fread(_in_buf, 1, IO_BUF_SIZE, stdin), _in_p1 == _in_p2)
           ? EOF : *_in_p1++;
}

static inline void _IO_pc(char c) {
    if (_out_pp - _out_buf == IO_BUF_SIZE) {
        fwrite(_out_buf, 1, IO_BUF_SIZE, stdout);
        _out_pp = _out_buf;
    }
    *_out_pp++ = c;
}

int IO_read() {
#ifndef USER_FAST_IO
    int x;
    scanf("%d", &x);
    return x;
#else
    int x = 0, f = 1;
    char ch = _IO_gc();
    while (!isdigit(ch)) {
        if (ch == '-') f = -1;
        ch = _IO_gc();
    }
    while (isdigit(ch)) {
        x = (x << 1) + (x << 3) + (ch ^ 48);
        ch = _IO_gc();
    }
    return x * f;
#endif
}

void IO_write(int x) {
#ifndef USER_FAST_IO
    printf("%d\n", x);
#else
    if (x < 0) {
        _IO_pc('-');
        x = -x;
    }
    static char st[35];
    int top = 0;
    do {
        st[top++] = x % 10 + '0';
        x /= 10;
    } while (x);
    while (top) {
        _IO_pc(st[--top]);
    }
#endif

}

void IO_putchar(const char c) {
#ifndef USER_FAST_IO
    putchar(c);
#else
    _IO_pc(c);
#endif

}

void IO_puts(const char* s) {
#ifndef USER_FAST_IO
    puts(s);
#else
    while (*s) _IO_pc(*s++);
#endif
}

void IO_flush() {
    fwrite(_out_buf, 1, _out_pp - _out_buf, stdout);
    _out_pp = _out_buf;
}

int gcd(const int a, const int b) {
    return b ? gcd(b, a % b) : a;
}

int main() {
    int T = IO_read();
    while (T --) {
        const int nodes = IO_read();
        int q = IO_read();
        while (q --) {
            const int a = IO_read();
            const int b = IO_read();
            IO_write(a + b - 2 * gcd(a, b));
            IO_putchar('\n');
        }
    }
    IO_flush();
    return 0;
}
