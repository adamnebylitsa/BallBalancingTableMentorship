from flask import Flask, render_template, request, jsonify, Response
import socket
import threading

app = Flask(__name__,static_folder= 'static')

HOST = "0.0.0.0"
ARDUINO_PORT = 6000

arduino_connected = False

arduino_thread = None
arduino_socket = None

message=""

@app.route('/')
def index():
    return render_template('index.html')

@app.route("/connected",methods=['GET','POST'])
def connected():
    if request.method == "POST":
        start_arduino_thread()
    if request.method == "GET":
        x=request.args['x']
        y=request.args['y']
        sendData(x+" "+y)
    return render_template('connected.html')

def remote_arduino_handler():
    global arduino_socket, arduino_connected, message
    print("test")
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.bind((HOST, ARDUINO_PORT))
        server_socket.listen()

        print(f"Waiting for Arduino Client to connect on {HOST}:{ARDUINO_PORT}")
        arduino_socket, addr2 = server_socket.accept()
        arduino_connected = True
        print('arduino connected')
        while True:
            full_data = ''
            while True:
                data = arduino_socket.recv(1024)
                if not data:
                    break

                full_data += data.decode('utf-8')
                if full_data.endswith('\n'):
                    break
            
            full_data = full_data[:-1]
            message=full_data
            print(full_data)


def start_arduino_thread():
    global arduino_thread
    print('starting connection to arduino')
    arduino_thread = threading.Thread(target=remote_arduino_handler)
    arduino_thread.daemon = True
    arduino_thread.start()

def sendData(data):
    if 'arduino_socket' in globals():
        try:
            arduino_socket.sendall(data.encode('utf-8'))
        except Exception as e:
            print(f'Error sending data to Arduino: {e}')
    return


if __name__ == '__main__':
    app.run( host=HOST, port= 5000, debug= True)