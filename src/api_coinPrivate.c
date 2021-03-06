#include "api.h"
#include "os.h"
#include "ux.h"
#include "utils.h"
#include "crypto.h"
#include "globals.h"

void handleGenCoinPrivate(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, volatile unsigned int *flags, volatile unsigned int *tx)
{
  UNUSED(dataLength);
  uint8_t txc = 0;
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
  G_io_apdu_buffer[txc++] = 0x90;
  G_io_apdu_buffer[txc++] = 0x00;
  sendResponse(txc, true);
}
