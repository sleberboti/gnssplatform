import threading
import serial
import shared
from threading import Lock
import re
from database import DataBase

stop_thread = False
mutex = Lock()

class SerialWrapper:
    def __init__(self, device):
        self.ser = serial.Serial(port=device, baudrate=115200, timeout=0)
        self.readData()
        shared.stop_thread = False       

    def sendData(self, data):
        mutex.acquire()
        try:
            data += "\r\n"
            self.ser.write(data.encode())
        except:
            pass
        finally:
            mutex.release()
       
    
    def readData(self):
        self.readingThread = ReadLine(self.ser)
        self.readingThread.start()
    
    def stop(self):
        self.sendData("\x03")
        self.sendData("\x03")
        self.sendData("exit")
        self.readingThread.join()
        self.ser.close()

class ReadLine(SerialWrapper, threading.Thread):
    def __init__(self, s):
        threading.Thread.__init__(self)
        self.buf = bytearray()
        self.s = s
        self.stop_thread = stop_thread
        #self.string = str()

    def parser(self, bitmessage):
        data = []
        #print("parser running ")
        print(bitmessage)
        message = str(bitmessage)
        if message.find('DE10')!=-1:            
            print(message)
            sensorpos = message.find('name')+5
            commapos = message.find(',')
            sensorname = message[sensorpos:commapos]
            message = message[commapos+1:]

            if sensorname == 'imu_cv5':                 
                gyro = []
                acc = []
                accpos = message.find('acc:')
                message = message[accpos+4:]
                for _ in range(3):
                    commapos = message.find(',')
                    acc.append(float(message[:commapos]))
                    message = message[commapos+1:]
                gyropos = message.find('gyro:')
                message = message[gyropos+5:]
                for _ in range(3):
                    commapos = message.find(',')
                    gyro.append(float(message[:commapos]))
                    message = message[commapos+1:]            
                towpos = message.find('tow:')
                seppos = message.find("|")
                tow = float(message[towpos+4:seppos-1])

                #for _ in range(3):
                #    data.append([])

                data.append(sensorname)
                data.append(acc)
                data.append(gyro)
                data.append(tow)
            
            if sensorname == 'mm': 
                mm = []
                xpos = message.find('x:')                
                message = message[xpos+2:]
                commapos = message.find(',')
                mm.append(message[:commapos-1])

                ypos = message.find('y:') 
                message = message[ypos+2:]
                commapos = message.find(',')                
                mm.append(message[:commapos-1])

                zpos = message.find('z:') 
                message = message[zpos+2:]
                commapos = message.find('|') 
                mm.append(message[:commapos-1])

                data.append(sensorname)
                data.append(mm)
                print(" ")
            
            if sensorname == 'ublox_pvt':
                pvt = []
                numOfFields = 31
                for _ in range (numOfFields):
                    #print(message)
                    colonpos = message.find(':')
                    commapos = message.find(',')                    
                    pvt.append(message[colonpos+1:commapos-1])
                    message = message[commapos+1:]

                print(pvt)
                data.append(sensorname)
                data.append(pvt)

            if sensorname == 'ms':
                ms = []
                numOfFields = 2
                for _ in range (numOfFields):
                    colonpos = message.find(':')
                    commapos = message.find(',')                 
                    ms.append(message[colonpos+1:commapos])
                    message = message[commapos+1:]

                print(ms)
                data.append(sensorname)
                data.append(ms)
            
            if sensorname == 'imu_icm':
                imu_icm = []
                numOfFields = 10
                for _ in range (numOfFields):
                    colonpos = message.find(':')
                    commapos = message.find(',')                 
                    imu_icm.append(message[colonpos+1:commapos])
                    message = message[commapos+1:]

                print(imu_icm)
                data.append(sensorname)
                data.append(imu_icm)

            DataBase(data)
    
    def run(self):
        while True:            
            #print(self.readline())
            self.parser(self.readline())
            if shared.stop_thread: 
                break

    def readline(self):
        i = self.buf.find(b"\n")
        if i >= 0:
            r = self.buf[:i+1]
            self.buf = self.buf[i+1:]
            return r
        while True:
            i = max(1, min(2048, self.s.in_waiting))
            data = self.s.read(i)
            i = data.find(b"\n")
            if i >= 0:
                r = self.buf + data[:i+1]
                self.buf[0:] = data[i+1:]
                return r
            else:
                self.buf.extend(data)
            if shared.stop_thread:
                break