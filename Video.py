import cv2

class Video:

    # underlying data
    capture = cv2.VideoCapture()
    cap_width = capture.get(cv2.CAP_PROP_FRAME_WIDTH)
    cap_height = capture.get(cv2.CAP_PROP_FRAME_HEIGHT)

    def __init__(self, filename=None, cap=None):
        self.capture = cv2.VideoCapture(filename) if filename else cap

        if not self.capture.isOpened():
            raise FailedToLoadImgErr

    def edge_detect(self, lower_threshold, upper_threshold):
        # Create a new video file with the specified dimensions and frame rate
        output = cv2.VideoWriter("columbia_video.mp4", cv2.VideoWriter_fourcc('M', 'J', 'P', 'G'), 30,
                                 (self.cap_width, self.cap_height))

        # Read and process frames from the input video
        frame = None
        print("Saving Edge Detection Video...")
        while self.capture.read(frame):
            ret = cv2.Canny(frame, lower_threshold, upper_threshold)
            cv2.imshow("Edge Detection", ret)
            if cv2.waitKey(1) != -1:
                self.capture.release()
                print("finished by user")
                break
            output.write(ret)

        # Release the video file
        output.release()

        # Return the output video
        return Video("videos/edge_detection_video.avi")

