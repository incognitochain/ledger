from application_client.boilerplate_transaction import Transaction
from application_client.boilerplate_command_sender import BoilerplateCommandSender, Errors
from application_client.boilerplate_response_unpacker import unpack_raw_bytes_response
import binascii

# If the user has just approved "Export OTA Key", get_key_image will not require approval
def test_otakey_then_key_image(backend, navigator, test_name):
    # Use the app interface instead of raw interface
    client = BoilerplateCommandSender(backend)

    testdata = "4bbb5070174aa82a6a2b9863ebae048319bdfb0e5f05d2c1164a3f9bfec9190164cb8908d328499e532d730d1af9e0aeede74d9666245226e4ea0ddea77c8993"
    # Send the sign device instruction.
    # As it requires on-screen validation, the function is asynchronous.
    # It will yield the result when the navigation is done
    rapdu = client.gen_key_image(binascii.unhexlify(testdata))

    result = unpack_raw_bytes_response(rapdu.data)
    expectedResult = "81b7473a3fcb0f74b7511758ec04f86a3632677ce23d44433ef98390bc7aa159"
    assert result == expectedResult
