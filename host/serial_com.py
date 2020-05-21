import signal
import sys
import shared
from serialwrapper import SerialWrapper

def keyboardInterruptHandler(signal, frame):
    print("KeyboardInterrupt has been caught. Cleaning up...")
    shared.stop_thread = True
    ser.stop
    exit(0)

signal.signal(signal.SIGINT, keyboardInterruptHandler)

if __name__ ==  '__main__':

    ser = SerialWrapper(shared.port)
    
    while True:
        ser.readData