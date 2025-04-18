import os


def write_hex_file(file_path, hex_data):
    print("Saving...")
    try:
        binary_data = bytes.fromhex(hex_data)

        directory = os.path.dirname(file_path)
        if directory and not os.path.exists(directory):
            os.makedirs(directory)

        # Write the binary data to the file
        with open(file_path, "wb") as file:
            file.write(binary_data)
        print("Done!")

    except ValueError as e:
        print(f"Error converting hex data to binary: {e}")
    except IOError as e:
        print(f"Error writing file {file_path}: {e}")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")


def int16_to_hex_string(value: int) -> str:
    """Converts a 16-bit integer to a space-separated hex string (e.g., '1234' -> '12 34')."""
    if not (0 <= value <= 0xFFFF):
        raise ValueError("Value must be a 16-bit unsigned integer (0–65535).")
    hex_str = f"{value:04X}"
    return " ".join(hex_str[i:i+2] for i in range(0, 4, 2))


def int32_to_hex_string(value: int) -> str:
    """Converts a 32-bit integer to a space-separated hex string (e.g., '12345678' -> '12 34 56 78')."""
    if not (0 <= value <= 0xFFFFFFFF):
        raise ValueError("Value must be a 32-bit unsigned integer.")
    hex_str = f"{value:08X}"
    return " ".join(hex_str[i:i+2] for i in range(0, 8, 2))


def int64_to_hex_string(value: int) -> str:
    """Converts a 64-bit integer to a space-separated hex string."""
    if not (0 <= value <= 0xFFFFFFFFFFFFFFFF):
        raise ValueError("Value must be a 64-bit unsigned integer.")
    hex_str = f"{value:016X}"
    return " ".join(hex_str[i:i+2] for i in range(0, 16, 2))



def reverse_bytes_for_little_endian(hex_data):
    reversed_data = []
    for byte_pair in hex_data:
        reversed_data.append(" ".join(byte_pair.split(' ')[::-1]))
    return reversed_data
