import cv2


class GstH264UDPClient(object):

    def __init__(self, port=5000):
        gst_string = 'udpsrc port={} ! application/x-rtp, encoding-name=H264 ! '\
            'rtph264depay ! avdec_h264 ! videoconvert ! appsink'\
            .format(port)

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
        ret, frame = cap_rcv.read()

        if not ret:
            return None

        return frame


if __name__ == "__main__":

    import time

    cam1 = GstH264UDPClient(port=5000)
    cam2 = GstH264UDPClient(port=4000)

    current_cam = cam1

    print('Start!')

    while True:
        frame = current_cam.read_frame()
        if not frame:
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