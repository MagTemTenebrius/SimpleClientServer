//
// Created by tenebrius on 30.03.2023.
//

#ifndef CLIENT_SERVER_DIFFIE_H
#define CLIENT_SERVER_DIFFIE_H

unsigned powmod(unsigned base, unsigned exp, unsigned modulo);
// Диффи-Хеллман
unsigned long generateNumber(int* a, unsigned int g, unsigned int p);

unsigned long generateASh(int a, unsigned long B, unsigned int p);


#endif //CLIENT_SERVER_DIFFIE_H
