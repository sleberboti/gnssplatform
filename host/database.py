from influxdb import InfluxDBClient
import json
import time

import threading
from threading import Lock

class DataBase:
    def __init__(self, data):
        #asddas
        self.client = InfluxDBClient(host='localhost', port=8086)
        self.client.switch_database('platform')
        self.data = data
        self.writeDB(self.data)
    
    def writeDB(self, data):
        json_body = [
            {
                "measurement": self.data[0],
                #"time": self.data[2], #tow
                "fields": {
                    "time": self.data[3], #tow
                    "acc[x]": self.data[1][0],  #acc[0]
                    "acc[y]": self.data[1][1],  #acc[1],
                    "acc[z]": self.data[1][2],  #acc[2],
                    "gyro[x]": self.data[2][0], #gyro[0],
                    "gyro[y]": self.data[2][1], #gyro[1],
                    "gyro[z]": self.data[2][2]  #gyro[2],
                }
            }
        ]
        self.client.write_points(json_body)