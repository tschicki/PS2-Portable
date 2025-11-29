def merge_hex_files(hex_file1, hex_file2, output_file):
    merged_data = b""

    # Read data from the first file
    with open(hex_file1, "rb") as file:
        file1 = file.read()
        bin_length = len(file1)

        if(bin_length < 4194304):
            len_diff = 4194304 - bin_length
            merged_data += file1
            merged_data += b'\0' * len_diff
        else:
            merged_data += file1

    # Read data from the second file
    with open(hex_file2, "rb") as file:
        file2 = file.read()
        bin_length = len(file2)

        if(bin_length < 4194304):
            len_diff = 4194304 - bin_length
            merged_data += file2
            merged_data += b'\0' * len_diff
        else:
            merged_data += file2

    # Write merged data to the output file
    with open(output_file, "wb") as file:
        file.write(merged_data)

    print("Hex files merged successfully!")


# Specify the file names
hex_file2 = "BOOT_ROM.bin"
hex_file1 = "DVD_ROM.bin"
output_file = "MERGED_ROM.bin"

# Call the merge_hex_files function
merge_hex_files(hex_file1, hex_file2, output_file)