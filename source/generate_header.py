import sys

def generate_header_file(input_file_path, output_file_path):
    try:
        # Read the contents of the input text file
        with open(input_file_path, 'r') as input_file:
            text_content = input_file.read()

        # Escape special characters in the text content
        text_content = text_content.replace('\\', '\\\\')  # Escape backslashes
        text_content = text_content.replace('"', '\\"')    # Escape double quotes
        text_content = text_content.replace('\n', '\\n')   # Replace newlines with escape sequence

        # Write the text content to the output header file
        with open(output_file_path, 'w') as output_file:
            output_file.write('#ifndef TEXT_DATA_H\n')
            output_file.write('#define TEXT_DATA_H\n\n')
            output_file.write('const char text_data[] = "{}";\n'.format(text_content))
            output_file.write('\n#endif // TEXT_DATA_H\n')

        print("Header file '{}' generated successfully.".format(output_file_path))
    except Exception as e:
        print("Error:", e)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python generate_header.py <input_file_path> <output_file_path>")
    else:
        input_file_path = sys.argv[1]
        output_file_path = sys.argv[2]
        generate_header_file(input_file_path, output_file_path)
