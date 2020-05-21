import threading
import serial
import shared

stop_thread = False

class SerialWrapper:
    def __init__(self, device):
        self.ser = serial.Serial(port=device, baudrate=115200, timeout=0)
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
        self.stop_thread = stop_thread
    
    def run(self):
        while True:
            print(self.readline())
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