#include "api.h"
#include "crypto.h"
#include "os.h"
#include "ux.h"
#include "utils.h"
#include "string.h"

// static char view[32];

static uint8_t set_result_decrypt_coin()
{
    uint8_t tx = 0;
    const uint8_t data_size = 64;
    // G_io_apdu_buffer[tx++] = data_size;
    memmove(G_io_apdu_buffer + tx, processData, data_size);
    tx += data_size;
    memset(processData, 0, sizeof(processData));
    return tx;
}

//////////////////////////////////////////////////////////////////////
UX_STEP_NOCB(
    ux_display_decrypt_flow_1_step,
    pnn,
    {
        &C_icon_warning,
        "Decrypt",
        "Coin Amount?",
    });
UX_STEP_NOCB(
  ux_display_decrypt_flow_2_step,
  bnnn_paging,
  {
    .title = "Decrypt",
    .text = (const char*)processData,
  });
UX_STEP_VALID(
    ux_display_decrypt_flow_3_step,
    pb,
    sendResponse(set_result_decrypt_coin(), true),
    {
        &C_icon_validate_14,
        "Approve",
    });
UX_STEP_VALID(
    ux_display_decrypt_flow_4_step,
    pb,
    sendResponse(0, false),
    {
        &C_icon_crossmark,
        "Reject",
    });

UX_FLOW(ux_display_decrypt_flow,
    &ux_display_decrypt_flow_1_step,
    // &ux_display_decrypt_flow_2_step,
    &ux_display_decrypt_flow_3_step,
    &ux_display_decrypt_flow_4_step,
    FLOW_LOOP);

void handleDecryptCoin(uint8_t p1, uint8_t p2, uint8_t* dataBuffer, uint16_t dataLength, volatile unsigned int* flags, volatile unsigned int* tx)
{
    UNUSED(dataLength);
    UNUSED(p2);
    UNUSED(p1);
    UNUSED(tx);

    unsigned char key[32];
    unsigned char txConcealRnd[32];
    unsigned char amount[32];
    unsigned char mask[32];
    incognito_gen_private_view_key(key);
    memmove(txConcealRnd, dataBuffer, 32);
    memmove(amount, dataBuffer + 32, 32);
    memmove(mask, dataBuffer + 64, 32);

    incognito_ecmul_k(key, txConcealRnd, key);
    incognito_hash_to_scalar(key, key, 32);
    incognito_hash_to_scalar(key, key, 32);
    incognito_subm(mask, mask, key);

    incognito_hash_to_scalar(key, key, 32);
    incognito_subm(amount, amount, key);

    memmove(processData, amount, 32);
    memmove(processData + 32, mask, 32);

    if (trust_host == 0)
    {
        ux_flow_init(0, ux_display_decrypt_flow, NULL);
        *flags |= IO_ASYNCH_REPLY;
    }
    if (trust_host == 1)
    {
        sendResponse(set_result_decrypt_coin(), true);
    }
}
