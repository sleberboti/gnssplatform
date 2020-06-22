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
        if (self.data[0]=="imu_cv5"):
            json_body = [
                {
                    "measurement": self.data[0],
                    #"time": self.data[2], #tow
                    "fields": {
                        "internal_time": self.data[3], #tow
                        "acc[x]": self.data[1][0],  #acc[0]
                        "acc[y]": self.data[1][1],  #acc[1],
                        "acc[z]": self.data[1][2],  #acc[2],
                        "gyro[x]": self.data[2][0], #gyro[0],
                        "gyro[y]": self.data[2][1], #gyro[1],
                        "gyro[z]": self.data[2][2]  #gyro[2],
                    }
                }
            ]
        if (self.data[0]=="mm"):
            json_body = [
                {
                    "measurement": self.data[0],
                    #"time": self.data[2], #tow
                    "fields": {
                        #"time": , #tow
                        "mm[x]": self.data[1][0],
                        "mm[y]": self.data[1][1], 
                        "mm[z]": self.data[1][2]
                    }
                }
            ]
        if (self.data[0]=="ms"):
            json_body = [
                {
                    "measurement": self.data[0],
                    #"time": self.data[2], #tow
                    "fields": {
                        "temperature": self.data[1][0],
                        "pressure":    self.data[1][1]
                    }
                }
            ]
        if (self.data[0]=="ublox_pvt"):
            json_body = [
                {
                    "measurement": self.data[0],
                    #"time": self.data[2], #tow
                    "fields": {
                        #"time": , #tow
                        "iTow": self.data[1][0],
                        "year": self.data[1][1],
                        "month": self.data[1][2], 
                        "day": self.data[1][3], 
                        "hour": self.data[1][4],  
                        "min": self.data[1][5], 
                        "sec": self.data[1][6], 
                        "valid": self.data[1][7], 
                        "tAcc": self.data[1][8], 
                        "nano": self.data[1][9], 
                        "fixType": self.data[1][10], 
                        "flags": self.data[1][11], 
                        "flags2": self.data[1][12], 
                        "numSV": self.data[1][13], 
                        "lat": self.data[1][14], 
                        "lon": self.data[1][15], 
                        "height": self.data[1][16], 
                        "hMSL": self.data[1][17], 
                        "hAcc": self.data[1][18], 
                        "vAcc": self.data[1][19],
                        "velN": self.data[1][20],
                        "velE": self.data[1][21],
                        "velD": self.data[1][22],
                        "gSpeed": self.data[1][23],
                        "headMot": self.data[1][24],
                        "sAcc": self.data[1][25],
                        "headAcc": self.data[1][26],
                        "pDOP": self.data[1][27],
                        "headVeh": self.data[1][28], 
                        "magDec": self.data[1][29],
                        "magAcc": self.data[1][30]
                    }
                }
            ]
        self.client.write_points(json_body)