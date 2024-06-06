import serial
import time
import threading
from flask import Flask, render_template, jsonify

app = Flask(__name__)

# Path to save the SMS content
SMS_FILE_PATH = "/home/pi/Documents/SMS/data.txt"

# Serial port configuration
# Replace '/dev/ttyUSB0' with the serial port your GSM module is connected to
ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=0.5)
ser.flush()

def init_gsm_module():
    commands = ['AT', 'AT+CMGF=1', 'AT+CPIN="0000"']
    for cmd in commands:
        ser.write((cmd + '\r\n').encode())
        time.sleep(1)

def check_for_sms():
    while True:
        ser.write('AT+CMGL="REC UNREAD"\r\n'.encode())
        time.sleep(1)
        response = ser.readlines()
        print("rep:",response)
        message_started = False
        for line in response:
            decoded_line = line.decode().strip()
            if decoded_line.startswith('+CMGL:'):
                message_started = True
            elif message_started and decoded_line != "OK":
                with open(SMS_FILE_PATH, "w") as file:
                    file.write(decoded_line)
                break
        time.sleep(3)


def sms_alert_level():
    try:
        with open(SMS_FILE_PATH, 'r') as file:
            content = file.read().strip()
            return content
    except FileNotFoundError:
        return "0"

@app.route('/')
def index():
    return render_template('index.html', alert_level=sms_alert_level())

@app.route('/data')
def data():
    alert_level = sms_alert_level()
    bridge_status = {"0": "Ouvert", "1": "Ouvert", "2": "Sous Surveillance", "3": "Fermé"}.get(alert_level, "Inconnu")
    return jsonify(alert_level=alert_level, bridge_status=bridge_status)

if __name__ == '__main__':
    init_gsm_module()
    threading.Thread(target=check_for_sms, daemon=True).start()
    app.run(host='0.0.0.0', port=5000)
