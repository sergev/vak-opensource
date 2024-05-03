#include <iostream>
#include <opencv2/opencv.hpp>

void show(const cv::Mat &image, const std::string &title)
{
    // Display the image.
    cv::imshow(title, image);

    // Wait for any key pressed.
    for (;;) {
        char key = cv::waitKey(10);
        if (key > 0)
            break;
    }
}

int main()
{
    // Read the file.
    cv::Mat image;
    image = cv::imread("../funny.jpg", cv::IMREAD_UNCHANGED);
    if (image.empty()) {
        std::cout << "Could not open the image!" << std::endl;
        return -1;
    }
    show(image, "Original");

    // Resize.
    const int new_width = 240;
    const int new_height = 274;
    cv::Mat resized;
    cv::resize(image, resized, cv::Size(new_width, new_height));
    show(resized, "Resized");

    // Crop.
    const int x = (image.cols - new_width) / 2;
    const int y = (image.rows - new_height) / 2;
    cv::Mat cropped = image(cv::Rect(x, y, new_width, new_height));
    show(cropped, "Cropped");

    // Rotate.
    const double angle = 45.0;
    cv::Point2f center(image.cols / 2.0, image.rows / 2.0);
    cv::Mat rotation_matrix = cv::getRotationMatrix2D(center, angle, 1.0);
    cv::Mat rotated;
    cv::warpAffine(image, rotated, rotation_matrix, image.size());
    show(rotated, "Rotated");

    cv::destroyAllWindows();
    return 0;
}
