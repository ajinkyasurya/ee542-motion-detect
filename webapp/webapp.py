from threading import Lock
from flask import Flask, render_template, session, request
from flask_socketio import SocketIO, emit, disconnect
import os
from random import random
import psutil

async_mode = None

app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(app, async_mode=async_mode)
thread = None
thread_lock = Lock()

# The websocket is maintained in the background, and this
# function outputs a random number every 5 seconds
def background_thread():
    """Example of how to send server generated events to clients."""
    count = 0
    while True:
        socketio.sleep(1)
        cpu_count = psutil.cpu_count()
        cpu_percent = psutil.cpu_percent()
        total_virtual_mem = psutil.virtual_memory().total
        available_virtual_mem = psutil.virtual_memory().available
        frame_dir = "/static/images/"
        frame_src = os.path.join(frame_dir, "image.jpg")
        count += 1
        number = round(random()*10, 3)
        socketio.emit('my_response',
                      {'data': number,
                      'count': count,
                      'cpu_count': cpu_count,
                      'cpu_percent': cpu_percent,
                      'total_virtual_mem': total_virtual_mem,
                      'available_virtual_mem': available_virtual_mem,
                      'frame_src': frame_src},
                      namespace='/test')

@app.route("/")
def index():
    num_cpus = psutil.cpu_count()
    total_virtual_mem = psutil.virtual_memory().total
    available_virtual_mem = psutil.virtual_memory().available
    return render_template('index.html',
        async_mode=socketio.async_mode,
        num_cpus=num_cpus,
        total_virtual_mem=total_virtual_mem,
        available_virtual_mem=available_virtual_mem)

@socketio.on('connect', namespace='/test')
def test_connect():
    global thread
    with thread_lock:
        if thread is None:
            thread = socketio.start_background_task(target=background_thread)
    emit('my_response', {'data': 'Connected', 'count': 0})

# Ping-pong allows Javascript in the web page to calculate the
# connection latency, averaging over time
@socketio.on('my_ping', namespace='/test')
def ping_pong():
    emit('my_pong')

# Notification that a client has disconnected
@socketio.on('disconnect', namespace='/test')
def test_disconnect():
    print('Client disconnected', request.sid)

# Run the web app
if __name__ == '__main__':
    socketio.run(app)
