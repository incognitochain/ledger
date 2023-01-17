#include "api.h"
#include "os.h"
#include "ux.h"
#include "utils.h"
#include "crypto.h"
#include "string.h"

static uint8_t set_result_sign_schnorr(uint8_t sig_size)
{
    uint8_t tx = 0;
    // G_io_apdu_buffer[tx++] = ota_size;
    memmove(G_io_apdu_buffer + tx, processData, sig_size);
    tx += sig_size;
    memset(processData, 0, sizeof(processData));
    return tx;
}

void handleSignSchnorr(uint8_t p1, uint8_t p2, uint8_t* dataBuffer, uint16_t dataLength, volatile unsigned int* flags, volatile unsigned int* tx)
{
    UNUSED(dataLength);
    UNUSED(p2);
    UNUSED(p1);
    UNUSED(tx);
    UNUSED(flags);
    memset(processData, 0, sizeof(processData));

    union ugroup1
    {
        unsigned char pedPrivate[32]; //publicKey.g
        unsigned char pedRandom[32];  //publicKey.h
        unsigned char randomness[32]; //r
        unsigned char randomnumber[32]; 
    } U;

    unsigned char message[64];
    unsigned char s1[32];
    unsigned char z1[32];

    memmove(U.randomness, dataBuffer + 64, 32);
    if (cx_math_is_zero(U.randomness, 32) != 0)
    {
        unsigned char e[32];
        cx_rng(U.randomnumber, 32);
        incognito_hash_to_scalar(s1, U.randomnumber, 32);

        memmove(U.pedPrivate, dataBuffer + 32, 32);
        incognito_ecmul_k(message, U.pedPrivate, s1);

        memmove(message + 32, dataBuffer + 96, 32);

        incognito_hash_to_scalar(e, message, 64);

        incognito_multm(U.pedPrivate, G_crypto_state_t.key.key, e);
        incognito_subm(z1, s1, U.pedPrivate);

        memmove(processData, e, 32);
        memmove(processData + 32, z1, 32);
        sendResponse(set_result_sign_schnorr(64), true);
        return;
    }
    else
    {
        unsigned char s2[32];
        union ugroup2
        {
            unsigned char t1[32];
            unsigned char z2[32];
        } U2;

        union ugroup3
        {
            unsigned char t[32];
            unsigned char e[32];
        } U3;

        cx_rng(U.randomnumber, 32);
        incognito_hash_to_scalar(s1, U.randomnumber, 32);
        cx_rng(U.randomnumber, 32);
        incognito_hash_to_scalar(s2, U.randomnumber, 32);

        memmove(U.pedPrivate, dataBuffer + 32, 32);
        incognito_ecmul_k(U3.t, U.pedPrivate, s1);

        memmove(U.pedRandom, dataBuffer, 32);
        incognito_ecmul_k(U2.t1, U.pedRandom, s2);

        incognito_ecadd(U3.t, U3.t, U2.t1);

        memmove(message, U3.t, 32);
        memmove(message + 32, dataBuffer + 96, 32);

        incognito_hash_to_scalar(U3.e, message, 64);

        incognito_multm(U.pedPrivate, G_crypto_state_t.key.key, U3.e);
        incognito_subm(z1, s1, U.pedPrivate);

        memmove(U.randomness, dataBuffer + 64, 32);
        incognito_multm(U2.z2, U.randomness, U3.e);
        incognito_subm(U2.z2, s2, U2.z2);

        memmove(processData, U3.e, 32);
        memmove(processData + 32, z1, 32);
        memmove(processData + 64, U2.z2, 32);

        sendResponse(set_result_sign_schnorr(96), true);
        return;
    }
}
