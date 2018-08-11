#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <wiringPiSPI.h>
#include <unistd.h>

#include <ctype.h>
#include <stdio.h>
#include <iostream>
#include <chrono>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include "arducam.h"

#define OV2640_CHIPID_HIGH 0x0A
#define OV2640_CHIPID_LOW 0x0B

#define BUF_SIZE (384*1024)

uint8_t buffer[BUF_SIZE] = {0xFF};
