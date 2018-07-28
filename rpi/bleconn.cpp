#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <wiringPi.h>
#include <string>

#define M1_FWD 	4 //Motor 1 Forward
#define M1_BCK  5 //Motor 1 Reverse
#define M2_FWD  1 //Motor 2 Forward
#define M2_BCK 	0 //Motor 2 Reverse
using namespace std;

int main(int argc, char **argv)
{
	struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
	char buf[1024] = { 0 };
	int s, client, bytes_read;
	socklen_t opt = sizeof(rem_addr);

	// allocate socket
	s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

	// bind socket to port 1 of the first available 
	// local bluetooth adapter
	loc_addr.rc_family = AF_BLUETOOTH;
	bdaddr_t my_bdaddr_any = { 0 };
	loc_addr.rc_bdaddr = my_bdaddr_any;
	loc_addr.rc_channel = (uint8_t)1;
	bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));

	// put socket into listening mode
	listen(s, 1);

	// accept one connection
	client = accept(s, (struct sockaddr *)&rem_addr, &opt);
	if (client < 0){
			perror("error in accept");
			return 0;
	}

	ba2str(&rem_addr.rc_bdaddr, buf);
	fprintf(stderr, "Accepted connection from %s\n", buf);
	memset(buf, 0, sizeof(buf));
	
	//Initiate wiring Pi
	wiringPiSetup();
	piHiPri(99);
	pinMode(M1_FWD, OUTPUT);
    pinMode(M1_BCK, OUTPUT);
    pinMode(M2_FWD, OUTPUT);
    pinMode(M2_BCK, OUTPUT);

	while (true){
		// read data from the client
		bytes_read = read(client, buf, sizeof(buf));
		if (bytes_read > 0) {
			printf("Received [%s]\n", buf);
		}
		
		if ((string)buf == "8"){
			printf("Going forward\n");
			digitalWrite(M1_FWD, HIGH);
			digitalWrite(M1_BCK, LOW);	
			digitalWrite(M2_FWD, HIGH);
			digitalWrite(M2_BCK, LOW);
			
		}else if ((string)buf == "2"){
			printf("Going reverse\n");
			digitalWrite(M1_FWD, LOW);
			digitalWrite(M1_BCK, HIGH);
			digitalWrite(M2_FWD, LOW);
			digitalWrite(M2_BCK, HIGH);
			
		}else if ((string)buf == "6"){
			printf("Going Right\n");
			digitalWrite(M1_FWD, HIGH);
			digitalWrite(M1_BCK, LOW);
			digitalWrite(M2_FWD, LOW);
			digitalWrite(M2_BCK, LOW);
			
		} else if ((string)buf == "4"){
			printf("Going Left\n");
			digitalWrite(M1_FWD, LOW);
			digitalWrite(M1_BCK, LOW);
			digitalWrite(M2_FWD, HIGH);
			digitalWrite(M2_BCK, LOW);
	
		} else if ((string)buf == "0"){
			printf("Stop\n");
			//Stop the motor  
			digitalWrite(M1_FWD, LOW);
			digitalWrite(M1_BCK, LOW);
			digitalWrite(M2_FWD, LOW);
			digitalWrite(M2_BCK, LOW);
		}else if ((string)buf == "1"){
			printf("Starting\n");
			//Stop the motor  
			digitalWrite(M1_FWD, HIGH);
			digitalWrite(M1_BCK, LOW);
			digitalWrite(M2_FWD, HIGH);
			digitalWrite(M2_BCK, LOW);
			delay (2000);
			digitalWrite(M1_FWD, LOW);
			digitalWrite(M1_BCK,HIGH);
			digitalWrite(M2_FWD, LOW);
			digitalWrite(M2_BCK, HIGH);
			delay (2000);
			digitalWrite(M1_FWD, LOW);
			digitalWrite(M1_BCK,LOW);
			digitalWrite(M2_FWD, LOW);
			digitalWrite(M2_BCK, LOW);
		
		} else if ((string)buf == "e") {
			digitalWrite(M1_FWD, LOW);
			digitalWrite(M1_BCK,LOW);
			digitalWrite(M2_FWD, LOW);
			digitalWrite(M2_BCK, LOW);
			printf("Exit\n");
			break;
		}


	}
	
	// close connection
	close(client);
	close(s);
	return 0;
}
