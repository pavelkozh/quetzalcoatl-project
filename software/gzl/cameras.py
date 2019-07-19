import cv2


class GstClient(object):
    def __init__(self, gst_string):
        self.gst_string = gst_string
        self.cap = None

    def __del__(self):
        self.disconnect()

    def disconnect(self):
        if self.cap is not None:
            self.cap.release()

        self.cap = None

    def connect(self):
        self.cap = cv2.VideoCapture(
            self.gst_string,
            cv2.CAP_GSTREAMER
        )

        self.cap.set(cv2.CAP_PROP_BUFFERSIZE, 1)

        if not self.cap.isOpened():
            raise Exception('VideoCapture not opened, check server')

    def read_frame(self):
        if self.cap is None:
            raise Exception('VideoCapture not opened, check connection')

        ret, frame = self.cap.read()

        if not ret:
            return None

        return frame


class GstH264UDPClient(GstClient):
    def __init__(self, port=5000, ip=None):
        gst_string = 'udpsrc port={} ! application/x-rtp, encoding-name=H264 ! '\
            'rtph264depay ! avdec_h264 ! videoconvert ! appsink'\
            .format(port)

        super().__init__(gst_string)


class GstH264TCPClient(GstClient):
    def __init__(self, ip='127.0.0.1', port=5000):
        gst_string = 'tcpclientsrc host={} port={} ! '\
            'h264parse ! avdec_h264 ! videoconvert ! '\
            'appsink drop=true sync=false'\
            .format(ip, port)

        super().__init__(gst_string)


class GstCamerasController(object):
    def __init__(self, cameras_list, logger=None):
        self.cameras_list = cameras_list
        self.cameras = []
        self.active_idx = 0
        self.active_cam = None
        self.logger = logger

        for camera_info in cameras_list:
            class_name = camera_info[0]
            class_params = camera_info[1]

            self.cameras += [class_name(**class_params)]

        if len(self.cameras) > 0:
            self.active_cam = self.cameras[self.active_idx]

            self._connect_active()

    def _connect_active(self):
        self.active_cam = self.cameras[self.active_idx]
        try:
            self.active_cam.connect()
        except:
            if self.logger:
                self.logger.error('Failed to connect to camera')
            else:
                print('Failed to connect to camera')
            self.active_cam = None
            return False

        return True

    def _disconnect_active(self):
        if self.active_cam is not None:
            self.active_cam.disconnect()

    def is_connected(self):
        return self.active_cam is not None

    def read_frame(self):
        if not self.is_connected():
            if not self._connect_active():
                return None

        return self.active_cam.read_frame()

    def switch_cam(self):
        self._disconnect_active()

        self.active_idx += 1
        if self.active_idx >= len(self.cameras):
            self.active_idx = 0

        self._connect_active()


if __name__ == "__main__":

    import time
    import numpy as np

    CAMERAS_SERVER_IP = '127.0.0.1'

    cam_params = [
        [GstH264TCPClient, {'ip': CAMERAS_SERVER_IP, 'port': 5000}],
        [GstH264TCPClient, {'ip': CAMERAS_SERVER_IP, 'port': 4000}],
    ]

    cams_controller = GstCamerasController(cam_params)

    print('Start!')

    prev_frame = np.zeros((600, 800, 3), np.uint8)

    while True:
        frame = cams_controller.read_frame()
        if frame is None:
            print('No frame from camera')

            cv2.imshow('result', prev_frame)
            inp = cv2.waitKey(300)

        else:
            prev_frame = frame

            cv2.imshow('result', frame)
            inp = cv2.waitKey(1)

        if inp & 0xFF == ord('q'):
            break
        elif inp & 0xFF == ord('w'):
            cams_controller.switch_cam()
