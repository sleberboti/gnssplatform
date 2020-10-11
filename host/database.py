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
        try:
            if (self.data[0]=="imu_cv5"):
                json_body = [
                    {
                        "measurement": self.data[0],
                        #"time": self.data[2], #tow
                        "fields": {
                            "tow": self.data[3], #tow
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
        except Exception:
            pass
        
        try:
            if (self.data[0]=="imu_icm"):
                json_body = [
                    {
                        "measurement": self.data[0],
                        #"time": self.data[2], #tow
                        "fields": {
                            "tow":        self.data[1][10],
                            "accel_xout": self.data[1][0],
                            "accel_yout": self.data[1][1],
                            "accel_zout": self.data[1][2],
                            "gyro_xout":  self.data[1][3],
                            "gyro_yout":  self.data[1][4],
                            "gyro_zout":  self.data[1][5],
                            "mm_xout":    self.data[1][6],
                            "mm_yout":    self.data[1][7],
                            "mm_zout":    self.data[1][8],
                            "temp_out":   self.data[1][9]
                        }
                    }
                ]
                self.client.write_points(json_body)
        except Exception:
            pass
        
        try:
            if (self.data[0]=="mm"):
                json_body = [
                    {
                        "measurement": self.data[0],
                        #"time": self.data[2], #tow
                        "fields": {
                            "tow":   self.data[1][3],
                            "mm[x]": self.data[1][0],
                            "mm[y]": self.data[1][1], 
                            "mm[z]": self.data[1][2]
                        }
                    }
                ]
                self.client.write_points(json_body)
        except Exception:
            pass
        
        try:
            if (self.data[0]=="ms"):
                json_body = [
                    {
                        "measurement": self.data[0],
                        #"time": self.data[2], #tow
                        "fields": {
                            "tow":        self.data[1][2],
                            "temperature": self.data[1][0],
                            "pressure":    self.data[1][1]
                        }
                    }
                ]
                self.client.write_points(json_body)
        except Exception:
            pass

        try:
            if (self.data[0]=="ublox_sfrbx"):
                json_body = [
                    {
                        "measurement": self.data[0],
                        "fields": {
                            "gnssId":    self.data[1][0],
                            "svId":      self.data[1][1],
                            "freqId":    self.data[1][2],
                            "numWords":  self.data[1][3],
                            "chn":       self.data[1][4],
                            "word0":     self.data[1][5],
                            "word1":     self.data[1][6],
                            "word2":     self.data[1][7],
                            "word3":     self.data[1][8],
                            "word4":     self.data[1][9],
                            "word5":     self.data[1][10],
                            "word6":     self.data[1][11],
                            "word7":     self.data[1][12],
                            "word8":     self.data[1][13],
                            "word9":     self.data[1][14]
                        }
                    }
                ]
                self.client.write_points(json_body)
        except Exception:
            pass
        
        try:
            if (self.data[0]=="ublox_measx"):
                for i in range(1, int(self.data[1][8])+1):
                    json_body = [
                        {
                                "measurement":         self.data[0],
                                "fields": {
                                    "gpsTOW":          self.data[1][0],
                                    "gloTOW":          self.data[1][1],
                                    "bdsTOW":          self.data[1][2], 
                                    "gzssTOW":         self.data[1][3], 
                                    "gpsTOWacc":       self.data[1][4],  
                                    "gloTOWacc":       self.data[1][5], 
                                    "bdsTOWacc":       self.data[1][6], 
                                    "qzssTOWacc":      self.data[1][7], 
                                    "numSV":           self.data[1][8], 
                                    "flags":           self.data[1][9],
                                    "gnssId":          self.data[1][-1+0+i*11],
                                    "svId":            self.data[1][-1+1+i*11],
                                    "cNo":             self.data[1][-1+2+i*11],
                                    "mpathIndic":      self.data[1][-1+3+i*11],
                                    "dopplerMS":       self.data[1][-1+4+i*11],
                                    "dopplerHz":       self.data[1][-1+5+i*11],
                                    "wholeChips":      self.data[1][-1+6+i*11],
                                    "fracChips":       self.data[1][-1+7+i*11],
                                    "codePhase":       self.data[1][-1+8+i*11],
                                    "intCodePhase":    self.data[1][-1+9+i*11],
                                    "pseuRangeRMSErr": self.data[1][-1+10+i*11]
                            }
                        }
                    ]
                    self.client.write_points(json_body)
        except Exception:
            pass
        
        try:
            if (self.data[0]=="ublox_pvt"):
                json_body = [
                    {
                        "measurement": self.data[0],
                        #"time": self.data[2], #tow
                        "fields": {
                            "iTow":    self.data[1][0],
                            "year":    self.data[1][1],
                            "month":   self.data[1][2], 
                            "day":     self.data[1][3], 
                            "hour":    self.data[1][4],  
                            "min":     self.data[1][5], 
                            "sec":     self.data[1][6], 
                            "valid":   self.data[1][7], 
                            "tAcc":    self.data[1][8], 
                            "nano":    self.data[1][9], 
                            "fixType": self.data[1][10], 
                            "flags":   self.data[1][11], 
                            "flags2":  self.data[1][12], 
                            "numSV":   self.data[1][13], 
                            "lat":     self.data[1][14], 
                            "lon":     self.data[1][15], 
                            "height":  self.data[1][16], 
                            "hMSL":    self.data[1][17], 
                            "hAcc":    self.data[1][18], 
                            "vAcc":    self.data[1][19],
                            "velN":    self.data[1][20],
                            "velE":    self.data[1][21],
                            "velD":    self.data[1][22],
                            "gSpeed":  self.data[1][23],
                            "headMot": self.data[1][24],
                            "sAcc":    self.data[1][25],
                            "headAcc": self.data[1][26],
                            "pDOP":    self.data[1][27],
                            "headVeh": self.data[1][28], 
                            "magDec":  self.data[1][29],
                            "magAcc":  self.data[1][30]
                        }
                    }
                ]
                self.client.write_points(json_body)
        except Exception:
            pass