#include "api.h"
#include "os.h"
#include "ux.h"
#include "utils.h"
#include "crypto.h"
#include "globals.h"

void handleGenAlpha(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, volatile unsigned int *flags, volatile unsigned int *tx)
{
    UNUSED(dataLength);
    uint8_t txc = 0;
   
    if (p2 == 0) 
    {
         for (size_t i = 0; i < p1; i++)
        {
            unsigned char rnd[32];
            cx_rng(rnd, 32);
            unsigned char alpha[32];
            incognito_hash_to_scalar(alpha, rnd, 32);
            os_memmove(G_crypto_state_t.alpha + (i * 32), alpha, 32);       
        }
    } 
    else 
    {
        for (size_t i = 0; i < p1; i++)
        {
            unsigned char rndToken[32];
            cx_rng(rndToken, 32);
            unsigned char alphaToken[32];
            incognito_hash_to_scalar(alphaToken, rndToken, 32);
            os_memmove(G_crypto_state_t.alphaToken + (i * 32), alphaToken, 32);
        }
    }

    G_io_apdu_buffer[txc++] = 0x90;
    G_io_apdu_buffer[txc++] = 0x00;
    sendResponse(txc, true);
}
