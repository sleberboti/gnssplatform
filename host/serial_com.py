import serial
import threading
import time
import signal
import sys

stop_thread = False

def keyboardInterruptHandler(signal, frame):
    print("KeyboardInterrupt has been caught. Cleaning up...".format(signal))
    global stop_thread
    stop_thread = True
    ser.stop
    exit(0)

signal.signal(signal.SIGINT, keyboardInterruptHandler)

class SerialWrapper:
    def __init__(self):
        #print("SerialWrapper init")
        self.ser = serial.Serial(port='COM5', baudrate=115200, timeout=0)
        self.readData()
        self.stop_thread = False       

    def sendData(self, data):
        data += "\r\n"
        self.ser.write(data.encode())
    
    def readData(self):
        #self.ser.write(data.encode())
        self.readingThread = ReadLine(self.ser)
        self.readingThread.start()
        #print("readdata started")
    
    def stop(self):
        #self.stop_thread = True
        #stop_thread = True
        self.readingThread.join()
        #print("After join()")
        self.ser.close()
        #print("After ser.close()")

class ReadLine(SerialWrapper, threading.Thread):
    def __init__(self, s):
        threading.Thread.__init__(self)
        self.buf = bytearray()
        self.s = s
        #print("readline started")
    
    def run(self):
        while True:
            #print("Reading1")
            print(self.readline())
            #print("Reading")
            global stop_thread
            #stop_thread = True
            if stop_thread: 
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
            global stop_thread
            if stop_thread:
                break

if __name__ ==  '__main__':
    #print("asd")
    ser = SerialWrapper()
    
    """ while 1:
        #misc code here
        ser.sendData(data) """
   
    """ while True:
        try:
            #stop_thread = True
            ser.readData
        except KeyboardInterrupt:
            stop_thread = True
            ser.stop """
    while True:
        ser.readData