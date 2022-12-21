#include "api.h"
#include "os.h"
#include "ux.h"
#include "utils.h"
#include "crypto.h"
#include "globals.h"

static uint8_t set_result_calculate_coin_private()
{
  uint8_t txc = 0;
  G_io_apdu_buffer[txc++] = 0x90;
  G_io_apdu_buffer[txc++] = 0x00;
  return txc;
}

//////////////////////////////////////////////////////////////////////
UX_STEP_NOCB(
    ux_display_coin_private_flow_1_step,
    pnn,
    {
        &C_icon_warning,
        "Calculate coin",
        "private key?",
    });
UX_STEP_VALID(
    ux_display_coin_private_flow_2_step,
    pb,
    sendResponse(set_result_calculate_coin_private(), true),
    {
        &C_icon_validate_14,
        "Approve",
    });
UX_STEP_VALID(
    ux_display_coin_private_flow_3_step,
    pb,
    sendResponse(0, false),
    {
        &C_icon_crossmark,
        "Reject",
    });

UX_FLOW(ux_display_coin_private_flow,
        &ux_display_coin_private_flow_1_step,
        &ux_display_coin_private_flow_2_step,
        &ux_display_coin_private_flow_3_step,
        FLOW_LOOP);


void handleGenCoinPrivate(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, volatile unsigned int *flags, volatile unsigned int *tx)
{
  UNUSED(dataLength);
  if (p1 == 1)
  {
    os_memmove(G_crypto_state_t.coinPrivateKey + (p2 * 32), dataBuffer, 32);
  }
  else
  {
    os_memset(processData, 0, sizeof(processData));
    unsigned char kota[32];
    os_memmove(kota, dataBuffer, 32);
    incognito_addm(kota, kota, G_crypto_state_t.key.key);
    os_memmove(G_crypto_state_t.coinPrivateKey + (p2 * 32), kota, 32);
  }

  if (trust_host == 0)
  {
    ux_flow_init(0, ux_display_coin_private_flow, NULL);
    *flags |= IO_ASYNCH_REPLY;
  }

  if (trust_host == 1)
  {
    sendResponse(set_result_calculate_coin_private(), true);
  }
}
