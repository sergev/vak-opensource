#!/usr/bin/env python3
import sys
from PySide6.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout,
                               QPushButton, QLabel, QFileDialog, QMessageBox)
from PySide6.QtCore import Qt

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        # Window setup
        self.setWindowTitle("PySide6 Demo App")
        self.setGeometry(100, 100, 400, 200)  # Position and size

        # Central widget and layout
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        layout = QVBoxLayout(central_widget)
        layout.setAlignment(Qt.AlignmentFlag.AlignCenter)
        layout.setSpacing(20)

        # Title label
        self.title_label = QLabel("Welcome to PySide6!")
        self.title_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        layout.addWidget(self.title_label)

        # File path display label
        self.file_label = QLabel("No file selected yet.")
        self.file_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        layout.addWidget(self.file_label)

        # Open File button
        self.open_button = QPushButton("Open File")
        self.open_button.clicked.connect(self.open_file_dialog)
        layout.addWidget(self.open_button)

        # Quit button
        self.quit_button = QPushButton("Quit")
        self.quit_button.clicked.connect(self.quit_app)
        layout.addWidget(self.quit_button)

        # Styling for a modern look
        self.apply_styles()

    def apply_styles(self):
        """Apply basic styling for a modern appearance."""
        self.setStyleSheet("""
            QMainWindow {
                background-color: #f0f0f0;
            }
            QLabel {
                font-size: 16px;
                color: #333;
            }
            QPushButton {
                background-color: #4a90e2;
                color: white;
                border-radius: 5px;
                padding: 8px;
                font-size: 14px;
            }
            QPushButton:hover {
                background-color: #357abd;
            }
        """)

    def open_file_dialog(self):
        """Open a native file dialog and update the label."""
        file_name, _ = QFileDialog.getOpenFileName(
            self,
            "Select a File",
            "",
            "All Files (*);;Text Files (*.txt);;Python Files (*.py)"
        )
        if file_name:
            self.file_label.setText(f"Selected: {file_name}")
        else:
            self.file_label.setText("No file selected.")

    def quit_app(self):
        """Show a native confirmation dialog before quitting."""
        reply = QMessageBox.question(
            self,
            "Confirm Quit",
            "Are you sure you want to quit?",
            QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No,
            QMessageBox.StandardButton.No
        )
        if reply == QMessageBox.StandardButton.Yes:
            QApplication.quit()

# Run the application
if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())
