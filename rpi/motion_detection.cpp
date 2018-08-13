#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <wiringPi.h>
#include "PiMotor.h"
#include "tof.h"
#include "motion_detection.h"

#define M1_FWD 	25 //Motor 1 Forward
#define M1_BCK  24 //Motor 1 Reverse
#define M1_PWM  16 //Motor 1 pwm

#define M2_FWD  28 //Motor 2 Forward
#define M2_BCK 	27 //Motor 2 Reverse
#define M2_PWM  29 //Motor 2 pwm 
#define BLE 23
#define MINIMUM_SPEED 20
#define MAXIMUM_SPEED 100
using namespace std;

//Create a new instance for the motors
PiMotor motor1(M1_FWD, M1_BCK, M1_PWM);
PiMotor motor2(M2_FWD, M2_BCK, M2_PWM);
int distReading;
int direction;
int speed = MINIMUM_SPEED;

//bluetooth variables
struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
char buf[1024] = { 0 };
int s, client, bytes_read;
socklen_t opt = sizeof(rem_addr);


int ble();
void setDistaceSensor();
int getDistance();
void controlMotors();

void setup()
{
    uint8_t vid,pid;
    uint8_t temp;
    wiring_init();
    arducam(smOV2640,CAM1_CS,-1,-1,-1);
    
     // Check if the ArduCAM SPI bus is OK
    arducam_write_reg(ARDUCHIP_TEST1, 0x55, CAM1_CS);
    temp = arducam_read_reg(ARDUCHIP_TEST1, CAM1_CS);
    //printf("temp=%x\n",temp);
    if(temp != 0x55) {
        printf("SPI interface error!\n");
        exit(EXIT_FAILURE);
    }
     else{
    	   printf("SPI interface OK!\n");
    	}
    
    // Change MCU mode
    arducam_write_reg(ARDUCHIP_MODE, 0x00, CAM1_CS);
    
    // Check if the camera module type is OV2640
    arducam_i2c_read(OV2640_CHIPID_HIGH, &vid);
    arducam_i2c_read(OV2640_CHIPID_LOW, &pid);
    if((vid != 0x26) || (pid != 0x42)) {
        printf("Can't find OV2640 module!\n");
        exit(EXIT_FAILURE);
    } else {
        printf("OV2640 detected\n");
    }
}

int main(int argc, char *argv[]) {

	wiringPiSetup();
	piHiPri(99);
	string input;
	while (true) {
		int bleInd = ble();
		pinMode(BLE, OUTPUT);
		digitalWrite(BLE, LOW);
		if (bleInd > -1) {
			digitalWrite(BLE, HIGH);
		}
		setDistaceSensor();
		controlMotors();
		cout << "Conenct again? y/n\n" << endl;
		cin >> input;
		if (input == "n" || input == "N") {
			break;
		}
	}
	//close connection
	close(client);
	close(s);
	digitalWrite(BLE, LOW);
	return 0;
}


int ble() {
	// allocate socket
	s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);	
	// bind socket to port 1 of the first available 
	// local bluetooth adapter
	loc_addr.rc_family = AF_BLUETOOTH;
	bdaddr_t my_bdaddr_any = { 0 };
	loc_addr.rc_bdaddr = my_bdaddr_any;
	loc_addr.rc_channel = (uint8_t)1;
	//while (bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr)) < 0);
	bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));

	// put socket into listening mode
	listen(s, 1);

	// accept one connection
	
	client = accept(s, (struct sockaddr *)&rem_addr, &opt);

	ba2str(&rem_addr.rc_bdaddr, buf);
	fprintf(stderr, "Accepted connection from %s\n", buf);
	memset(buf, 0, sizeof(buf));
	return 0;
}

void setDistaceSensor() {
	int i;
	int model, revision;
	// For Raspberry Pi's, the I2C channel is usually 1
	i = tofInit(1, 0x29, 1); // set long range mode (up to 2m)
	if (i != 1)
	{
		printf("VL53L0X device not opened.\n");
		return; // -1; // problem - quit
	}
	printf("VL53L0X device successfully opened.\n");
	i = tofGetModel(&model, &revision);
	printf("Model ID - %d\n", model);
	printf("Revision ID - %d\n", revision);
}

int getDistance() {
	int dist, threshold = 99999;

	dist = tofReadDistance();
	// valid range? 
	if (dist < 4096) {
		return dist;
	}
	else {
		return threshold;
	}
}

void controlMotors() {
	//fcntl(s, F_SETFL, O_NONBLOCKING);
	
	cv::TermCriteria termcrit(cv::TermCriteria::COUNT|cv::TermCriteria::EPS,20,0.03);
	cv::Size subPixWinSize(10,10), winSize(31,31);
	const int MAX_COUNT = 500;
	
	cv::Mat raw_data, frame, prev_frame, image;
	std::vector<cv::Point2f> points[2];
	
	//cv::VideoWriter writer;
	//writer.open("test.avi", cv::VideoWriter::fourcc('X', 'V', 'I', 'D'), 5, cv::Size(320, 240), false);
	
	setup();
	arducam_set_format(fmtJPEG);
	arducam_init();
	arducam_set_jpeg_size(sz320x240);
	sleep(1);
		
	arducam_flush_fifo(CAM1_CS);
	arducam_clear_fifo_flag(CAM1_CS);
	arducam_start_capture(CAM1_CS);
	while (!(arducam_read_reg(ARDUCHIP_TRIG, CAM1_CS) & CAP_DONE_MASK));
	
	//printf("Reading FIFO\n");    
	size_t len = read_fifo_length(CAM1_CS);
	size_t image_size = len;
	//std::cout << image_size << std::endl;
	if (len >= 393216){
		printf("Over size.");
		exit(EXIT_FAILURE);
	} else if (len == 0 ){
		printf("Size is 0.");
		exit(EXIT_FAILURE);
	} 
	digitalWrite(CAM1_CS,LOW);  //Set CS low       
	set_fifo_burst(BURST_FIFO_READ);
	arducam_spi_transfers(buffer,1);//dummy read  
	int32_t i=0;
	while(len>4096)
	{	 
		arducam_transfers(&buffer[i],4096);
		len -= 4096;
		i += 4096;
	}
	arducam_spi_transfers(&buffer[i],len); 

	digitalWrite(CAM1_CS,HIGH);  //Set CS HIGH
	
	raw_data = cv::Mat(1, image_size, CV_8UC1, (void*) buffer);
	frame = cv::imdecode(raw_data, 0);
	
	cv::goodFeaturesToTrack(frame, points[1], MAX_COUNT, 0.01, 10, cv::Mat(), 3, 0, 0.04);
	cv::cornerSubPix(frame, points[1], subPixWinSize, cv::Size(-1,-1), termcrit);

	while (true) {
		//captureImage();
		//extraceFeatures();
		//detectMotion();
		
		auto start = std::chrono::system_clock::now();
		// Flush the FIFO
		arducam_flush_fifo(CAM1_CS);    
		// Clear the capture done flag
		arducam_clear_fifo_flag(CAM1_CS);
		// Start capture
		//printf("Start capture\n");  
		arducam_start_capture(CAM1_CS);
		while (!(arducam_read_reg(ARDUCHIP_TRIG, CAM1_CS) & CAP_DONE_MASK)) ;
		//printf("CAM1 Capture Done\n");
		
		//printf("Reading FIFO\n");    
		size_t len = read_fifo_length(CAM1_CS);
		size_t image_size = len;
		//std::cout << image_size << std::endl;
		if (len >= 393216){
			printf("Over size.");
			exit(EXIT_FAILURE);
		} else if (len == 0 ){
			printf("Size is 0.");
			exit(EXIT_FAILURE);
		} 
		digitalWrite(CAM1_CS,LOW);  //Set CS low       
		set_fifo_burst(BURST_FIFO_READ);
		arducam_spi_transfers(buffer,1);//dummy read  
		int32_t i=0;
		while(len>4096)
		{	 
			arducam_transfers(&buffer[i],4096);
			len -= 4096;
			i += 4096;
		}
		arducam_spi_transfers(&buffer[i],len); 

		digitalWrite(CAM1_CS,HIGH);  //Set CS HIGH
		
		raw_data = cv::Mat(1, image_size, CV_8UC1, (void*) buffer);
		frame = cv::imdecode(raw_data, 0);
		frame.copyTo(image);
		
		if (!points[0].empty())
		{
			cv::Mat diff_image;
			cv::absdiff(prev_frame, frame, diff_image);
			cv::threshold(diff_image, diff_image, 128, 255, cv::THRESH_BINARY);
			cv::Scalar diff_mean, diff_std;
			cv::meanStdDev(diff_image, diff_mean, diff_std);
			
			if (diff_mean[0] > 10) {
				std::cout << "Object entered/leaving scene. Recomputing features..." << std::endl;
				cv::goodFeaturesToTrack(frame, points[0], MAX_COUNT, 0.01, 10, cv::Mat(), 3, 0, 0.04);
				cv::cornerSubPix(frame, points[0], subPixWinSize, cv::Size(-1,-1), termcrit);
			}
			
			std::vector<uchar> status;
			std::vector<float> err;
			cv::calcOpticalFlowPyrLK(prev_frame, frame, points[0], points[1],
										status, err, winSize, 3, termcrit, cv::OPTFLOW_LK_GET_MIN_EIGENVALS, 0.001);
			
			std::vector<float> magnitude, direction;
			cv::Mat points_diff = cv::Mat(points[1]) - cv::Mat(points[0]);

			for (size_t i = 0; i < points_diff.size().height; i++)
			{
				cv::Point2f point = points_diff.at<cv::Point2f>(i);
				magnitude.push_back(sqrt(pow(point.x, 2) + pow(point.y, 2)));
				direction.push_back(atan2(point.x, point.y));
				cv::circle(image, points[1][i], 3, cv::Scalar(0,255,0), -1, 8);
			}
			
			cv::Scalar magnitude_mean, magnitude_std, direction_mean, direction_std;
			cv::meanStdDev(cv::Mat(magnitude), magnitude_mean, magnitude_std);
			cv::meanStdDev(cv::Mat(direction), direction_mean, direction_std);
			
			if (direction_std[0] < 0.5) {
				std::cout << "Vehicle moving" << std::endl;
			}
			if (magnitude_mean[0] > 1) {
				std::cout << "Movement detected" << std::endl;
			}
			
			//std::cout << points_diff << std::endl << cv::Mat(magnitude) << std::endl << cv::Mat(direction) << std::endl;
			//std::cout << magnitude_mean[0] << " " << magnitude_std[0] << " " << direction_mean[0] << " " << direction_std[0] << std::endl;
			
			//writer.write(image);
			cv::namedWindow("Video Display");
			cv::imshow("Video Display", image);
			//std::cout << "Mean: " << diff_mean[0] << " Std: " << diff_std[0] << std::endl;
			cv::waitKey(1);
		}
		
		std::swap(points[1], points[0]);
		cv::swap(prev_frame, frame);
		
		//read data from the client
		//bytes_read = read(client, buf, sizeof(buf));
		bytes_read = recv(client, buf, sizeof(buf), MSG_DONTWAIT);
		int command = ((int) buf[0]) - 48;
		distReading = getDistance();

		if (bytes_read > 0) {
			printf("Received [%d]\n", command);
		}
		if (command == 1) {
			printf("Start\n");
			printf("Distance: %dmm", distReading);
			//Start the motor
			motor1.start();
			motor2.start();
		}
		if (command == 8) {
			direction = 1;
			printf("Going forward\n");
			printf("Distance: %dmm\n", distReading);
			//Set PWM value for direction (0 = reverse, 1 = forwards)
			motor1.run(direction, speed);
			motor2.run(direction, speed);
		}
		if (command == 2) {
			direction = 0;
			printf("Going reverse\n");
			printf("Distance: %dmm\n", distReading);
			//Set PWM value for direction (0 = reverse, 1 = forwards)
			motor1.run(direction, speed);
			motor2.run(direction, speed);
		}
		if (command == 6) {
			direction = 1;
			printf("Going right\n");
			printf("Distance: %dmm\n", distReading);
			motor1.run(direction, speed);
			printf("Speed:%d \n", speed);			
			motor2.stop();
		}
		if (command == 4) {
			direction = 1;
			printf("Going left\n");
			printf("Distance: %d mm\n", distReading);
			motor2.run(direction, speed);
			motor1.stop();
		}
		if (command == 0) {
			printf("Stop\n");
			//Stop the motor  
			motor1.stop();
			motor2.stop();
		}
		if (command == 7) {
			printf("Increasing speed\n");
			speed = min(speed + 5, MAXIMUM_SPEED);
			 printf("Speed:%d \n", speed);
			motor1.run(direction, speed);
			motor2.run(direction, speed);
		}
		if (command == 9) {
			printf("Decreasing speed\n");
			speed = max(speed - 5, MINIMUM_SPEED);
			printf("Speed:%d \n", speed);
			motor1.run(direction, speed);
			motor2.run(direction, speed);
		}
		if (command == 3) {
			printf("Exit\n");
			close(client);
			close(s);
			digitalWrite(BLE, LOW);
			break;
		}
		
		auto end = std::chrono::system_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
		std::cout << elapsed.count() << "ms" << std::endl;
	}

}

