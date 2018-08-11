#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(){


Mat image;
image = imread("/home/pi/openCV_projects/Capture2.PNG", CV_LOAD_IMAGE_UNCHANGED); //read the file


if (!image.data){
		cout << "Could not load the image." << endl;
		return -1;
	}

namedWindow("Display Window", CV_WINDOW_NORMAL);
imshow("Display Window", image);

waitKey(0);

return 0;
}
