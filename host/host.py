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
    ser.sendData("./platform host")
    time.sleep(3)

    for _ in range(1):
        #ser.sendData("|Host|imu_cv5|acc|gyro|tow||")
        #ser.sendData("|Host|mm|mmdata||")
        #ser.sendData("|Host|ublox|pvt||")
        ser.sendData("|Host|ms|msdata||")
        time.sleep(1)
    
    ser.sendData("killthread")
    time.sleep(1)
    ser.sendData("\x03")
    shared.stop_thread = True
    ser.stop