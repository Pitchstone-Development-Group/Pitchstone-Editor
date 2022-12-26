#ifndef PITCHSTONE_MATH_RATIONAL
#define PITCHSTONE_MATH_RATIONAL

#include <stdint.h>
#include <math.h>
#include <iostream>

extern "C" {
#include <libavcodec/avcodec.h>
}

static int64_t gcd(int64_t a, int64_t b) {
    while (b) {
        a %= b;
        a ^= b; b ^= a; a ^= b;
    }
    return a;
}

static int sgn(int64_t a, int64_t b) {
    return ((a * b) > 0) ? a : -a;
}

struct rational {
    int64_t n, d;
	rational(int64_t _n = 1, int64_t _d = 1) { n = _n; d = _d; }
	rational(const rational& q) { n = q.n, d = q.d; }
	rational(const AVRational& q) { n = q.num, d = q.den; }

    rational operator =(const int64_t i) { n = i; d = 1; return *this; }
    rational operator =(const rational& q) { n = q.n; d = q.d; return *this; }
    rational operator =(const AVRational& q) {n = q.num; d = q.den; return *this; }
    rational operator -() { n = -n; return *this; }
    rational operator ~() { if (n == 0) d = 1; else if (d == 0) n = 1; else { int64_t g = gcd(abs(n), abs(d)); n = sgn(n, d) / g; d = abs(d) / g; } return *this; }
    float  toFloat() { return (float)n / d; }
    double toDouble() { return (double)n / d; }

    bool operator ==(const rational& q) {return n * q.d == d * q.n; }
    bool operator ==(const int64_t   i) {return n == d * i; }
    bool operator <=(const rational& q) {return n * q.d <= d * q.n; }
    bool operator <=(const int64_t  i) {return n <= d * i; }
    bool operator >=(const rational& q) {return n * q.d >= d * q.n; }
    bool operator >=(const int64_t   i) {return n >= d * i; }
    bool operator  <(const rational& q) {return n * q.d < d * q.n; }
    bool operator  <(const int64_t   i) {return n < d * i; }
    bool operator  >(const rational& q) {return n * q.d > d * q.n; }
    bool operator  >(const int64_t   i) {return n > d * i; }

    rational operator  +(const rational& q) { return rational(n * q.d + d * q.n, d * q.d); }
    rational operator  +(const int64_t i) { return rational(n + i * d, d); }
    rational operator  -(const rational& q) { return rational(n * q.d - d * q.n, d * q.d); }
    rational operator  -(const int64_t i) { return rational(n - i * d, d); }
    rational operator  *(const rational& q) { return rational(n * q.n, d * q.d); }
    rational operator  *(const int64_t i) { return rational(n * i, d); }
    rational operator  /(const rational& q) { return rational(sgn(n, q.n) * q.d, d * abs(q.n)); }
    rational operator  /(const int64_t i) { return rational(n, i * d); }

    rational operator +=(const rational& q) { n = n * q.d + d * q.n; d = d * q.d; return *this; }
    rational operator +=(const int64_t i) { n += i * d; return *this; }
    rational operator -=(const rational& q) { n = n * q.d - d * q.n; d *= q.d; return *this; }
    rational operator -=(const int64_t i) { n -= i * d; return *this; }
    rational operator *=(const rational& q) { n *= q.n; d *= q.d; return *this; }
    rational operator *=(const int64_t i) { n *= i; return *this; }
    rational operator /=(const rational& q) { n = sgn(n, q.n) * q.d; d *= abs(q.n); return *this; }
    rational operator /=(const int64_t i) { d *= i; return *this; }

    friend std::ostream& operator<<(std::ostream& os, const rational& q) { os << q.n; if (q.n != 0 && q.d != 1) os << '/' << q.d; return os; }
};

#endif
