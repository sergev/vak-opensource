#include <iostream>
#include <opencv2/opencv.hpp>

int main()
{
    // Read the file.
    cv::Mat image;
    image = cv::imread("../funny.jpg", cv::IMREAD_UNCHANGED);

    // Check if the image is loaded successfully.
    if (image.empty()) {
        std::cout << "Could not open the image!" << std::endl;
        return -1;
    }

    // Display the image.
    cv::imshow("My image", image);

    // Wait for any key pressed.
    for (;;) {
        char key = cv::waitKey(10);
        if (key > 0)
            break;
    }

    cv::destroyAllWindows();
    return 0;
}
