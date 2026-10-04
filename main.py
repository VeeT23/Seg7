import sys
from PyQt6.QtWidgets import QApplication, QMainWindow, QWidget, QVBoxLayout, QLabel
from PyQt6.QtGui import QFont
from seg7.ui.io_ribbon import IoRibbon


class Seg7App(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Seg7 - Arduino 7 Segment Display Controller")
        self.create_widgets()

    def create_widgets(self):
        central_widget = QWidget()
        self.setCentralWidget(central_widget)

        layout = QVBoxLayout()
        layout.setContentsMargins(0, 0, 0, 0)
        central_widget.setLayout(layout)

        # Main content area (expandable)
        layout.addStretch()

        # IO Ribbon at the bottom
        self.io_ribbon = IoRibbon()
        layout.addWidget(self.io_ribbon)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = Seg7App()
    window.showMaximized()
    sys.exit(app.exec())
