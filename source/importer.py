import os
from PyQt5.QtWidgets import QApplication, QWidget, QPushButton, QFileDialog, QMessageBox


def serialize_directory(directory_path, indent=0):
    serialized_str = f'{"    " * indent}"D {os.path.basename(directory_path)}"\n'
    for item in os.listdir(directory_path):
        item_path = os.path.join(directory_path, item)
        if os.path.isdir(item_path):
            serialized_str += serialize_directory(item_path, indent + 1)
        elif os.path.isfile(item_path):
            serialized_str += f'{"    " * (indent + 1)}"F {item}"\n'
    return serialized_str


class MainWindow(QWidget):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("Directory Serializer")
        self.setGeometry(100, 100, 400, 200)

        self.select_button = QPushButton("Select Directory", self)
        self.select_button.setGeometry(100, 50, 200, 50)
        self.select_button.clicked.connect(self.select_directory)

    def select_directory(self):
        directory_path = QFileDialog.getExistingDirectory(self, "Select Directory")
        if directory_path:
            serialized_str = serialize_directory(directory_path)
            self.save_to_file(serialized_str)

    def save_to_file(self, serialized_str):
        file_path = "imported_data.h"
        try:
            with open(file_path, "w") as file:
                file.write("#ifndef IMPORTED_DATA_H\n")
                file.write("#define IMPORTED_DATA_H\n")
                file.write("const char* text_data =\n")
                file.write(serialized_str)
                file.write("\n#endif")
            QMessageBox.information(self, "Success", "File saved successfully!")
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Error saving file: {e}")


if __name__ == "__main__":
    app = QApplication([])
    window = MainWindow()
    window.show()
    app.exec_()