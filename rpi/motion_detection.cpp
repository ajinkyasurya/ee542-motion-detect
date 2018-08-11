#include "motion_detection.h"

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

int main(int argc, char** argv)
{
	cv::TermCriteria termcrit(cv::TermCriteria::COUNT|cv::TermCriteria::EPS,20,0.03);
	cv::Size subPixWinSize(10,10), winSize(31,31);
	const int MAX_COUNT = 500;
	
	cv::Mat raw_data, frame, prev_frame, image;
	std::vector<cv::Point2f> points[2];
	
	cv::VideoWriter writer;
	writer.open("test.avi", cv::VideoWriter::fourcc('X', 'V', 'I', 'D'), 5, cv::Size(320, 240), false);
	
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
			
	for (int j = 0; j < 200; j++)
	{
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
			std::vector<uchar> status;
			std::vector<float> err;
			cv::calcOpticalFlowPyrLK(prev_frame, frame, points[0], points[1],
										status, err, winSize, 3, termcrit, 0, 0.001);
			
			size_t i;
			for (i = 0; i < points[1].size(); i++)
			{
				//points[1][k++] = points[1][i];
				cv::circle(image, points[1][i], 3, cv::Scalar(0,255,0), -1, 8);
			}
			i = 0;
			//points[1].resize(k);
			
			cv::Mat diff_image;
			cv::absdiff(prev_frame, frame, diff_image);
			cv::threshold(diff_image, diff_image, 128, 255, cv::THRESH_BINARY);
			cv::Scalar diff_mean, diff_std;
			cv::meanStdDev(diff_image, diff_mean, diff_std);
			writer.write(image);
			cv::namedWindow("Video Display");
			cv::imshow("Video Display", image);
			std::cout << "Mean: " << diff_mean[0] << " Std: " << diff_std[0] << std::endl;
			cv::waitKey(1);
		}
		
		//cv::Mat big_frame(480, 640, CV_8UC1);
		//cv::resize(frame, big_frame, big_image.size(), 0, 0, cv::INTER_LINEAR);
		
		
		auto end = std::chrono::system_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
		std::cout << elapsed.count() << "ms" << std::endl;
		
		std::swap(points[1], points[0]);
		cv::swap(prev_frame, frame);
		//frame.copyTo(prev_frame);
	}
	
	writer.release();
	
	return 0;
}
