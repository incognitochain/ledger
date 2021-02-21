#include "api.h"
#include "os.h"
#include "ux.h"
#include "utils.h"
#include "crypto.h"

static uint8_t set_result_sign_schnorr(uint8_t sig_size)
{
    uint8_t tx = 0;
    // G_io_apdu_buffer[tx++] = ota_size;
    os_memmove(G_io_apdu_buffer + tx, processData, sig_size);
    tx += sig_size;
    os_memset(processData, 0, sizeof(processData));
    return tx;
}

void handleSignSchnorr(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, volatile unsigned int *flags, volatile unsigned int *tx)
{
    UNUSED(dataLength);
    UNUSED(p2);
    UNUSED(p1);
    os_memset(processData, 0, sizeof(processData));

    union api_signSchnorr
    {
        unsigned char pedPrivate[32]; //publicKey.g
        unsigned char pedRandom[32];  //publicKey.h
        unsigned char randomness[32]; //r
    } U;

    unsigned char message[64];
    unsigned char e[32];
    unsigned char s1[32];
    unsigned char z1[32];

    os_memmove(U.randomness, dataBuffer + 64, 32);

    if (cx_math_is_zero(U.randomness, 32) != 0)
    {
        cx_rng(U.randomness, 32);
        incognito_hash_to_scalar(s1, U.randomness, 32);

        os_memmove(U.pedPrivate, dataBuffer + 32, 32);
        incognito_ecmul_k(message, U.pedPrivate, s1);

        os_memmove(message + 32, dataBuffer + 96, 32);

        incognito_hash_to_scalar(e, message, 64);

        incognito_multm(U.pedPrivate, G_crypto_state_t.key.key, e);
        incognito_subm(z1, s1, U.pedPrivate);

        os_memmove(processData, e, 32);
        os_memmove(processData + 32, z1, 32);
        sendResponse(set_result_sign_schnorr(64), true);
        return;
    }
    else
    {
        // unsigned char s2[32];
        // unsigned char t[32];
        // unsigned char t1[32];
        // unsigned char z2[32];

        // cx_rng(U.randomness, 32);
        // incognito_hash_to_scalar(s1, U.randomness, 32);
        // cx_rng(U.randomness, 32);
        // incognito_hash_to_scalar(s2, U.randomness, 32);

        // os_memmove(U.pedPrivate, dataBuffer + 32, 32);
        // incognito_ecmul_k(t, U.pedPrivate, s1);

        // os_memmove(U.pedRandom, dataBuffer + 32, 32);
        // incognito_ecmul_k(t1, U.pedRandom, s2);

        // incognito_addm(t, t, t1);
        // os_memmove(message, t, 32);
        // os_memmove(message + 32, dataBuffer + 96, 32);

        // incognito_hash_to_scalar(e, message, 64);

        // incognito_multm(U.pedPrivate, G_crypto_state_t.key.key, e);
        // incognito_subm(z1, s1, U.pedPrivate);

        // os_memmove(U.randomness, dataBuffer + 64, 32);
        // incognito_multm(s1, U.randomness, e);
        // incognito_subm(z2, s2, s1);

        // os_memmove(processData, e, 32);
        // os_memmove(processData + 32, z1, 32);
        // os_memmove(processData + 64, z2, 32);

        // sendResponse(set_result_sign_schnorr(96), true);
        // return;
    }
}
