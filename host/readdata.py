import serial
import threading
import time

stop_thread = False

class ReadLine(threading.Thread):
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

if __name__ ==  '__main__':

    ser = serial.Serial(port='COM5', baudrate=115200, timeout=0)

    readingThread = ReadLine(ser)
    readingThread.start()

    # Login with the username
    ser.write(bytes(b'root\r\n'))
    time.sleep(0.3)
    ser.write(bytes(b'./HPS_FPGA_LED\r\n'))

	# Commands
    ser.write(bytes(b'3\r\n'))
    time.sleep(0.5)
    ser.write(bytes(b'4\r\n'))
    time.sleep(0.5)
    ser.write(bytes(b'5\r\n'))
    time.sleep(0.5)
    ser.write(bytes(b'6\r\n'))
    time.sleep(0.5)

    # Ctrl+Z to stop running the executable
    ser.write(b'\x03') 
    time.sleep(0.3)
    ser.write(bytes(b'exit\r\n'))
    time.sleep(0.3)
    
    stop_thread = True
    readingThread.join()

    ser.close()