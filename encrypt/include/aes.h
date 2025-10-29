/* include/aes.h */
#ifndef AES_H
#define AES_H

#include <stdint.h>

typedef enum {
    AES_SUCCESS = 0,
    AES_ERROR   = 1,
} aes_code_t;

typedef uint8_t state_t[4][4];

void plaintext_to_state(const uint8_t *plaintext, state_t *state);
void sub_bytes(state_t *state);
void shift_rows(state_t *state);
void mix_columns(state_t *state);

aes_code_t encrypt(const uint8_t *plaintext);

#endif