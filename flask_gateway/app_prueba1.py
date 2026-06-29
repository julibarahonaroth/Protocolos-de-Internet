import eventlet
eventlet.monkey_patch()

from flask import Flask, render_template_string
from flask_socketio import SocketIO

app = Flask(__name__)
# Esta línea es necesaria durante el desarrollo
app.config['SECRET_KEY'] = 'secreto'
socketio = SocketIO(app, cors_allowed_origins="*")

html = '''
<!DOCTYPE html>
<html>
<head>
<script src="https://cdn.socket.io/3.1.3/socket.io.min.js"></script>
</head>
<body>
<h2>Test Socket.IO</h2>
<ul id="messages"></ul>
<script>
  var socket = io();
  socket.on('connect', () => console.log('Connected!'));
  socket.on('message', data => {
    console.log('Received:', data);
    var li = document.createElement('li');
    li.textContent = data;
    document.getElementById('messages').appendChild(li);
  });
</script>
</body>
</html>
'''

@app.route('/')
def index():
    return render_template_string(html)

def send_messages():
    import time
    while True:
        with app.app_context():
            socketio.emit('message', 'Mensaje desde el servidor!')
        socketio.sleep(3)

@socketio.on('connect')
def on_connect():
    print('Cliente conectado')

if __name__ == '__main__':
    socketio.start_background_task(send_messages)
    socketio.run(app, host='0.0.0.0', port=5001)
