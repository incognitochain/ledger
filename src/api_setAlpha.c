#include "api.h"
#include "os.h"
#include "ux.h"
#include "utils.h"
#include "crypto.h"
#include "globals.h"

void handleSetAlpha(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, volatile unsigned int *flags, volatile unsigned int *tx)
{
    UNUSED(dataLength);
    UNUSED(p2);
    
    uint8_t txc = 0;
    os_memmove(G_crypto_state_t.alpha + (p1 * 32), dataBuffer, 32);
    
    G_io_apdu_buffer[txc++] = 0x90;
    G_io_apdu_buffer[txc++] = 0x00;
    sendResponse(txc, true);
}
