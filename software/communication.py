import serial
import numpy as np
import time
import sys


class CommunicationOnSerial(object):
    """Класс для работы с USB.

    Перед началом работы необходимо разрешить передачу данных.

    Перед началом чтения отладочной строки необходимопередать разрешение на отладку.
    """

    def __init__(self, num_port, transmission_speed=115200):

        if sys.version_info < (3, 0):
            raise ImportError(
                "Sorry, script requires Python 3.x, not Python 2.x")

        self.ser = serial.Serial(
            num_port,
            transmission_speed,
            timeout=5,
            writeTimeout=0
        )
        self.ser_buffer = ""
        self.work_resolution = False

        self.start_byte_ctl = '#'
        self.start_byte_cmd = '&'

    def activate_connection(self):
        """Разрешение передачи данных."""

        self.work_resolution = True

    # Команда остановки
    def deactivate_connection(self):
        """Остановка соединения."""

        pkg = bytes([ord(self.start_byte_cmd), 67, 89, 23])
        self.__push_msg(pkg)

        self.work_resolution = False

    # Команда пауза
    def stop_connection(self):
        """Пауза соединения."""

        pkg = bytes([ord(self.start_byte_cmd), 34, 63, 129])
        self.__push_msg(pkg)

        self.work_resolution = False

    def on_start(self):
        """Включить зажигание."""

        pkg = bytes([ord(self.start_byte_cmd), 25, 45, 65])
        self.__push_msg(pkg)

        # self.work_resolution = False

    def on_stop(self):
        """Остановка."""

        pkg = bytes([ord(self.start_byte_cmd), 13, 26, 39])
        self.__push_msg(pkg)

        # self.work_resolution = False

    def set_control(self, speed_gaz, steer_gaz):
        """Передача значения скорости и угла поворота."""

        if self.work_resolution is True:
            pkg = bytes([ord(self.start_byte_ctl),
                         np.uint8(speed_gaz),
                         np.uint8(steer_gaz),
                         np.uint8(speed_gaz + steer_gaz * 2)
                         ])

            self.__push_msg(pkg)
        else:
            print('Disabled connection! Activate it first!')
            # print(self.work_resolution)

    def enable_debugging(self):
        """Передача разрешения для отладки."""

        if self.work_resolution is not False:

            pkg = bytes([ord(self.start_byte_cmd), 38, 79, 123])
            self.__push_msg(pkg)

    def disable_debugging(self):
        """Передача запрета для отладки."""

        if self.work_resolution is not False:

            pkg = bytes([ord(self.start_byte_cmd), 31, 39, 115])
            self.__push_msg(pkg)

    def __push_msg(self, pkg):
        # print('Send package: {} / hex: {}'.format(list(pkg), pkg))
        self.ser.write(pkg)

    def get_state_msg(self):
        """Получения отладочной информации."""

        data_2_read = self.ser.inWaiting()
        if data_2_read > 0:
            self.ser_buffer += self.ser.read(data_2_read).decode('ascii')

        nl_idx = self.ser_buffer.find('\n')
        result = None

        if nl_idx >= 0:
            result = StateMessage(self.ser_buffer[:nl_idx])
            self.ser_buffer = self.ser_buffer[nl_idx+1:]

        return result


class StateMessage(object):
    """ Класс для определения приоритета сообщения.

    Значения каждого типа сообщения:

    "Information" - информационное сообщение.

    "Warning" - предупреждающее сообщение.

    "Error" - сообщение об ошибки.
    """

    UNKNOWN_LVL = 0
    INFO_LVL = 1
    WARNING_LVL = 2
    ERROR_LVL = 3

    _INFO_LVL_STR = "INF: "
    _WARNING_LVL_STR = "WARN: "
    _ERROR_LVL_STR = "ERR: "

    def __init__(self, raw_msg):
        NOT_FOUND = -1
        print(raw_msg)
        if self._INFO_LVL_STR in raw_msg:
            self.lvl, self.msg = self.INFO_LVL, raw_msg[len(
                self._INFO_LVL_STR):]

        elif self._WARNING_LVL_STR in raw_msg:
            self.lvl, self.msg = self.WARNING_LVL, raw_msg[len(
                self._WARNING_LVL_STR):]

        elif self._ERROR_LVL_STR in raw_msg:
            self.lvl, self.msg = self.ERROR_LVL, raw_msg[len(
                self._ERROR_LVL_STR):]

        else:
            self.lvl, self.msg = self.UNKNOWN_LVL, raw_msg


# Для теста предлагается в скрипте выделить часть "main" и в ней
# провести инициализацию и читать отладочные строки с периодической передачей значений скорости и поворота.
#
# Для теста необходимо передать имя устройства в консоле при запуске файла на подобие: "python communication.py /dev/ttyACM1".
#
# Для тестирования модуля определения типа сообщения, сообщение из дебага отправляется в метод данного класса
# на основе возращаемой константы класса выводится определенное сообщение.


if __name__ == "__main__":

    import argparse

    # Производится чтение аргумента из консоли.
    parser = argparse.ArgumentParser(description='Communication script test')
    parser.add_argument('device', action='store', help='Serial port device')

    args = parser.parse_args()

    # Создание объекта и передача полученного аргумента из консоли.
    Connection = CommunicationOnSerial(args.device)

    print('Set connection activated')
    Connection.activate_connection()
    print('Set debug enabled')
    Connection.enable_debugging()

    print('Start main loop')

    check_time = time.time()

    while(1):

        if time.time() - check_time > 1:
            check_time = time.time()

            # speed, angle = input("Put speed, angle ").split()
            # Connection.set_control(speed, angle)

            spst_pair = (np.random.randint(-100, 100),
                         np.random.randint(-100, 100))
            print('New speed/steer pair: {}'.format(spst_pair))

            Connection.set_control(spst_pair[0], spst_pair[1])

            print('Send start/stop')
            Connection.on_start()
            Connection.on_stop()

        # speed, angle = input('Print speed and angle: ').split()
        # Connection.set_control(int(speed), int(angle))
        inp = Connection.get_state_msg()
        if inp:
            print('I get: {} ({})'.format(inp.msg, inp.lvl))

            if inp.lvl != StateMessage.UNKNOWN_LVL:
                print("Logger")

            if inp.lvl != StateMessage.INFO_LVL and \
                    inp.lvl != StateMessage.UNKNOWN_LVL:
                print("State Pub")
