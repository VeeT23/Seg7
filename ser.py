# serial_helpers.py
import serial
import serial.tools.list_ports

# -------------------- Serial Helpers --------------------

def list_serial_ports():
    """Return a list of available COM/serial ports."""
    return list(serial.tools.list_ports.comports())

def find_arduino_port():
    """Try to auto-detect an Arduino by description."""
    for port in list_serial_ports():
        desc = port.description.lower()
        if any(x in desc for x in ["arduino", "ch340", "cp210", "ftdi"]):
            return port.device
    return None

def open_serial(port: str, baud: int = 9600, timeout: float = 1):
    """Open a serial connection safely."""
    try:
        ser = serial.Serial(port, baud, timeout=timeout)
        return ser
    except Exception as e:
        print(f"[ERROR] Could not open serial port {port}: {e}")
        return None
