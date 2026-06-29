import eventlet
eventlet.monkey_patch()

from flask import Flask, render_template_string
from flask_socketio import SocketIO
import paho.mqtt.client as mqtt
import json
import threading

app = Flask(__name__)
# Esta línea es necesaria durante el desarrollo
app.config['SECRET_KEY'] = 'secreto'
socketio = SocketIO(app, cors_allowed_origins="*")

# HTML embebido para prueba directa
HTML = '''
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <script src="https://cdn.socket.io/3.1.3/socket.io.min.js"></script>
</head>
<body>
<h2>Datos desde MQTT</h2>
<ul id="mensajes"></ul>
<script>
    const socket = io({ transports: ['websocket'] });

    socket.on('connect', () => {
        console.log('Conectado!');
    });

    socket.on('nueva_lectura', function(data) {
        console.log('Recibido:', data);
        const li = document.createElement('li');
        li.textContent = `Sensor: ${data.sensor} - Valor: ${data.value}`;
        document.getElementById('mensajes').prepend(li);
    });

    socket.on('disconnect', () => {
        console.warn("Desconectado del WebSocket");
    });

    socket.on('connect_error', err => {
        console.error('Error de conexión:', err.message);
    });
</script>
</body>
</html>
'''

@app.route('/')
def index():
    return render_template_string(HTML)

@socketio.on('connect')
def on_connect():
    print('Cliente web conectado al WebSocket')

def on_mqtt_message(client, userdata, msg):
    print("MQTT recibido:", msg.payload)
    try:
        data = json.loads(msg.payload.decode())
        print("Emitiendo a WebSocket:", data)
        socketio.emit('nueva_lectura', data)
    except Exception as e:
        print("Error al parsear MQTT:", e)

def mqtt_thread():
    mqttc = mqtt.Client()
    mqttc.on_message = on_mqtt_message
    mqttc.connect("localhost", 1883, 60)
    mqttc.subscribe("sensors/temp")
    print("MQTT conectado y suscripto a sensors/temp")
    mqttc.loop_forever()

if __name__ == '__main__':
    threading.Thread(target=mqtt_thread, daemon=True).start()
    socketio.run(app, host='0.0.0.0', port=5001)
