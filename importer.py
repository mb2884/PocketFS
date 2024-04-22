import sys
import os
from PyQt5.QtWidgets import QApplication, QPushButton, QLabel, QMainWindow, QGridLayout, QFrame, QSizePolicy, QHBoxLayout, QCheckBox, QFileDialog, QMessageBox
from PyQt5.QtGui import QIcon, QPixmap
from PyQt5.QtCore import Qt, QSize


def serialize_directory(directory_path):
    serialized_str = 'D ' + os.path.basename(directory_path) + ' {'
    for item in os.listdir(directory_path):
        item_path = os.path.join(directory_path, item)
        if os.path.isdir(item_path):
            serialized_str += serialize_directory(item_path)
        elif os.path.isfile(item_path):
            if item_path.endswith('.txt'):  # Check if the file is a text file
                with open(item_path, 'r') as file:
                    file_content = file.read().replace('"', r'\"')
                    file_content = file_content.replace('\n', r'\n')
                    file_content = file_content.replace('\t', r'\t')
                    file_content = file_content.replace('{', r'(')
                    file_content = file_content.replace('}', r')')
                serialized_str += 'F ' + item + ' {' + file_content + '}'
    serialized_str += '}'
    return serialized_str


def select_directory():
    directory_path = QFileDialog.getExistingDirectory(None, "Select Directory")
    if directory_path:
        serialized_str = serialize_directory(directory_path)
        save_to_file(serialized_str)


def save_to_file(serialized_str):
    file_path = "./source/imported_data.h"
    try:
        with open(file_path, "w") as file:
            file.write("#ifndef IMPORTED_DATA_H\n")
            file.write("#define IMPORTED_DATA_H\n\n")
            file.write("const char* imported_text_data = \"")
            file.write(serialized_str)
            file.write("\";\n\n#endif")
        QMessageBox.information(None, "Success", "File saved successfully!")
    except Exception as e:
        QMessageBox.critical(None, "Error", f"Error saving file: {e}")


def update_main_c(checkbox):
    if checkbox.isChecked():
        with open("./source/main.c", "r") as file:
            lines = file.readlines()
        with open("./source/main.c", "w") as file:
            for line in lines:
                if "#define USE_SAVE" in line:
                    file.write("#define USE_SAVE 0\n")
                elif "#define USE_IMPORT" in line:
                    file.write("#define USE_IMPORT 1\n")
                else:
                    file.write(line)
    else:
        with open("./source/main.c", "r") as file:
            lines = file.readlines()
        with open("./source/main.c", "w") as file:
            for line in lines:
                if "#define USE_SAVE" in line:
                    file.write("#define USE_SAVE 1\n")
                elif "#define USE_IMPORT" in line:
                    file.write("#define USE_IMPORT 0\n")
                else:
                    file.write(line)


def create_widgets():
    # Load and display the image for the title
    title_label = QLabel()
    # Adjust the path accordingly
    title_pixmap = QPixmap("./assets/title.png")
    title_label.setPixmap(title_pixmap)

    # Ensure the title label takes the full width within its layout
    title_layout = QHBoxLayout()
    # Center the title label within its layout
    title_layout.addWidget(title_label, alignment=Qt.AlignCenter)

    # Set font for title label
    title_font = title_label.font()
    title_font.setFamily("Cascadia Mono")
    title_label.setFont(title_font)

    # HTML formatted text for instructions label
    instructions_text = "<h2>Instructions:</h2>" \
                        "<div style='width: 300px; font-family: Cascadia Mono;'>" \
                        "<ol style='word-wrap: break-word; padding-left: 20px;'>" \
                        "<li style='margin-bottom: 10px;'>Click the Browse button below and select a directory to be the root directory to import into PocketFS.</li>" \
                        "<li style='margin-bottom: 10px;'>Make sure to check the checkbox if you wish for PocketFS to load your imported data over any saved data. This will correctly set the <tt>#define</tt> in <tt>main.c</tt>.</li>" \
                        "<li style='margin-bottom: 10px;'>Be sure to double-check the limitations of PocketFS and its importer on the <a href='https://github.com/mb2884/PocketFS'>readme</a>.</li>" \
                        "</ol>" \
                        "</div>"
    instructions_label = QLabel()
    instructions_label.setTextFormat(Qt.RichText)
    instructions_label.setTextInteractionFlags(Qt.TextBrowserInteraction)
    instructions_label.setOpenExternalLinks(True)
    instructions_label.setText(instructions_text)
    instructions_label.setAlignment(Qt.AlignLeft)  # Set alignment to left
    # Enable word wrap for multi-line text
    instructions_label.setMinimumHeight(300)
    instructions_label.setWordWrap(True)

    # Set font for instructions label
    instructions_font = instructions_label.font()
    instructions_font.setFamily("Cascadia Mono")
    instructions_label.setFont(instructions_font)

    # Load and display the image
    image_label = QLabel()
    pixmap = QPixmap("./assets/defines.png")  # Adjust the path accordingly
    # Adjust the width of the image as needed
    pixmap = pixmap.scaledToWidth(300)
    image_label.setPixmap(pixmap)
    image_label.setMaximumWidth(400)  # Set maximum width for the image

    browse_button = QPushButton("Browse")
    # Set icon for the browse button
    browse_button.setIcon(QIcon("./assets/folder_icon.png"))
    browse_button.setIconSize(QSize(20, 20))  # Set size for the icon

    # Set font for browse button
    browse_font = browse_button.font()
    browse_font.setFamily("Cascadia Mono")
    browse_button.setFont(browse_font)

    # Apply some styling
    instructions_label.setStyleSheet("color: #666; font-size: 16px;")
    browse_button.setStyleSheet(
        "border: none; padding: 0.75rem 1.5rem; background-color: #7734B3; color: #ffffff; font-size: 0.75rem; line-height: 1rem; font-weight: 700; text-align: center; text-transform: uppercase; vertical-align: middle; border-radius: 0.5rem;")
    browse_button.setMinimumWidth(150)  # Set minimum width
    browse_button.setMinimumHeight(50)  # Set minimum height

    checkbox = QCheckBox(
        "Uncheck to use saved data.\nCheck to use imported data.")
    checkbox.setStyleSheet("color: #666; font-size: 16px; padding-left: 15px;")

    checkbox_font = checkbox.font()
    checkbox_font.setFamily("Cascadia Mono")
    checkbox.setFont(checkbox_font)

    layout = QGridLayout()
    # Centered title label spanning 2 columns
    layout.addLayout(title_layout, 0, 0, 1, 2, alignment=Qt.AlignCenter)
    layout.addWidget(instructions_label, 1, 0, 1, 1, alignment=Qt.AlignLeft)
    layout.addWidget(image_label, 1, 1, 2, 1, alignment=Qt.AlignCenter)
    layout.addWidget(checkbox, 2, 0, 1, 2, alignment=Qt.AlignLeft)
    layout.addWidget(browse_button, 3, 0, 1, 2, alignment=Qt.AlignCenter)

    # Set horizontal stretch for the first column
    layout.setColumnStretch(0, 1)
    # Set horizontal stretch for the second column
    layout.setColumnStretch(1, 1)
    layout.setRowStretch(4, 1)

    frame = QFrame()
    frame.setLayout(layout)

    window = QMainWindow()
    window.setWindowTitle("PocketFS Importer")
    window.setCentralWidget(frame)

    window_icon = QIcon("./assets/windowlogo.png")
    window.setWindowIcon(window_icon)

    # Allow the window to resize dynamically
    window.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)

    return window, browse_button, checkbox


def main():
    app = QApplication(sys.argv)
    window, browse_button, checkbox = create_widgets()
    browse_button.clicked.connect(select_directory)
    checkbox.stateChanged.connect(lambda: update_main_c(checkbox))
    update_main_c(checkbox)

    window.show()
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
