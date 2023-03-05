import cv2
import numpy as np

class Image:
    def __init__(self, filename=None, img=None):
        if filename:
            self.img = cv2.imread(filename)
            if self.img is None:
                raise FailedToLoadImgErr()
        elif img is not None:
            self.img = img
        else:
            self.img = None

    def show(self, filename="Image"):
        cv2.namedWindow(filename, cv2.WINDOW_NORMAL)
        cv2.imshow(filename, self.img)
        print("* Press 0 on the Image Window to continue\n")
        cv2.waitKey(0)
        cv2.destroyWindow(filename)

    def save(self, filename):
        cv2.imwrite(filename, self.img)

    @staticmethod
    def destroy_all_windows():
        cv2.destroyAllWindows()

    def fit_to_size(self, other):
        self.img = cv2.resize(self.img, other.img.shape[1::-1])

    def alpha_blend(self, other, other_weight):
        ret = Image()
        ret.img = cv2.addWeighted(self.img, (1 - other_weight), other.img, other_weight, 0.0)
        return ret

    def edge_detect(self, lower_threshold, upper_threshold):
        ret = Image()
        ret.img = cv2.Canny(self.img, lower_threshold, upper_threshold)
        return ret

    def gaussian_blur(self, kernel_sz):
        assert kernel_sz % 2 == 1, "Kernel size must be an odd number"
        assert kernel_sz > 1, "Kernel size must be greater than 1"
        assert kernel_sz < 1000, "Kernel size must be less than 1000"

        ret = Image()
        ret.img = cv2.GaussianBlur(self.img, (kernel_sz, kernel_sz), 0)
        return ret

    def collect_points(self, window_name="_tmp_collect"):
        ret = []

        def add_point_cb(event, x, y, flags, data):
            if event == cv2.EVENT_LBUTTONDOWN:
                data.append(x, y)
                print("Selected point: ({},{})".format(x, y))

        cv2.namedWindow(window_name, cv2.WINDOW_NORMAL)
        cv2.setMouseCallback(window_name, add_point_cb, ret)
        cv2.imshow(window_name, self.img)
        return ret

    def create_homography(self, points):
        assert len(points) == 4, "Exactly 4 points must be given"

        dst_points = np.array([
            (0, 0),
            (self.img.shape[1], 0),
            (self.img.shape[1], self.img.shape[0]),
            (0, self.img.shape[0])
        ])

        tmp = cv2.findHomography(points, dst_points)
        ret = np.zeros_like(self.img)
        cv2.warpPerspective(self.img, ret, tmp, (self.img.shape[1], self.img.shape[0]))
        return ret

    def get_mask(self, points):
        # Create the mask
        sz = len(points)
        # Use a list comprehension to convert the points to a list of (x, y) tuples
        pnts = [(p.x, p.y) for p in points]
        # Create a black image of the same size as the input image
        mask = np.zeros((self.img.shape[0], self.img.shape[1], 3), dtype=np.uint8)
        # Use the fillConvexPoly() function to fill the mask with white using the selected points
        cv2.fillConvexPoly(mask, pnts, (255, 255, 255))

        # Combine the mask and the input image
        ret = cv2.bitwise_and(mask, self.img)

        # Return the masked image
        return Image(ret)

    def get_object_features(self, img):
        pts = self.collect_points()

        # Create a mask using the selected points
        mask = self.get_mask(pts)

        # Calculate the center of mass
        M = cv2.moments(mask)
        cx = int(M['m10'] / M['m00'])
        cy = int(M['m01'] / M['m00'])
        print(f"Centre of mass: ({cx}, {cy})")

        # Calculate the area of the object
        area = cv2.contourArea(mask)
        print(f"Area: {area}")

        return mask



