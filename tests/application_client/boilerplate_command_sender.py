from enum import IntEnum
from typing import Generator, List, Optional
from contextlib import contextmanager

from ragger.backend.interface import BackendInterface, RAPDU
from ragger.bip import pack_derivation_path


MAX_APDU_LEN: int = 255

CLA: int = 0xE0


class P1(IntEnum):
    # Parameter 1 for first APDU number.
    P1_START = 0x00
    # Parameter 1 for maximum APDU number.
    P1_MAX = 0x03
    # Parameter 1 for screen confirmation for GET_PUBLIC_KEY.
    P1_CONFIRM = 0x01


class P2(IntEnum):
    # Parameter 2 for last APDU to receive.
    P2_LAST = 0x00
    # Parameter 2 for more APDU to receive.
    P2_MORE = 0x80


class InsType(IntEnum):
    GET_VERSION = 0x01
    GET_ADDRESS = 0x02
    SWITCH_KEY = 0x05
    GET_OTA_KEY = 0x06
    GET_KEY_IMAGE = 0x10
    GEN_ALPHA = 0x21
    CALC_C = 0x22
    CALC_CCA = 0x28
    CALC_R = 0x23
    COIN_PRIVATE = 0x24
    DECRYPT_COIN = 0x27
    TRUST_DEVICE = 0x40
    SIGN_SCHNORR = 0x60


class Errors(IntEnum):
    SW_DENY = 0x6985
    SW_WRONG_P1P2 = 0x6A86
    SW_WRONG_DATA_LENGTH = 0x6A87
    SW_INS_NOT_SUPPORTED = 0x6D00
    SW_CLA_NOT_SUPPORTED = 0x6E00
    SW_WRONG_RESPONSE_LENGTH = 0xB000
    SW_DISPLAY_BIP32_PATH_FAIL = 0xB001
    SW_DISPLAY_ADDRESS_FAIL = 0xB002
    SW_DISPLAY_AMOUNT_FAIL = 0xB003
    SW_WRONG_TX_LENGTH = 0xB004
    SW_TX_PARSING_FAIL = 0xB005
    SW_TX_HASH_FAIL = 0xB006
    SW_BAD_STATE = 0xB007
    SW_SIGNATURE_FAIL = 0xB008


def split_message(message: bytes, max_size: int) -> List[bytes]:
    return [message[x:x + max_size] for x in range(0, len(message), max_size)]


class BoilerplateCommandSender:
    def __init__(self, backend: BackendInterface) -> None:
        self.backend = backend

    def get_version(self) -> RAPDU:
        return self.backend.exchange(cla=CLA,
                                     ins=InsType.GET_VERSION,
                                     p1=P1.P1_START,
                                     p2=P2.P2_LAST,
                                     data=b"")

    def get_address(self) -> RAPDU:
        return self.backend.exchange(cla=CLA,
                                     ins=InsType.GET_ADDRESS,
                                     p1=P1.P1_START,
                                     p2=P2.P2_LAST,
                                     data=b"")

    def get_ota_key(self) -> RAPDU:
        return self.backend.exchange_async(cla=CLA,
                                           ins=InsType.GET_OTA_KEY,
                                           p1=P1.P1_START,
                                           p2=P2.P2_LAST,
                                           data=b"")

    def switch_key(self, path: str) -> RAPDU:
        return self.backend.exchange_async(cla=CLA,
                                           ins=InsType.SWITCH_KEY,
                                           p1=P1.P1_START,
                                           p2=P2.P2_LAST,
                                           data=pack_derivation_path(path))

    def gen_key_image(self, data) -> RAPDU:
        return self.backend.exchange_async(cla=CLA,
                                           ins=InsType.GET_KEY_IMAGE,
                                           p1=P1.P1_START,
                                           p2=P2.P2_LAST,
                                           data=data)

    def gen_alpha(self, p1) -> RAPDU:
        return self.backend.exchange(cla=CLA,
                                     ins=InsType.GEN_ALPHA,
                                     p1=p1,
                                     p2=P2.P2_LAST,
                                     data=b"")

    def calc_c(self, data: bytes, isca: bool) -> RAPDU:
        return self.backend.exchange(cla=CLA,
                                     ins=InsType.CALC_CCA if isca else InsType.CALC_C,
                                     p1=P1.P1_START,
                                     p2=P2.P2_LAST,
                                     data=data)

    def calc_r(self, p1, p2, data) -> RAPDU:
        return self.backend.exchange(cla=CLA,
                                     ins=InsType.GET_KEY_IMAGE,
                                     p1=p1,
                                     p2=p2,
                                     data=data)

    def get_coin_private(self, p1, p2, data) -> RAPDU:
        return self.backend.exchange(cla=CLA,
                                     ins=InsType.COIN_PRIVATE,
                                     p1=p1,
                                     p2=p2,
                                     data=data)

    def decrypt_coin(self, p1, p2, data) -> RAPDU:
        return self.backend.exchange(cla=CLA,
                                     ins=InsType.DECRYPT_COIN,
                                     p1=p1,
                                     p2=p2,
                                     data=data)

    @contextmanager
    def get_public_key_with_confirmation(self, path: str) -> Generator[None, None, None]:
        with self.backend.exchange_async(cla=CLA,
                                         ins=InsType.GET_PUBLIC_KEY,
                                         p1=P1.P1_CONFIRM,
                                         p2=P2.P2_LAST,
                                         data=pack_derivation_path(path)) as response:
            yield response

    @contextmanager
    def sign_tx(self, path: str, transaction: bytes) -> Generator[None, None, None]:
        self.backend.exchange(cla=CLA,
                              ins=InsType.SIGN_TX,
                              p1=P1.P1_START,
                              p2=P2.P2_MORE,
                              data=pack_derivation_path(path))
        messages = split_message(transaction, MAX_APDU_LEN)
        idx: int = P1.P1_START + 1

        for msg in messages[:-1]:
            self.backend.exchange(cla=CLA,
                                  ins=InsType.SIGN_TX,
                                  p1=idx,
                                  p2=P2.P2_MORE,
                                  data=msg)
            idx += 1

        with self.backend.exchange_async(cla=CLA,
                                         ins=InsType.SIGN_TX,
                                         p1=idx,
                                         p2=P2.P2_LAST,
                                         data=messages[-1]) as response:
            yield response

    def get_async_response(self) -> Optional[RAPDU]:
        return self.backend.last_async_response
