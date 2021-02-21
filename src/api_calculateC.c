#include "api.h"
#include "os.h"
#include "ux.h"
#include "utils.h"
#include "crypto.h"
#include "globals.h"

static uint8_t set_result_calculate_c(uint8_t size)
{
    uint8_t tx = 0;
    // const uint8_t c_size = 64;
    os_memmove(G_io_apdu_buffer + tx, processData, size);
    tx += size;
    os_memset(processData, 0, sizeof(processData));
    return tx;
}

//calculate c
void handleCalculateC(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, volatile unsigned int *flags, volatile unsigned int *tx)
{
    // UNUSED(p1);

    unsigned char alphaG[32];
    unsigned char alpha[32];
    if (p1 == 0)
    {
        unsigned char alphaH[32];
        unsigned char H[32];
        os_memmove(alpha, G_crypto_state_t.alpha + (p2 * 32), 32);
        incognito_ecmul_G(alphaG, alpha);
        os_memmove(H, dataBuffer, 32);
        incognito_hash_to_point(H, H);
        incognito_ecmul_k(alphaH, H, alpha);

        os_memmove(processData, alphaG, 32);
        os_memmove(processData + 32, alphaH, 32);
        sendResponse(set_result_calculate_c(64), true);
    }
    else
    {
        unsigned char pedComG[32];
        os_memmove(alpha, G_crypto_state_t.alpha + (p2 * 32), 32);
        os_memmove(pedComG, dataBuffer, 32);
        incognito_ecmul_k(alphaG, pedComG, alpha);
        os_memmove(processData, alphaG, 32);
        sendResponse(set_result_calculate_c(32), true);
    }
}
