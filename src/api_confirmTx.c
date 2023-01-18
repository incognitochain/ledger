#include "api.h"
#include "ux.h"
#include "utils.h"
#include "globals.h"
#include "crypto.h"
#include "menu.h"
#include "key.h"

static uint8_t ADDRESS_LENGTH;
static char amount[9] ;
static uint8_t set_result_get_address()
{
    uint8_t tx = 0;
    // const uint8_t address_size = ADDRESS_LENGTH;
    // G_io_apdu_buffer[tx++] = address_size;
    memmove(G_io_apdu_buffer + tx, processData, ADDRESS_LENGTH);
    tx += ADDRESS_LENGTH;

    memset(processData, 0, sizeof(processData));
    return tx;
}

//////////////////////////////////////////////////////////////////////
UX_STEP_NOCB(
    ux_display_confirm_flow_1_step,
    pnn,
    {
        &C_icon_warning,
        "Confirm",
        "Transaction Receiver?",
    });
UX_STEP_NOCB(
    ux_display_confirm_flow_2_step,
    bnnn_paging,
    {
        .title = "Amount",
        .text = (const char*) amount,
    });
UX_STEP_NOCB(
    ux_display_confirm_flow_3_step,
    bnnn_paging,
    {
        .title = "PaymentAddress",
        .text = (const char*)processData,
    });
UX_STEP_VALID(
    ux_display_confirm_flow_4_step,
    pb,
    sendResponse(set_result_get_address(), true),
    {
        &C_icon_validate_14,
        "Approve",
    });
UX_STEP_VALID(
    ux_display_confirm_flow_5_step,
    pb,
    sendResponse(0, false),
    {
        &C_icon_crossmark,
        "Reject",
    });

UX_FLOW(ux_display_confirm_flow,
    &ux_display_confirm_flow_1_step,
    &ux_display_confirm_flow_2_step,
    &ux_display_confirm_flow_3_step,
    &ux_display_confirm_flow_4_step,
    &ux_display_confirm_flow_5_step,
    FLOW_LOOP);


void handleConfirmTx(uint8_t p1, uint8_t p2, uint8_t* dataBuffer, uint16_t dataLength, volatile unsigned int* flags, volatile unsigned int* tx)
{
    UNUSED(dataLength);
    UNUSED(p1);
    UNUSED(tx);

    //cache data
    memset(payment_info, 0, 104);
    memmove(payment_info, dataBuffer, 104);

    //parse ammount
    uint64_t amountRaw;
    incognito_decode_varint(dataBuffer, 8, &amountRaw);
    if (p2 == 6) 
    {
        PRINTF(amount, "%f", amountRaw / 1e6);
    } 
    else 
    {
         PRINTF(amount, "%f", amountRaw / 1e9);
    }
    //parse address
    memset(processData, 0, sizeof(processData));
    processData[0] = 1;
    processData[1] = 32;
    //PaymentAddress.Pk
    memmove(processData + 2, dataBuffer+8, 32);
    //PaymentAddress.TKTK
    processData[34] = 32;
    memmove(processData + 35, dataBuffer+40, 32);
    //PaymentAddress.OTAPublic
    processData[67] = 32;
    memmove(processData + 68, dataBuffer+72, 32);

    unsigned char buffer[32];
    memset(buffer, 0, 32);
    incognito_add_B58checksum(processData, 100, buffer);

    unsigned char base58check[109];
    base58check[0] = 0;
    memmove(base58check + 1, processData, 104);
    memset(processData, 0, sizeof(processData));
    incognito_add_B58checksum(base58check, 105, buffer);
    ADDRESS_LENGTH = encodeBase58(base58check, 109, (unsigned char*)processData, 255);


    ux_flow_init(0, ux_display_confirm_flow, NULL);
    *flags |= IO_ASYNCH_REPLY;
}
