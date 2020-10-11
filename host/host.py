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


    # MEASUREMENT VALUES
    duration = 1000
    frequency = 5

    ser = SerialWrapper(shared.port)

    #ser.sendData("exit")
    ser.sendData("root")
    time.sleep(2)
    #ser.sendData("./platform host")
    ser.sendData("./platform host_init")
    time.sleep(5)

    # MEASUREMENT COMMANDS
    ser.sendData("|Host|freq|"+str(frequency)+"|")  # Hz
    ser.sendData("|Host|dur|"+str(duration)+"|")    # in seconds
    ser.sendData("|Host|imu_cv5|acc|gyro|tow||")
    ser.sendData("|Host|mm|mmdata||")
    ser.sendData("|Host|ublox|pvt|measx|sfrbx||")
    ser.sendData("|Host|ms|msdata|")
    ser.sendData("|Host|imu_icm|acc|gyro|mm|temp||")


    # used for host dicatated measurements
    """ for _ in range(200):
        ser.sendData("|Host|imu_cv5|acc|gyro|tow||")
        ser.sendData("|Host|mm|mmdata||")
        ser.sendData("|Host|ublox|pvt|measx|sfrbx||")
        ser.sendData("|Host|ms|msdata|")
        ser.sendData("|Host|imu_icm|acc|gyro|mm|temp||")
        time.sleep(0.5) """
    
    time.sleep(duration+10)
    ser.sendData("killthread")
    time.sleep(1)
    ser.sendData("\x03")
    shared.stop_thread = True
    ser.stop