import dash
from dash.dependencies import Output, Input
from dash import dcc, html, dcc
from datetime import datetime
import json
import plotly.graph_objs as go
from collections import deque
from flask import Flask, request

import socket
import struct

# udp stream
DST_IP = "127.0.0.1"
DST_PORT = 27027
CMD_PREFIX_ID = 155 
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

MIN_DELAY_BTW_PACKS_MS = 50 

MAX_DATA_POINTS = 1000
UPDATE_FREQ_MS = 850

server = Flask(__name__)
app = dash.Dash(__name__, server=server)

timestamps = deque(maxlen=MAX_DATA_POINTS)
time = deque(maxlen=MAX_DATA_POINTS)
accel_x = deque(maxlen=MAX_DATA_POINTS)
accel_y = deque(maxlen=MAX_DATA_POINTS)
accel_z = deque(maxlen=MAX_DATA_POINTS)


app.layout = html.Div(
	[
		dcc.Markdown(
			children="""
			# Live Sensor Readings
			Streamed from Sensor Logger: tszheichoi.com/sensorlogger
		"""
		),
		dcc.Graph(id="live_graph"),
		dcc.Interval(id="counter", interval=UPDATE_FREQ_MS),
	]
)


@app.callback(Output("live_graph", "figure"), Input("counter", "n_intervals"))
def update_graph(_counter):
	data = [
		go.Scatter(x=list(time), y=list(d), name=name)
		for d, name in zip([accel_x, accel_y, accel_z], ["X", "Y", "Z"])
	]

	graph = {
		"data": data,
		"layout": go.Layout(
			{
				"xaxis": {"type": "date"},
				"yaxis": {"title": "Orientation"}, #Acceleration ms<sup>-2</sup>
			}
		),
	}
	if (
		len(time) > 0
	):  #  cannot adjust plot ranges until there is at least one data point
		graph["layout"]["xaxis"]["range"] = [min(time), max(time)]
		graph["layout"]["yaxis"]["range"] = [
			min(accel_x + accel_y + accel_z),
			max(accel_x + accel_y + accel_z),
		]

	return graph


@server.route("/data", methods=["POST"])
def data():  # listens to the data streamed from the sensor logger
	if str(request.method) == "POST":
		# print(f'received data: {request.data}')
		data = json.loads(request.data)
		for d in data['payload']:
			if(d.get("name", None) == "orientation"): #gyroscopeuncalibrated, gyroscope
				timestamp = d["time"] / 1000000000 
				ts = datetime.fromtimestamp(timestamp)	
				
				if len(time) == 0 or timestamp - timestamps[-1] > 1:
					time.append(ts)
					timestamps.append(timestamp)
					# modify the following based on which sensor is accessed, log the raw json for guidance
					roll = d["values"]["roll"]
					pitch = d["values"]["pitch"]
					yaw = d["values"]["yaw"]

					accel_x.append(d["values"]["roll"])
					accel_y.append(d["values"]["pitch"])
					accel_z.append(d["values"]["yaw"])
					print("Orientation:")
					print("\tRoll =", roll)
					print("\tPitch =", pitch)
					print("\tYaw =", yaw)
					print("\n\n")
					orient_struct = struct.pack('=Bfff', CMD_PREFIX_ID, roll, pitch, yaw)
					print(orient_struct)
					sock.sendto(orient_struct, (DST_IP, DST_PORT))

	return "success"


if __name__ == "__main__":
	app.run_server(port=8000, host="0.0.0.0")
