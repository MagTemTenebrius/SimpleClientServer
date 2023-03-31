//
// Created by tenebrius on 30.03.2023.
//

#include <cstdlib>
#include <ctime>
#include "diffie.h"

unsigned powmod(unsigned base, unsigned exp, unsigned modulo) {
    unsigned res = 1;
    while (exp != 0) {
        if ((exp & 1) != 0) {
            res = (1ll * res * base) % modulo;
        }
        base = (1ll * base * base) % modulo;
        exp >>= 1;
    }
    return res;
}

unsigned long generateNumber(int* a, unsigned int g, unsigned int p) {
//    srand(time(NULL));   // Initialization, should only be called once.
    *a = rand() % 10'000;      // Returns a pseudo-random integer between 0 and RAND_MAX.
    int A = powmod(g, *a, p);
    return A;
}

unsigned long generateASh(int a, unsigned long B, unsigned int p) {
    int Bh = powmod(B, a, p);
    return Bh;
}