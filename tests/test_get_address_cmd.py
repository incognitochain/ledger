from application_client.boilerplate_command_sender import BoilerplateCommandSender
from application_client.boilerplate_response_unpacker import unpack_str_response


def test_get_address(backend, backend_name, firmware):
    # Use the app interface instead of raw interface
    client = BoilerplateCommandSender(backend)
    # Send the special instruction to BOLOS
    response = client.get_address()
    # Use an helper to parse the response, assert the values
    address_str = unpack_str_response(response.data)

    assert address_str == "12siTz4vkkL4cdUMff2cwJUrkyDL1vdEz6dgCcA6KGCS1rFZ1sqWKhQcchzF3eK5PmR5L1TksHxJYUuis65CAk5HNJ1N7FyuUjnDy1hqBUJwQQpRrd3es5Fs8bWn6XcybYC4PtybN2CMnqeMZbaT"
