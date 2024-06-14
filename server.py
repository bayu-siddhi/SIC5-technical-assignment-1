import os
import pytz
import datetime
import pandas as pd
from flask import Flask
from flask import request
from flask import jsonify
from dotenv import load_dotenv

app = Flask(__name__)

load_dotenv()
df = pd.read_csv(os.getenv('CSV_DATA'))
timezone = pytz.timezone(os.getenv('TIMEZONE'))
date_format = os.getenv('DATE_FORMAT')

@app.route('/sensor/data', methods=['POST'])
def sensor_data():
  # Get the data from the request
  data = request.get_json()
  temperature = data['temperature']
  humidity = data['humidity']
  millis = data['millis']

  # Change millis to local datetime
  timestamp = datetime.datetime.fromtimestamp(millis, tz=timezone)
  timestamp = timestamp.strftime(date_format)[:-3]

  # Add and save the sensor data
  df.loc[len(df)] = [temperature, humidity, timestamp]
  df.to_csv('tmp_data.csv', index=False)

  # Send response
  response = {'message': 'Data received successfully!'}
  return jsonify(response)

@app.route('/sensor/data', methods=['GET'])
def get_sensor_data():
  # Get the sensor data from df
  json_data = df.to_dict(orient='records')

  # Return the sensor data
  return jsonify(json_data)

if __name__ == '__main__':
  app.run(debug=True)
