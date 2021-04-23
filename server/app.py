from flask import Flask, render_template, redirect, request
from random import randint
import requests
import time

app = Flask(__name__)
data = "stop"
stopping = True
current = None

def reset():
    global data
    data = "stop"

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/submit', methods=["POST"])
def submit():
    global data, current, stopping
    values = request.form.get('fname')
    try:
        values = values.split(",")
        red = (int(values[0]) + randint(0, 255)) % 255
        green = (int(values[1]) + randint(0, 255)) % 255
        blue = (int(values[2]) + randint(0, 255)) % 255
        data = str(red) + "," + str(green) + "," + str(blue)
        current = time.time()
        stopping = False
        return redirect('/status')
    except:
        data = "stop"
        return redirect('/status')

@app.route('/colors')
def value():
    global data, current, stopping
    values = request.args.get('values')
    try:
        values2 = values.split("A")

        response = requests.get(
            "https://www.thecolorapi.com/scheme?rgb=" +
            str(values2[0]) + "," + str(values2[1]) + "," +
            str(values2[2]) + "&mode=analogic")

        if (response.status_code == 200):
            response.json()
            rgb = response.json()["colors"]
            choice = 4
            r = rgb[choice]["rgb"]["r"]
            g = rgb[choice]["rgb"]["g"]
            b = rgb[choice]["rgb"]["b"]
            data = str(r) + "," + str(g) + "," + str(b)
        current = time.time()
        stopping = False
        return values.replace("A", ",")
    except:
        data = "stop"
        return "stop"

@app.route('/status')
def status():
    global stopping
    if stopping:
        return "stop"
    elif int(time.time() - current) <= 15:
        return data
    else:
        stopping = True
        return "stop"

if __name__ == '__main__':
    app.run(debug=True)