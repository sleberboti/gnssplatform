import serial
import threading
import time
import signal
import sys

stop_thread = False

def keyboardInterruptHandler(signal, frame):
    print("KeyboardInterrupt has been caught. Cleaning up...")
    global stop_thread
    stop_thread = True
    ser.stop
    exit(0)

signal.signal(signal.SIGINT, keyboardInterruptHandler)

class SerialWrapper:
    def __init__(self):
        self.ser = serial.Serial(port='COM5', baudrate=115200, timeout=0)
        self.readData()
        self.stop_thread = False       

    def sendData(self, data):
        data += "\r\n"
        self.ser.write(data.encode())
    
    def readData(self):
        self.readingThread = ReadLine(self.ser)
        self.readingThread.start()
    
    def stop(self):
        self.readingThread.join()
        self.ser.close()

class ReadLine(SerialWrapper, threading.Thread):
    def __init__(self, s):
        threading.Thread.__init__(self)
        self.buf = bytearray()
        self.s = s
    
    def run(self):
        while True:
            print(self.readline())
            global stop_thread
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
    
    while True:
        ser.readData