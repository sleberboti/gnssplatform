import signal
import sys
import shared
from serialwrapper import SerialWrapper
import threading
from threading import Lock
import time

mutex = Lock()

def keyboardInterruptHandler(signal, frame):
    print("KeyboardInterrupt has been caught. Cleaning up...")
    shared.stop_thread = True
    ser.stop
    exit(0)

signal.signal(signal.SIGINT, keyboardInterruptHandler)


if __name__ ==  '__main__':

    ser = SerialWrapper(shared.port)

    #ser.sendData("exit")
    ser.sendData("root")
    time.sleep(0.3)
    ser.sendData("./platform")
    time.sleep(3)
    ser.sendData("|Host|imu_cv5|acc|gyro|tow||")

    time.sleep(3)
    ser.sendData("\x03")
    shared.stop_thread = True
    ser.stop