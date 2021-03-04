#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "os.h"
#include "ux.h"
#include "os_io_seproxyhal.h"
#include "key.h"

#define PIN_VERIFIED BOLOS_UX_OK
#define P1_CONFIRM 0x01
#define P1_NON_CONFIRM 0x00
#define P1_FIRST 0x00
#define P1_MORE 0x80

#define FULL_ADDRESS_LENGTH 54
#define BIP32_PATH 5

#define SW_WRONG_DATA_RANGE 0x6985
extern ux_state_t ux;
// display stepped screens
extern unsigned int ux_step;
extern unsigned int ux_step_count;
extern unsigned char processData[300];
extern unsigned char trust_host;

typedef struct internalStorage_t
{
    unsigned char setting_1;
    unsigned char setting_2;
    unsigned char setting_3;
    unsigned char setting_4;

    uint8_t initialized;
} internalStorage_t;

extern const internalStorage_t N_storage_real;
#define N_storage (*(volatile internalStorage_t *)PIC(&N_storage_real))

struct crypto_state_s
{
    privatekey_t key;

    union keccakunion
    {
    cx_sha3_t keccakF;
    cx_sha3_t keccakH;
    } KU;

    // for ring sig
    unsigned char coinPrivateKey[256];
    unsigned char alpha[288];

};

typedef struct crypto_state_s crypto_state_t;
extern crypto_state_t G_crypto_state_t;

#define INCOGNITO_IO_BUFFER_LENGTH (300)
#endif
