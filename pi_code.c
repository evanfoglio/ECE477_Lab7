#include <wiringSerial.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
int init(void);
int main()
{
	//serial descriptor
	int fd;
	char buffer[42];
	FILE *fd2 = NULL;
	FILE *serial_in;
	int fdserial = init();	
	int average[32];
	bool average_recording = false;
	int sum;
	int i, j;
	
	//open serial port
	fd = serialOpen("/dev/serial0", 9600);	
	
	//serial port error checking
	if(fd < 0){
		printf("Could not open serial port 0\n");
		return -1;
	}

	//Open the file using the standard library
	serial_in = fdopen(fdserial, "r");
	
	usleep(2000000);// let serial sort itself out
	
	//clear all awaiting data
	serialFlush(fd);
	


	//Read 42 characters from the serial
	while(fgets(buffer, 42, serial_in)){
		
		//print recieved data
		printf("value is %s \n", buffer);

		//Reset if it hits 32, start averaging	
		if(i == 32){
			i = 0;
			average_recording = true;
		}
		
		//Set average to buffer value
		average[i] = atoi(buffer);
		
		//If there are enough values to take the average, take the average and print the values
		if(average_recording){
			sum = 0;
			for(j = 0; j<32; j++){
				sum += average[j];
			}
			printf("average is %d\n sum is %d\n", sum/32, sum);
		}

	}
	//Close the file descriptor
	close(fd);
}



int init()
{
	int fd1;
	struct termios tc; // terminal control structure

	//todo serial port should not be hard coded
	fd1 = open("/dev/serial0", O_RDWR | O_NOCTTY);
	if (fd1 < 1) {
		printf("Failed to open serial port\n");
		return 0;
	}
	tcgetattr(fd1, &tc);
	tc.c_iflag = IGNPAR;
	tc.c_oflag = 0;
	tc.c_cflag = CS8 | CREAD | CLOCAL; //8 bit chars enable receiver no modem status lines
	tc.c_lflag = ICANON;

	//todo baud rate should not be hard coded
	cfsetispeed(&tc, B9600);
	cfsetospeed(&tc, B9600);
	//todo should have bits per character set
	tcsetattr(fd1, TCSANOW, &tc);
	return fd1;
}
