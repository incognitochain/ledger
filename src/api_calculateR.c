#include "api.h"
#include "os.h"
#include "ux.h"
#include "utils.h"
#include "crypto.h"
#include "globals.h"

static uint8_t set_result_calculate_r()
{
    uint8_t tx = 0;
    const uint8_t r_size = 32;
    os_memmove(G_io_apdu_buffer + tx, processData, r_size);
    tx += r_size;
    os_memset(processData, 0, sizeof(processData));
    return tx;
}

//calculate r
void handleCalculateR(uint8_t p1, uint8_t p2, uint8_t* dataBuffer, uint16_t dataLength, volatile unsigned int* flags, volatile unsigned int* tx)
{
    UNUSED(dataLength);
    UNUSED(p1);
    unsigned char c[32];
    unsigned char alpha[32];
    unsigned char coinPrivate[32];
    unsigned char ck[32];
    unsigned char result[32];
    os_memset(processData, 0, sizeof(processData));
    os_memmove(c, dataBuffer, 32);
    os_memmove(alpha, G_crypto_state_t.alpha + (p2 * 32), 32);
    os_memmove(coinPrivate, G_crypto_state_t.coinPrivateKey + (p2 * 32), 32);
    incognito_multm(ck, c, coinPrivate);
    incognito_subm(result, alpha, ck);
    os_memmove(processData, result, 32);
    sendResponse(set_result_calculate_r(), true);
}
