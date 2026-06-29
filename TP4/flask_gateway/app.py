import eventlet
eventlet.monkey_patch()

from flask import Flask, render_template
from flask_socketio import SocketIO
from mqtt_receiver import start_mqtt_thread  # importa hilo MQTT

app = Flask(__name__)
app.config['SECRET_KEY'] = 'secreto'
socketio = SocketIO(app, cors_allowed_origins="*")

@app.route('/')
def index():
    return render_template("home.html")

@app.route('/panel')
def panel():
    return render_template("panel.html")

@app.route("/historial")
def historial():
    import sqlite3
    conn = sqlite3.connect("sensores.db")
    cursor = conn.cursor()
    cursor.execute("SELECT * FROM lecturas ORDER BY timestamp DESC LIMIT 100")
    rows = cursor.fetchall()
    conn.close()
    return render_template("historial.html", datos=rows)

@socketio.on('connect')
def on_connect():
    print("Cliente web conectado")

if __name__ == "__main__":
    start_mqtt_thread(socketio)
    print("Servidor accesible en el siguiente puerto: http://localhost:5001")
    socketio.run(app, host="0.0.0.0", port=5001)
