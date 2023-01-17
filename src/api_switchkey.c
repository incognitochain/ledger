#include "api.h"
#include "os.h"
#include "ux.h"
#include "utils.h"
#include "globals.h"
#include "crypto.h"
#include "string.h"

static uint32_t accNum;

void getRawAddress() 
{
    //PaymentAddress.Pk
    unsigned char buffer[32];
    incognito_gen_public_spend_key(buffer);
    memmove(processData + 4, buffer, 32);
    //PaymentAddress.TK
    memset(buffer, 0, 32);
    incognito_gen_public_view_key(buffer);
    memmove(processData + 36, buffer, 32);
    //PaymentAddress.OTAPublic
    memset(buffer, 0, 32); 
    incognito_gen_public_ota_key(buffer);
    memmove(processData + 68, buffer, 32);
    memset(buffer, 0, 32);
}

static uint8_t set_result_switch_account()
{
    uint8_t tx = 0;

    incognito_reset_crypto_state();
    incognito_gen_private_key(accNum);
    getRawAddress();
    memmove(G_io_apdu_buffer + tx, processData, 100);
    tx = 100;
    memset(processData, 0, sizeof(processData));
    return tx;
}

//////////////////////////////////////////////////////////////////////
UX_STEP_NOCB(
    ux_display_switch_flow_1_step,
    pnn,
    {
        &C_icon_warning,
        "Confirm switch",
        "account?",
    });
UX_STEP_NOCB(
    ux_display_switch_flow_2_step,
    bnnn_paging,
    {
        .title = "Account number",
        .text = (const char*)processData,
    });
UX_STEP_VALID(
    ux_display_switch_flow_3_step,
    pb,
    sendResponse(set_result_switch_account(), true),
    {
        &C_icon_validate_14,
        "Approve",
    });
UX_STEP_VALID(
    ux_display_switch_flow_4_step,
    pb,
    sendResponse(0, false),
    {
        &C_icon_crossmark,
        "Reject",
    });

UX_FLOW(ux_display_switch_flow,
    &ux_display_switch_flow_1_step,
    &ux_display_switch_flow_2_step,
    &ux_display_switch_flow_3_step,
    &ux_display_switch_flow_4_step,
    FLOW_LOOP);



void handleSwitchAccount(uint8_t p1, uint8_t p2, uint8_t* dataBuffer, uint16_t dataLength, volatile unsigned int* flags, volatile unsigned int* tx)
{
    UNUSED(dataLength);
    UNUSED(p1);
    UNUSED(p2);
    UNUSED(tx);
    UNUSED(flags);

    explicit_bzero(processData, sizeof(processData));
    memmove(processData, dataBuffer, 4);
    accNum = (processData[0] << 24) | (processData[1] << 16) | (processData[2] << 8) | (processData[3]);
    sendResponse(set_result_switch_account(), true);
    
};
