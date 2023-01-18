#include "api.h"
#include "os.h"
#include "ux.h"
#include "utils.h"
#include "crypto.h"
#include "globals.h"
#include "string.h"


static uint8_t set_result_calculate_r()
{
    uint8_t tx = 0;
    const uint8_t r_size = 32;
    memmove(G_io_apdu_buffer + tx, processData, r_size);
    tx += r_size;
    memset(processData, 0, sizeof(processData));
    return tx;
}

//////////////////////////////////////////////////////////////////////
UX_STEP_NOCB(
    ux_display_calculate_R_1_step,
    pnn,
    {
        &C_icon_warning,
        "Comfirm Sign",
        "Transaction?",
    });
UX_STEP_VALID(
    ux_display_calculate_R_2_step,
    pb,
    sendResponse(set_result_calculate_r(), true),
    {
        &C_icon_validate_14,
        "Approve",
    });
UX_STEP_VALID(
    ux_display_calculate_R_3_step,
    pb,
    sendResponse(0, false),
    {
        &C_icon_crossmark,
        "Reject",
    });

UX_FLOW(ux_display_calculate_R,
    &ux_display_calculate_R_1_step,
    &ux_display_calculate_R_2_step,
    &ux_display_calculate_R_3_step,
    FLOW_LOOP);

//calculate r
void handleCalculateR(uint8_t p1, uint8_t p2, uint8_t* dataBuffer, uint16_t dataLength, volatile unsigned int* flags, volatile unsigned int* tx)
{
    UNUSED(tx);
    UNUSED(dataLength);
    unsigned char c[32];
    unsigned char alpha[32];
    unsigned char coinPrivate[32];
    unsigned char ck[32];
    unsigned char result[32];
    memset(processData, 0, sizeof(processData));
    memmove(c, dataBuffer, 32);
    if (p1 == 0)
    {
        memmove(alpha, G_crypto_state_t.alpha + (p2 * 32), 32);
    }
    else 
    {
        memmove(alpha, G_crypto_state_t.alphaToken + (p2 * 32), 32);
    }
    memmove(coinPrivate, G_crypto_state_t.coinPrivateKey + (p2 * 32), 32);
    incognito_multm(ck, c, coinPrivate);
    incognito_subm(result, alpha, ck);
    memmove(processData, result, 32);

    if (trust_host == 0)
    {
        ux_flow_init(0, ux_display_calculate_R, NULL);
        *flags |= IO_ASYNCH_REPLY;
    }

    if (trust_host == 1)
    {
        if (p2 == 0 && p1 == 0) {
            ux_flow_init(0, ux_display_calculate_R, NULL);
            *flags |= IO_ASYNCH_REPLY;
        } 
        else {
            sendResponse(set_result_calculate_r(), true);
        }
    }
}
