from application_client.boilerplate_command_sender import BoilerplateCommandSender, Errors
from application_client.boilerplate_response_unpacker import unpack_raw_bytes_response
from ragger.bip import calculate_public_key_and_chaincode, CurveChoice
from ragger.backend import RaisePolicy
from ragger.navigator import NavInsID, NavIns
from utils import ROOT_SCREENSHOT_PATH


# In this test we check that the GET_OTA_KEY works in non-confirmation mode
# def test_get_public_key_no_confirm(backend, firmware):
#     for path in []:
#         client = BoilerplateCommandSender(backend)
#         response = client.get_public_key(path=path).data
#         _, public_key, _, chain_code = unpack_get_public_key_response(response)

#         ref_public_key, ref_chain_code = calculate_public_key_and_chaincode(CurveChoice.Secp256k1, path=path)
#         assert public_key.hex() == ref_public_key
#         assert chain_code.hex() == ref_chain_code

# In this test we check that the GET_OTA_KEY
# in confirmation mode replies an error if the user refuses
def test_get_ota_key_confirm_refused(backend, firmware, navigator, test_name):
    client = BoilerplateCommandSender(backend)
    backend.raise_policy = RaisePolicy.RAISE_NOTHING
    with client.get_ota_key():
        # Disable raising when trying to unpack an error APDU
        navigator.navigate_until_text_and_compare(NavIns(NavInsID.RIGHT_CLICK),
                                                  [NavIns(NavInsID.BOTH_CLICK)],
                                                  "Reject",
                                                  ROOT_SCREENSHOT_PATH,
                                                  test_name)

    response = client.get_async_response()

    # Assert that we have received a refusal
    assert response.status == Errors.SW_DENY
    assert len(response.data) == 0


# In this test we check that the GET_OTA_KEY works in confirmation mode
def test_get_ota_key_confirm_accepted(backend, firmware, navigator, test_name):
    client = BoilerplateCommandSender(backend)
    with client.get_ota_key():
        navigator.navigate_until_text_and_compare(NavIns(NavInsID.RIGHT_CLICK),
                                                  [NavIns(NavInsID.BOTH_CLICK)],
                                                  "Approve",
                                                  ROOT_SCREENSHOT_PATH,
                                                  test_name)
    response = client.get_async_response().data
    otakey = unpack_raw_bytes_response(response)

    assert otakey == "aecfe32b37192ac9899a0c07097a9372256e3fb1feea077c41e62fd7fba9330f"
