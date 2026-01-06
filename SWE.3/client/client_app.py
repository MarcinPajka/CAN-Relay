import tkinter as tk
from tkinter import ttk
import socket
import json
import threading
import time

# Configuration
RELAY_IP = "192.168.1.100"  # Static IP of the relay
RELAY_PORT = 5000  # Port for communication

def send_json_to_relay(data):
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((RELAY_IP, RELAY_PORT))
        message = json.dumps(data)
        sock.sendall(message.encode('utf-8'))
        sock.close()
        status_label.config(text="Message sent successfully")
    except Exception as e:
        status_label.config(text=f"Error sending message: {str(e)}")

def send_battery_level():
    try:
        value = int(battery_slider.get())
        data = {"signal": "battery_level", "value": value}
        send_json_to_relay(data)
    except ValueError:
        status_label.config(text="Invalid battery level value")

def send_velocity():
    try:
        value = float(velocity_entry.get())
        data = {"signal": "velocity", "value": value}
        send_json_to_relay(data)
    except ValueError:
        status_label.config(text="Invalid velocity value")

def send_charging_active():
    value = charging_active_var.get()
    data = {"signal": "charging_active", "value": value}
    send_json_to_relay(data)

def send_charge_request():
    value = charge_request_var.get()
    data = {"signal": "charge_request", "value": value}
    send_json_to_relay(data)

def check_connection():
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(5)
        sock.connect((RELAY_IP, RELAY_PORT))
        sock.close()
        status_label.config(text="Connection successful")
    except Exception as e:
        status_label.config(text=f"Connection failed: {str(e)}")

def update_config():
    global RELAY_IP, RELAY_PORT
    try:
        RELAY_IP = ip_entry.get()
        RELAY_PORT = int(port_entry.get())
        status_label.config(text="Configuration updated")
    except ValueError:
        status_label.config(text="Invalid port number")

def send_all_signals():
    try:
        battery = int(battery_slider.get())
        velocity = float(velocity_entry.get())
        charging = charging_active_var.get()
        request = charge_request_var.get()
        signals = [
            {"signal": "battery_level", "value": battery},
            {"signal": "velocity", "value": velocity},
            {"signal": "charging_active", "value": charging},
            {"signal": "charge_request", "value": request}
        ]
        for data in signals:
            send_json_to_relay(data)
            time.sleep(0.1)  # small delay
        status_label.config(text="All signals sent")
    except Exception as e:
        status_label.config(text=f"Error sending all: {str(e)}")

def run_scenario(scenario):
    def scenario_thread():
        if scenario == "Driving Scenario":
            # Start driving
            send_json_to_relay({"signal": "velocity", "value": 50.0})
            time.sleep(5)
            # Battery drops
            for i in range(100, 20, -10):
                send_json_to_relay({"signal": "battery_level", "value": i})
                time.sleep(2)
            # Stop and charge
            send_json_to_relay({"signal": "velocity", "value": 0.0})
            send_json_to_relay({"signal": "charging_active", "value": True})
            time.sleep(10)
            # Battery charges
            for i in range(30, 101, 10):
                send_json_to_relay({"signal": "battery_level", "value": i})
                time.sleep(2)
            # Drive again
            send_json_to_relay({"signal": "charging_active", "value": False})
            send_json_to_relay({"signal": "velocity", "value": 30.0})
            status_label.config(text="Scenario completed")
    threading.Thread(target=scenario_thread).start()

def start_scenario():
    scenario = scenario_var.get()
    if scenario:
        status_label.config(text=f"Starting {scenario}")
        run_scenario(scenario)
root = tk.Tk()
root.title("CAN Relay Client")

# Configuration Frame
config_frame = ttk.LabelFrame(root, text="Configuration")
config_frame.pack(pady=10, padx=10, fill="x")

ttk.Label(config_frame, text="Relay IP:").grid(row=0, column=0, padx=5, pady=5)
ip_entry = ttk.Entry(config_frame)
ip_entry.insert(0, RELAY_IP)
ip_entry.grid(row=0, column=1, padx=5, pady=5)

ttk.Label(config_frame, text="Port:").grid(row=0, column=2, padx=5, pady=5)
port_entry = ttk.Entry(config_frame)
port_entry.insert(0, str(RELAY_PORT))
port_entry.grid(row=0, column=3, padx=5, pady=5)

update_config_button = ttk.Button(config_frame, text="Update Config", command=update_config)
update_config_button.grid(row=0, column=4, padx=5, pady=5)

check_connection_button = ttk.Button(config_frame, text="Check Connection", command=check_connection)
check_connection_button.grid(row=0, column=5, padx=5, pady=5)
battery_frame = ttk.LabelFrame(root, text="Battery Level")
battery_frame.pack(pady=10, padx=10, fill="x")

battery_slider = tk.Scale(battery_frame, from_=0, to=100, orient=tk.HORIZONTAL)
battery_slider.pack(side=tk.LEFT, padx=5)

send_battery_button = ttk.Button(battery_frame, text="Send Battery Level", command=send_battery_level)
send_battery_button.pack(side=tk.RIGHT, padx=5)

# Velocity
velocity_frame = ttk.LabelFrame(root, text="Velocity")
velocity_frame.pack(pady=10, padx=10, fill="x")

velocity_entry = ttk.Entry(velocity_frame)
velocity_entry.pack(side=tk.LEFT, padx=5, expand=True, fill="x")

send_velocity_button = ttk.Button(velocity_frame, text="Send Velocity", command=send_velocity)
send_velocity_button.pack(side=tk.RIGHT, padx=5)

# Charging Active
charging_active_frame = ttk.LabelFrame(root, text="Charging Active")
charging_active_frame.pack(pady=10, padx=10, fill="x")

charging_active_var = tk.BooleanVar()
charging_active_check = ttk.Checkbutton(charging_active_frame, text="Charging Active", variable=charging_active_var)
charging_active_check.pack(side=tk.LEFT, padx=5)

send_charging_active_button = ttk.Button(charging_active_frame, text="Send Charging Active", command=send_charging_active)
send_charging_active_button.pack(side=tk.RIGHT, padx=5)

# Charge Request
charge_request_frame = ttk.LabelFrame(root, text="Charge Request")
charge_request_frame.pack(pady=10, padx=10, fill="x")

charge_request_var = tk.BooleanVar()
charge_request_check = ttk.Checkbutton(charge_request_frame, text="Charge Request", variable=charge_request_var)
charge_request_check.pack(side=tk.LEFT, padx=5)

send_charge_request_button = ttk.Button(charge_request_frame, text="Send Charge Request", command=send_charge_request)
send_charge_request_button.pack(side=tk.RIGHT, padx=5)

# Send All Frame
send_all_frame = ttk.Frame(root)
send_all_frame.pack(pady=10, padx=10, fill="x")

send_all_button = ttk.Button(send_all_frame, text="Send All Signals", command=send_all_signals)
send_all_button.pack()

# Scenario Frame
scenario_frame = ttk.LabelFrame(root, text="Scenarios")
scenario_frame.pack(pady=10, padx=10, fill="x")

scenario_var = tk.StringVar()
scenario_combo = ttk.Combobox(scenario_frame, textvariable=scenario_var, values=["Driving Scenario"])
scenario_combo.pack(side=tk.LEFT, padx=5)

start_scenario_button = ttk.Button(scenario_frame, text="Start Scenario", command=start_scenario)
start_scenario_button.pack(side=tk.RIGHT, padx=5)

# Status Label
status_label = ttk.Label(root, text="")
status_label.pack(pady=10)

# Run the application
root.mainloop()
