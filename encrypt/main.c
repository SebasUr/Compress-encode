/*
    Implementation of the AES-128 cipher algorithm
    By: Sebastián Andrés Uribe Ruiz & Daniel Santana Meza
*/
#include "include/aes.h"

int main() {
    uint8_t plaintext[16] = {
        0x32, 0x88, 0x31, 0xe0,
        0x43, 0x5a, 0x35, 0x37,
        0xf6, 0x30, 0x98, 0x07,
        0xa8, 0x8d, 0xa2, 0x34
    };

    encrypt(plaintext);

    return 0;
}