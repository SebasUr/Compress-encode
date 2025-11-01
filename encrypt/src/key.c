#include <string.h>
#include "key.h"

extern const uint8_t sbox[256];
extern void shift_row_n(uint8_t *row, uint8_t n);

static uint8_t round_constants[11] = {0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36};

void sub_word(uint8_t *word) {
    for (uint8_t i = 0; i < 4; i++) {
        word[i] = sbox[word[i]];
    }
}

void key_expansion(const uint8_t *key, word* words) {
    int i = 0;
    
    // First 4 bytes of the key are 4 words
    while (i <= Nk -1) {
        memcpy(words[i], key + 4*i, 4);
        i++;
    } 

    while (i <= 4*Nr+3) {
        word temp;
        memcpy(temp, words[i-1], 4);
        if (i % Nk == 0) {
            shift_row_n(temp, 1);
            sub_word(temp);
            temp[0] ^= round_constants[i/Nk];
        }
        words[i][0] = words[i - Nk][0] ^ temp[0];
        words[i][1] = words[i - Nk][1] ^ temp[1];
        words[i][2] = words[i - Nk][2] ^ temp[2];
        words[i][3] = words[i - Nk][3] ^ temp[3];
        i++;
    }
}