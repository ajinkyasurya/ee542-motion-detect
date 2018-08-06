#CC = g++
#CFLAGS = -g -Wall
#SRCS = motion_detection.cpp
#PROG = motion_detection
#
#OPENCV = `pkg-config opencv --cflags --libs`
#LIBS = $(OPENCV)
#
#$(PROG):$(SRC)
#	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)
CXX = g++ -std=c++11
CFLAGS = -lwiringPi -Wall
CXXFLAGS = `pkg-config --libs --cflags opencv`
all : motion_detection

objects = arducam.o arducam_arch_raspberrypi.o

motion_detection: $(objects) motion_detection.o
	g++ -o motion_detection $(objects) motion_detection.o $(CXXFLAGS) -lwiringPi -Wall

arducam.o: arducam.c
	g++ -c arducam.c -lwiringPi -Wall
arducam_arch_raspberrypi.o: arducam_arch_raspberrypi.c
	g++ -c arducam_arch_raspberrypi.c -lwiringPi -Wall

motion_detection.o: motion_detection.cpp
	$(CXX) -c motion_detection.cpp $(CFLAGS)

clean:
	rm -f motion_detection $(objects) *.o
