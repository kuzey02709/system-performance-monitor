import subprocess
from flask import Flask, render_template, Response

app = Flask(__name__)

def generate_telemetry():
    process = subprocess.Popen(
        ['main.exe'],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        bufsize=1
    )
    for line in iter(process.stdout.readline, ''):
        clean_line = line.strip()
        if clean_line.startswith('{') and clean_line.endswith('}'):
            yield f"data: {clean_line}\n\n"

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/stream')
def stream():
    return Response(generate_telemetry(), mimetype='text/event-stream')

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)