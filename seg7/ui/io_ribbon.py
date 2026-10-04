from PyQt6.QtWidgets import QWidget, QHBoxLayout, QVBoxLayout, QLabel, QComboBox, QPushButton, QLineEdit, QPlainTextEdit
from PyQt6.QtGui import QFont, QColor, QPalette
from PyQt6.QtCore import QThread, pyqtSignal
from seg7.io.ser import list_serial_ports, find_arduino_port, open_serial
import threading
from datetime import datetime


class SerialReader(QThread):
    """Worker thread to continuously read from serial port."""
    data_received = pyqtSignal(str)
    
    def __init__(self, serial_conn):
        super().__init__()
        self.serial_conn = serial_conn
        self.running = True
    
    def run(self):
        """Read from serial in a loop."""
        while self.running and self.serial_conn and self.serial_conn.is_open:
            try:
                if self.serial_conn.in_waiting:
                    data = self.serial_conn.readline().decode('utf-8', errors='ignore').rstrip()
                    if data:
                        self.data_received.emit(data)
            except Exception as e:
                self.data_received.emit(f"[ERROR] {str(e)}")
                break
    
    def stop(self):
        """Stop the reader thread."""
        self.running = False


class IoRibbon(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.serial_conn = None
        self.reader_thread = None
        self.setMaximumHeight(120)
        
        # Set background color using palette
        palette = self.palette()
        palette.setColor(QPalette.ColorRole.Window, QColor("#2d2d2d"))
        self.setPalette(palette)
        self.setAutoFillBackground(True)
        
        self.setup_ui()
        self.auto_detect()

    def setup_ui(self):
        main_layout = QHBoxLayout()
        main_layout.setContentsMargins(8, 5, 8, 5)
        main_layout.setSpacing(10)
        self.setLayout(main_layout)

        # Controls layout (vertical, on the left)
        controls_layout = QVBoxLayout()
        controls_layout.setSpacing(5)

        # Port controls
        port_layout = QHBoxLayout()
        port_label = QLabel("Port:")
        port_layout.addWidget(port_label)
        self.port_dropdown = QComboBox()
        self.port_dropdown.addItems(self.get_port_names())
        self.port_dropdown.setMaximumWidth(100)
        port_layout.addWidget(self.port_dropdown)
        controls_layout.addLayout(port_layout)

        # Baud controls
        baud_layout = QHBoxLayout()
        baud_label = QLabel("Baud:")
        baud_layout.addWidget(baud_label)
        self.baud_dropdown = QComboBox()
        self.baud_dropdown.addItems(["9600", "19200", "38400", "57600", "115200"])
        self.baud_dropdown.setCurrentText("9600")
        self.baud_dropdown.setMaximumWidth(100)
        baud_layout.addWidget(self.baud_dropdown)
        controls_layout.addLayout(baud_layout)

        # Status and Connect button
        status_layout = QHBoxLayout()
        self.status_label = QLabel("Not connected")
        self.status_label.setStyleSheet("color: orange;")
        self.status_label.setMinimumWidth(120)
        status_layout.addWidget(self.status_label)
        self.connect_button = QPushButton("Connect")
        self.connect_button.clicked.connect(self.connect_serial)
        self.connect_button.setMaximumWidth(100)
        status_layout.addWidget(self.connect_button)
        controls_layout.addLayout(status_layout)

        # Command controls
        command_layout = QHBoxLayout()
        self.command_input = QLineEdit()
        self.command_input.setPlaceholderText("Enter command...")
        self.command_input.returnPressed.connect(self.send_command)
        command_layout.addWidget(self.command_input)
        self.send_button = QPushButton("Send")
        self.send_button.clicked.connect(self.send_command)
        self.send_button.setMaximumWidth(60)
        command_layout.addWidget(self.send_button)
        controls_layout.addLayout(command_layout)

        controls_layout.addStretch()
        
        # Wrap controls in a widget to apply maximum width
        controls_widget = QWidget()
        controls_widget.setLayout(controls_layout)
        controls_widget.setMaximumWidth(400)
        main_layout.addWidget(controls_widget)

        # Log text display (on the right)
        self.log_text = QPlainTextEdit()
        self.log_text.setReadOnly(True)
        self.log_text.setMinimumWidth(300)
        self.log_text.setStyleSheet("background-color: #1e1e1e; color: #d4d4d4; font-family: monospace;")
        main_layout.addWidget(self.log_text)

    def get_port_names(self):
        ports = list_serial_ports()
        return [p.device for p in ports] or ["No ports found"]

    def auto_detect(self):
        port = find_arduino_port()
        if port:
            self.port_dropdown.setCurrentText(port)
            self.connect_serial()

    def connect_serial(self):
        # If already connected, disconnect first
        if self.serial_conn and self.serial_conn.is_open:
            self.disconnect_serial()
            return
        
        # Connect to serial
        port = self.port_dropdown.currentText()
        baud = int(self.baud_dropdown.currentText())
        self.serial_conn = open_serial(port, baud)
        if self.serial_conn:
            self.status_label.setText(f"Connected: {port} @ {baud}")
            self.status_label.setStyleSheet("color: lightgreen;")
            self.connect_button.setText("Disconnect")
            self.start_reader()
        else:
            self.status_label.setText("Connection failed")
            self.status_label.setStyleSheet("color: red;")

    def disconnect_serial(self):
        """Disconnect and close the serial connection."""
        self.stop_reader()
        if self.serial_conn and self.serial_conn.is_open:
            try:
                self.serial_conn.close()
            except Exception as e:
                self.log(f"[ERROR] Failed to close serial: {str(e)}")
        self.serial_conn = None
        self.status_label.setText("Not connected")
        self.status_label.setStyleSheet("color: orange;")
        self.connect_button.setText("Connect")

    def start_reader(self):
        """Start the serial reader thread."""
        if self.reader_thread is None or not self.reader_thread.isRunning():
            self.reader_thread = SerialReader(self.serial_conn)
            self.reader_thread.data_received.connect(self.log)
            self.reader_thread.start()

    def stop_reader(self):
        """Stop the serial reader thread."""
        if self.reader_thread and self.reader_thread.isRunning():
            self.reader_thread.stop()
            self.reader_thread.wait()

    def get_serial_connection(self):
        """Return the active serial connection."""
        return self.serial_conn

    def send_command(self):
        """Send the command text to serial."""
        command = self.command_input.text().strip()
        if command:
            self.serial_send(command)
            self.command_input.clear()

    def serial_send(self, data: str):
        """Send data over serial connection."""
        if not self.serial_conn or not self.serial_conn.is_open:
            self.status_label.setText("Not connected")
            self.status_label.setStyleSheet("color: red;")
            return False
        
        try:
            self.serial_conn.write((data + '\n').encode('utf-8'))
            self.log(f"> {data}")
            return True
        except Exception as e:
            self.status_label.setText(f"Send error: {str(e)}")
            self.status_label.setStyleSheet("color: red;")
            return False

    def log(self, message: str):
        """Append a message to the log text display with timestamp."""
        timestamp = datetime.now().strftime("%H:%M:%S")
        self.log_text.appendPlainText(f"[{timestamp}] {message}")
       