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



def int_to_hex_string(hex_value):
    hex_str = f"{hex_value:04X}"
    return " ".join(hex_str[i:i+2] for i in range(0, len(hex_str), 2))


def reverse_bytes_for_little_endian(hex_data):
    reversed_data = []
    for byte_pair in hex_data:
        reversed_data.append(" ".join(byte_pair.split(' ')[::-1]))
    return reversed_data
