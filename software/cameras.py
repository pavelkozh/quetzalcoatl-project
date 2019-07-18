import cv2


class GstClient(object):
    def __init__(self, gst_string):
        self.cap = cv2.VideoCapture(
            gst_string,
            cv2.CAP_GSTREAMER
        )

        self.cap.set(cv2.CAP_PROP_BUFFERSIZE, 1)

        if not self.cap.isOpened():
            raise Exception('VideoCapture not opened, check server')

    def __del__(self):
        self.cap.release()

    def read_frame(self):
        ret, frame = self.cap.read()

        if not ret:
            return None

        return frame



class GstH264UDPClient(GstClient):
    def __init__(self, port=5000):

        gst_string = 'udpsrc port={} ! application/x-rtp, encoding-name=H264 ! '\
            'rtph264depay ! avdec_h264 ! videoconvert ! appsink'\
            .format(port)

        super().__init__(gst_string)


class GstH264TCPClient(GstClient):
    def __init__(self, ip='127.0.0.1', port=5000):
        gst_string = 'tcpclientsrc host={} port={} ! '\
            'h264parse ! avdec_h264 ! videoconvert ! appsink drop=true sync=false'\
            .format(ip, port)

        super().__init__(gst_string)


if __name__ == "__main__":

    import time

    SERVER_IP='127.0.0.1'

    cam1 = GstH264TCPClient(ip=SERVER_IP, port=5000)
    cam2 = GstH264TCPClient(ip=SERVER_IP, port=4000)

    current_cam = cam1

    print('Start!')

    time.sleep(10)
    exit(1)

    while True:
        frame = current_cam.read_frame()
        if frame is None:
            time.sleep(0.001)
            continue

        cv2.imshow('send', frame)
        inp = cv2.waitKey(1)
        if inp & 0xFF == ord('q'):
            break
        elif inp & 0xFF == ord('1'):
            current_cam = cam1
        elif inp & 0xFF == ord('2'):
            current_cam = cam2
