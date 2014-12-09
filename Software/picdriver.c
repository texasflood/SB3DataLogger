#define M_PI 3.14159265358979323846264338327 
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>

#include <stdio.h>
#include <windows.h> 
#include <rs232.h>
#include <math.h> 
#include "picdriver.h"
#include"pictest.h"

#define READ_VERSION    0
#define MSG    8

//This file contains low level functions to send and receive data over the RS232 serial link
//You will need to modify some of these functions for your application

//Define transmit and receive buffer
BYTE rx_buff[RX_BUFF_LEN];
BYTE tx_buff[4];
static int com_port_no;
static int com_port_open = 0;
const double fc = 15000;
const double fs = 44037;
double globalDistortionValue = 1;
double K;
//RS232 functions

//Open a COM port
void RS232_Init(int port_no)
{
	//Open COM port given by port_no using parameters:
	// COM port, device name, baud rate, parity, data bits, stop bits, input queue size, outptu queue size
	OpenComConfig (port_no, "", 115200, 0, 8, 1, 1024, 16); 
	
	//remember COM port number and state
	com_port_no = port_no;
	com_port_open = 1;
}

//Close the open COM port
void RS232_Close(void)
{
	if (com_port_open == 1)
	{
		CloseCom(com_port_no);
	}
}


//Read in a block of data from the COM port's input buffer
BYTE* Read_Data_Block(void)
{
	int n;
	int i;
	
	//Wait for enough data (4 bytes here)
	while(GetInQLen(4) < RX_BUFF_LEN);
	
	//Perform read of the COM port buffer, placing results in rx_buff
	n = ComRd(4, rx_buff, RX_BUFF_LEN - 1);

	//return pointer to rx_buff
	return rx_buff;
}

//Send n-byte command
void send_command(int n)
{
	if(com_port_open == 1)
	{
		ComWrt(com_port_no, tx_buff, n);	
	}
}

//-------------------------------

//User comms functions

int read_analog_input (int *value)
{
	BYTE buffer[3];
	int timeout = 100000;
	
	if (com_port_open == 0)
	{
		MessagePopup ("Error", "No Open COM Port");
		return 0;
	}

	//flush RS232 input buffer
	FlushInQ(com_port_no);
	
	//send ADC read command
	tx_buff[0] = 1;
	send_command(1);
	
	//wait for returned ADC value
	while((GetInQLen(com_port_no) < 3) && (timeout > 0))
	{
		timeout--;
	}
	
	//If we time out something's wrong
	if (timeout == 0)
	{
		MessagePopup ("Error", "No received Data");
		exit(-1);
	}
	
	//read 3 bytes from input buffer
	ComRd(com_port_no, buffer, 3);
	
	//assign received value
	*value = buffer[1] * 256 + buffer[2];
	
  	return no_error;
}

//Command to set Distortion to on or off state
int switch_distortion (int value)
{
	if (com_port_open == 0)
	{
		MessagePopup ("Error", "No Open COM Port");
		return 0;
	}
	
	tx_buff[0] = 2;	//Command
	tx_buff[1] = value; 	//value (on or off)
	
	send_command(2);
	
	
  return no_error;
}

//Command to set Distortion value
int set_distortion (int value)
{
	globalDistortionValue = (double) value;
	if (com_port_open == 0)
	{
		MessagePopup ("Error", "No Open COM Port");
		return 0;
	}
	
	tx_buff[0] = 3;	//Command
	tx_buff[1] = value; 	//value
	
	send_command(2);
	
	
  return no_error;
}

int get_vol(unsigned char *volValue)
{
	BYTE buffer[2];
	
	if (com_port_open == 0)
	{
		MessagePopup ("Error", "No Open COM Port");
		return 0;
	}
	
	if (GetInQLen(com_port_no) >= 2)
	{
		ComRd(com_port_no, buffer, 2);
	}
	
	*volValue = buffer[1];
	
	return no_error;
}

int set_volume (int value)
{
	if (com_port_open == 0)
	{
		MessagePopup ("Error", "No Open COM Port");
		return 0;
	}
	value = 11 - value;
	tx_buff[0] = 4;	//Command
	tx_buff[1] = value; 	//value
	
	send_command(2);
	
	
  return no_error;
}

/*int set_offset (int offset)
{
	BYTE sign;
	BYTE firstPart;
	BYTE secondPart;
	
	if (offset > 0)
	{
		sign = 0;
	}
	else
	{
		sign = 1;
	}
	
	offset = abs(offset);
	
	if (offset > floor(512 - 1023/(2*globalDistortionValue)))
	{
		offset = (int) floor(512 - 1023/(2*globalDistortionValue));
	}
	
	offset = abs(offset);
	
	if (offset >= 2)
	{
		offset = offset - 2;
	}
	else
	{
		offset = 0;
	}
	
	firstPart = (BYTE) offset;
	secondPart = (BYTE) (offset >> 8);
	
	tx_buff[0] = 6;
	tx_buff[1] = sign;
	tx_buff[2] = firstPart;
	tx_buff[3] = secondPart;
	
	send_command(3);
	
	return no_error;
}*/

int set_treble (double value)
{
	/*double b0f, b1f, b2f, a1f, a2f, V0;
	int b0i, b1i, b2i, a1i, a2i;
	if (com_port_open == 0)
	{
		MessagePopup ("Error", "No Open COM Port");
		return 0;
	}
	
	V0 = pow(10,(value/20));
	K = tan((M_PI*fc)/fs);
	if (value >= 0)
	{
		b0f = (V0 + sqrt(2*V0)*K + pow(K,2))/(1 + sqrt(2)*K + pow(K,2));
	}
	
	
	
	
	
	tx_buff[0] = 5;	//Command
	tx_buff[1] = b0i; 	//value
	tx_buff[2] = b1i;
	tx_buff[3] = b2i;
	tx_buff[4] = a1i;
	tx_buff[5] = a2i;
	send_command(2); */
	
	
  return no_error;
}

