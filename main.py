# gui.py
import customtkinter as ctk
from ser import list_serial_ports, find_arduino_port, open_serial

ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("blue")

class SerialGUI(ctk.CTk):
    def __init__(self):
        super().__init__()
        self.title("Retirement Countdown Controller")
        self.geometry("420x260")
        self.resizable(False, False)

        self.serial_conn = None
        self.create_widgets()
        self.auto_detect()

    def create_widgets(self):
        ctk.CTkLabel(self, text="Arduino Connection", font=("Arial", 20)).pack(pady=10)

        # COM port dropdown
        self.port_var = ctk.StringVar()
        self.port_menu = ctk.CTkOptionMenu(
            self,
            values=self.get_port_names(),
            variable=self.port_var
        )
        self.port_menu.pack(pady=8)

        # Baud rate dropdown
        self.baud_var = ctk.StringVar(value="9600")
        self.baud_menu = ctk.CTkOptionMenu(
            self,
            values=["9600", "19200", "38400", "57600", "115200"],
            variable=self.baud_var
        )
        self.baud_menu.pack(pady=8)

        # Connect button
        self.connect_button = ctk.CTkButton(
            self,
            text="Connect",
            command=self.connect_serial
        )
        self.connect_button.pack(pady=10)

        # Status label
        self.status_label = ctk.CTkLabel(self, text="Not connected", text_color="orange")
        self.status_label.pack(pady=5)

    def get_port_names(self):
        ports = list_serial_ports()
        return [p.device for p in ports] or ["No ports found"]

    def auto_detect(self):
        port = find_arduino_port()
        if port:
            self.port_var.set(port)
            self.status_label.configure(text=f"Auto-detected Arduino on {port}", text_color="lightgreen")

    def connect_serial(self):
        port = self.port_var.get()
        baud = int(self.baud_var.get())
        self.serial_conn = open_serial(port, baud)
        if self.serial_conn:
            self.status_label.configure(text=f"Connected to {port} @ {baud}", text_color="lightgreen")
        else:
            self.status_label.configure(text="Connection failed", text_color="red")


if __name__ == "__main__":
    app = SerialGUI()
    app.mainloop()
