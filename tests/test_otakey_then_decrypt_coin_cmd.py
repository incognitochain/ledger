from application_client.boilerplate_transaction import Transaction
from application_client.boilerplate_command_sender import BoilerplateCommandSender, Errors
from application_client.boilerplate_response_unpacker import unpack_raw_bytes_response
import binascii

# If the user has just approved "Export OTA Key", decrypt_coin will not require approval
def test_otakey_then_decrypt_coin(backend, navigator, test_name):
    # Use the app interface instead of raw interface
    client = BoilerplateCommandSender(backend)

    testdata = "b1f0aa68677fe66df4883c8c3bb4a5581bcf9b8d386155933ad0f81e6d2afba4ddf56e835d26e804bcb0afb73812d9f4376a5d30c1d00dbaf4bc1812a20d7f07bb4d8412fd78f5438dee1467da0788a50075895612fa173e215cb88883d47907"
    # Send the sign device instruction.
    # As it requires on-screen validation, the function is asynchronous.
    # It will yield the result when the navigation is done
    rapdu = client.decrypt_coin(0, 0, binascii.unhexlify(testdata))

    result = unpack_raw_bytes_response(rapdu.data)
    expectedResult = "005ed0b20000000000000000000000000000000000000000000000000000000099d53e87cbc09b367ebff180fd19d6ddc794905fafb7c2325fe11c9811e21606"
    assert result == expectedResult
