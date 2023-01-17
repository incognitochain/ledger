from application_client.boilerplate_transaction import Transaction
from application_client.boilerplate_command_sender import BoilerplateCommandSender, Errors
from application_client.boilerplate_response_unpacker import unpack_raw_bytes_response
from ragger.backend import RaisePolicy
from ragger.navigator import NavInsID, NavIns
from utils import ROOT_SCREENSHOT_PATH, check_signature_validity
import binascii

def test_prepare_ring_native_token(backend, navigator, test_name):
    # Use the app interface instead of raw interface
    client = BoilerplateCommandSender(backend)

    testdata = "64cb8908d328499e532d730d1af9e0aeede74d9666245226e4ea0ddea77c8993"
    # call debug function set_alpha; real flow uses gen_alpha which randomizes the values
    client.set_alpha(0, binascii.unhexlify("bcaf88d9288ca2bd422346e0c9754f604407a12a46a66291a6935714cd00b303"))
    client.set_alpha(1, binascii.unhexlify("dd8f6d9c86b014354659becdfedafb0d6a25e137f9a82978eb8f6d7d729edc00"))
    rapdu = client.calc_c(0, 0, binascii.unhexlify(testdata), False)
    # The device as yielded the result, parse it and ensure that the signature is correct
    result = unpack_raw_bytes_response(rapdu.data)
    expectedResult = "3e2adcc1f12dbac45897b6cb601ff3d169d1375a5c590d062253659a5ba67b49d367b94d54a3c697c3473396fb2549e15e309787eb4d36d016608c9a8811ce0e"
    assert result == expectedResult

def test_prepare_ring_conf_assets(backend, navigator, test_name):
    # Use the app interface instead of raw interface
    client = BoilerplateCommandSender(backend)

    testdata = "64cb8908d328499e532d730d1af9e0aeede74d9666245226e4ea0ddea77c8993"
    # Send the sign device instruction.
    # As it requires on-screen validation, the function is asynchronous.
    # It will yield the result when the navigation is done
    rapdu = client.calc_c(0, 0, binascii.unhexlify(testdata), True)
    # The device as yielded the result, parse it and ensure that the signature is correct
    result = unpack_raw_bytes_response(rapdu.data)
    expectedResult = "01000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000"
    assert result == expectedResult
