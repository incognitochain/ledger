#include "api.h"
#include "os.h"
#include "ux.h"
#include "utils.h"
#include "globals.h"
#include "crypto.h"
#include "menu.h"
#include "key.h"

static uint8_t set_result_get_alpha()
{
    uint8_t tx = 0;
    os_memmove(G_io_apdu_buffer + tx, processData, 32);
    tx += 32;

    os_memset(processData, 0, sizeof(processData));
    return tx;
}

void handleGetAlpha(uint8_t p1, uint8_t p2, uint8_t* dataBuffer, uint16_t dataLength, volatile unsigned int* flags, volatile unsigned int* tx)
{
    UNUSED(dataLength);
    UNUSED(p2);

    unsigned char alpha[32];

    os_memset(processData, 0, sizeof(processData));
    os_memmove(alpha, G_crypto_state_t.alpha + (p1 * 32), 32);
    os_memmove(processData, alpha, 32);
    sendResponse(set_result_get_alpha(), true);
    // if (trust_host == 0)
    // {
    //     ux_flow_init(0, ux_display_public_flow, NULL);
    //     *flags |= IO_ASYNCH_REPLY;
    // }
    if (true) // (trust_host == 1)
    {
        sendResponse(set_result_get_alpha(), true);
    }
    
}
