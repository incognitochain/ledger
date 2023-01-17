from application_client.boilerplate_transaction import Transaction
from application_client.boilerplate_command_sender import BoilerplateCommandSender, Errors
from application_client.boilerplate_response_unpacker import unpack_raw_bytes_response
from ragger.backend import RaisePolicy
from ragger.navigator import NavInsID, NavIns
from utils import ROOT_SCREENSHOT_PATH, check_signature_validity
import binascii

# In this tests we check the behavior of the device when asked to sign a transaction

# Call functions to compute spend key of input coin; result stays on device
def test_coin_priv_no_confirm(backend, navigator, test_name):
    # Use the app interface instead of raw interface
    client = BoilerplateCommandSender(backend)

    testdata = "6a6b04419dd81f92133446d85cdc1a74d2e4e53927d674622b4de25ccd06bb05"
    rapdu = client.get_coin_private(0, 0, binascii.unhexlify(testdata))
    assert rapdu.status == 0x9000

    testdata = "dd8f6d9c86b014354659becdfedafb0d6a25e137f9a82978eb8f6d7d729edc00"
    rapdu = client.get_coin_private(1, 1, binascii.unhexlify(testdata))
    assert rapdu.status == 0x9000

# In this test we initiate TX ring signing. User has to approve
# for signing to continue
def test_sign_ring_confirm_accepted(backend, navigator, test_name):
    # Use the app interface instead of raw interface
    client = BoilerplateCommandSender(backend)

    testdata = "4e57ee48b2c646d357218cae541205369ede22d10e7d425b5367ddb366cc7e0c"
    # Send the sign device instruction.
    # As it requires on-screen validation, the function is asynchronous.
    # It will yield the result when the navigation is done
    with client.calc_r(0, 0, binascii.unhexlify(testdata)):
        # Validate the on-screen request by performing the navigation appropriate for this device
        navigator.navigate_until_text_and_compare(NavIns(NavInsID.RIGHT_CLICK),
                                                  [NavIns(NavInsID.BOTH_CLICK)],
                                                  "Approve",
                                                  ROOT_SCREENSHOT_PATH,
                                                  test_name)

    # The device as yielded the result, parse it and ensure that the signature is correct
    response = client.get_async_response().data
    result = unpack_raw_bytes_response(response)
    expectedResult = "4bd391770eb0b3a9ea6edfdc188f19f7e16f8db4f8518f2c2189265451912f01"
    assert result == expectedResult

def test_sign_ring_confirm_refused(backend, navigator, test_name):
    # Use the app interface instead of raw interface
    client = BoilerplateCommandSender(backend)

    testdata = "4e57ee48b2c646d357218cae541205369ede22d10e7d425b5367ddb366cc7e0c"
    # Send the sign device instruction.
    # As it requires on-screen validation, the function is asynchronous.
    # It will yield the result when the navigation is done
    with client.calc_r(0, 0, binascii.unhexlify(testdata)):
        # Validate the on-screen request by performing the navigation appropriate for this device
        navigator.navigate_until_text_and_compare(NavIns(NavInsID.RIGHT_CLICK),
                                                  [NavIns(NavInsID.BOTH_CLICK)],
                                                  "Reject",
                                                  ROOT_SCREENSHOT_PATH,
                                                  test_name)

    # The device as yielded the result, parse it and ensure that the signature is correct
    response = client.get_async_response()

    # Assert that we have received a refusal
    assert response.status == Errors.SW_DENY
    assert len(response.data) == 0

def test_sign_ring_after_confirm(backend, navigator, test_name):
    # Use the app interface instead of raw interface
    client = BoilerplateCommandSender(backend)

    testdata = "4e57ee48b2c646d357218cae541205369ede22d10e7d425b5367ddb366cc7e0c"
    # Send the sign device instruction.
    # As it requires on-screen validation, the function is asynchronous.
    # It will yield the result when the navigation is done
    rapdu = client.calc_r(0, 1, binascii.unhexlify(testdata))
    # The device as yielded the result, parse it and ensure that the signature is correct
    result = unpack_raw_bytes_response(rapdu.data)
    expectedResult = "d34365f4385af97e4aaeeb3de5de52039efd3bd0dccd7fec02d7a0cb341d710d"
    assert result == expectedResult
