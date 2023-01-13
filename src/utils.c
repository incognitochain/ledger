#include "os.h"
#include "cx.h"
#include <stdbool.h>
#include <stdlib.h>
#include "utils.h"
#include "menu.h"
#include "crypto.h"
#include "globals.h"
#include "string.h"

#define ACCOUNT_ADDRESS_PREFIX 1

static const char BASE_58_ALPHABET[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
                                        'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q',
                                        'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
                                        'h', 'i', 'j', 'k', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                        'w', 'x', 'y', 'z'};

unsigned char encodeBase58(unsigned char WIDE *in, unsigned char length,
                           unsigned char *out, unsigned char maxoutlen)
{
    unsigned char tmp[128];
    unsigned char buffer[224];
    unsigned char j;
    unsigned char startAt;
    unsigned char zeroCount = 0;
    if (length > sizeof(tmp))
    {
        THROW(INVALID_PARAMETER);
    }
    memmove(tmp, in, length);
    while ((zeroCount < length) && (tmp[zeroCount] == 0))
    {
        ++zeroCount;
    }
    j = 2 * length;
    startAt = zeroCount;
    while (startAt < length)
    {
        unsigned short remainder = 0;
        unsigned char divLoop;
        for (divLoop = startAt; divLoop < length; divLoop++)
        {
            unsigned short digit256 = (unsigned short)(tmp[divLoop] & 0xff);
            unsigned short tmpDiv = remainder * 256 + digit256;
            tmp[divLoop] = (unsigned char)(tmpDiv / 58);
            remainder = (tmpDiv % 58);
        }
        if (tmp[startAt] == 0)
        {
            ++startAt;
        }
        buffer[--j] = (unsigned char)BASE_58_ALPHABET[remainder];
    }
    while ((j < (2 * length)) && (buffer[j] == BASE_58_ALPHABET[0]))
    {
        ++j;
    }
    while (zeroCount-- > 0)
    {
        buffer[--j] = BASE_58_ALPHABET[0];
    }
    length = 2 * length - j;
    if (maxoutlen < length)
    {
        THROW(EXCEPTION_OVERFLOW);
    }
    memmove(out, (buffer + j), length);
    return length;
}

uint32_t readUint32BE(uint8_t *buffer)
{
    return (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | (buffer[3]);
}

void sendResponse(uint8_t tx, bool approve)
{
    G_io_apdu_buffer[tx++] = approve ? 0x90 : 0x69;
    G_io_apdu_buffer[tx++] = approve ? 0x00 : 0x85;
    // Send back the response, do not restart the event loop
    io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, tx);
    // Display back the original UX
    ui_idle();
}

void sendUnauthorized() 
{
    G_io_apdu_buffer[0] = 0x69;
    G_io_apdu_buffer[1] = 0x85;
    io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, 2);
    ui_idle();
}

unsigned int ui_prepro(const bagl_element_t *element)
{
    unsigned int display = 1;
    if (element->component.userid > 0)
    {
        display = (ux_step == element->component.userid - 1);
        if (display)
        {
            if (element->component.userid == 1)
            {
                UX_CALLBACK_SET_INTERVAL(2000);
            }
            else
            {
                UX_CALLBACK_SET_INTERVAL(MAX(3000, 1000 + bagl_label_roundtrip_duration_ms(element, 7)));
            }
        }
    }
    return display;
}
#define ADDR_CHECKSUM_SIZE 4
#define ADDR_LEN 95
#define INTEGRATED_ADDR_LEN 106