import serial
import serial.tools.list_ports
import concurrent.futures
import queue
import threading
import time


def check_port(port, baudrate=115200, timeout=1):
    """Check an individual serial port."""

    try:
        print(f"Checking {port}...")

        with serial.Serial(
            port=port,
            baudrate=baudrate,
            timeout=timeout,
            write_timeout=timeout
        ) as ser:

            time.sleep(0.2)
            ser.reset_input_buffer()

            ser.write(b"PING\n")
            ser.flush()

            response = ser.readline().strip()

            if response == b"PONG":
                print(f"Found device on {port}")
                return port

    except (serial.SerialException, OSError) as e:
        print(f"Error checking {port}: {e}")

    return None


class SerialScanner:

    def __init__(self, baudrate=115200, timeout=1):
        self.baudrate = baudrate
        self.timeout = timeout

        self.result_queue = queue.Queue()
        self.executor = None
        self.futures = []
        self.scanning = False
        self.lock = threading.Lock()

    def start_scan(self):
        with self.lock:
            if self.scanning:
                return

            self.scanning = True
            self.result_queue = queue.Queue()

        ports = [
            port.device
            for port in serial.tools.list_ports.comports()
        ]

        self.executor = concurrent.futures.ThreadPoolExecutor(
            max_workers=max(1, len(ports))
        )

        self.futures = [
            self.executor.submit(
                check_port,
                port,
                self.baudrate,
                self.timeout
            )
            for port in ports
        ]

        threading.Thread(
            target=self._collect_results,
            daemon=True
        ).start()

    def _collect_results(self):
        found_port = None

        for future in concurrent.futures.as_completed(self.futures):
            result = future.result()

            if result:
                found_port = result
                break

        self.result_queue.put(found_port)

        with self.lock:
            self.scanning = False

        self.executor.shutdown(wait=False, cancel_futures=True)

    def is_done(self):
        return not self.result_queue.empty()

    def get_result(self):
        try:
            return self.result_queue.get_nowait()
        except queue.Empty:
            return None

    def is_scanning(self):
        with self.lock:
            return self.scanning


# Example usage
if __name__ == "__main__":

    scanner = SerialScanner()

    scanner.start_scan()

    print("Scanning all serial ports simultaneously...")

    while True:

        if scanner.is_done():
            port = scanner.get_result()

            if port:
                print(f"Device found: {port}")
            else:
                print("No matching device found.")

            break

        # Your main application continues running here
        print("Main application running...")

        time.sleep(0.5)