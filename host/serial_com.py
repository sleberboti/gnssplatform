import serial
import threading
import time
import signal
import sys
import shared
from serialwrapper import SerialWrapper

shared.stop_thread = False

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