#include "api.h"
#include "os.h"
#include "ux.h"
#include "utils.h"
#include "crypto.h"
#include "globals.h"
#include "string.h"

static uint8_t set_result_calculate_c(uint8_t size)
{
    uint8_t tx = 0;
    // const uint8_t c_size = 64;
    memmove(G_io_apdu_buffer + tx, processData, size);
    tx += size;
    memset(processData, 0, sizeof(processData));
    return tx;
}

//calculate c
void handleCalculateC(uint8_t p1, uint8_t p2, uint8_t* dataBuffer, uint16_t dataLength, volatile unsigned int* flags, volatile unsigned int* tx)
{
    UNUSED(flags);
    UNUSED(tx);
    UNUSED(dataLength);
    unsigned char alphaG[32];
    unsigned char alpha[32];
    if (p1 == 0)
    {
        unsigned char alphaH[32];
        unsigned char H[32];
        memmove(alpha, G_crypto_state_t.alpha + (p2 * 32), 32);
        incognito_ecmul_G(alphaG, alpha);
        memmove(H, dataBuffer, 32);
        incognito_hash_to_point(H, H);
        incognito_ecmul_k(alphaH, H, alpha);

        memmove(processData, alphaG, 32);
        memmove(processData + 32, alphaH, 32);
        sendResponse(set_result_calculate_c(64), true);
    }
    else
    {
        unsigned char pedComG[32];
        memmove(alpha, G_crypto_state_t.alpha + (p2 * 32), 32);
        memmove(pedComG, dataBuffer, 32);
        incognito_ecmul_k(alphaG, pedComG, alpha);
        memmove(processData, alphaG, 32);
        sendResponse(set_result_calculate_c(32), true);
    }
}

//calculate c-ca
void handleCalculateCCA(uint8_t p1, uint8_t p2, uint8_t* dataBuffer, uint16_t dataLength, volatile unsigned int* flags, volatile unsigned int* tx)
{
    UNUSED(flags);
    UNUSED(tx);
    UNUSED(dataLength);
    unsigned char alphaG[32];
    unsigned char alphaToken[32];
    if (p1 == 0)
    {
        unsigned char alphaH[32];
        unsigned char H[32];
        memmove(alphaToken, G_crypto_state_t.alphaToken + (p2 * 32), 32);
        incognito_ecmul_G(alphaG, alphaToken);
        memmove(H, dataBuffer, 32);
        incognito_hash_to_point(H, H);
        incognito_ecmul_k(alphaH, H, alphaToken);

        memmove(processData, alphaG, 32);
        memmove(processData + 32, alphaH, 32);
        sendResponse(set_result_calculate_c(64), true);
    }
    else
    {
        unsigned char pedComG[32];
        memmove(alphaToken, G_crypto_state_t.alphaToken + (p2 * 32), 32);
        memmove(pedComG, dataBuffer, 32);
        incognito_ecmul_k(alphaG, pedComG, alphaToken);
        memmove(processData, alphaG, 32);
        sendResponse(set_result_calculate_c(32), true);
    }
}