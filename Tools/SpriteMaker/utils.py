import os


def write_binary_file(file_path: str, binary_data: bytes):
    """
    Writes raw binary data to a file.

    Parameters:
    - file_path: Path to the output file.
    - binary_data: Byte array or bytes object to write directly to disk.
    """
    print("Saving...")
    print("Header bytes:", binary_data[:10].hex(" "))
    try:
        directory = os.path.dirname(file_path)
        if directory and not os.path.exists(directory):
            os.makedirs(directory)

        with open(file_path, "wb") as file:
            file.write(binary_data)

        print("Done!")

    except IOError as e:
        print(f"Error writing file {file_path}: {e}")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")





