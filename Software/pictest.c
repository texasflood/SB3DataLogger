#include <rs232.h>
#include <cvirte.h>		
#include <userint.h>
#include "pictest.h"
#include "picdriver.h"  
#define true 1
#define false 0
void CallbackData(void){}
int com_port_no;  
//This file contains high level functions to respond to user inputs from the GUI
//You will need to develop a GUI and corresponding callback functions for your own application

static int panelHandle;


//Main - displays the GUI
int main (int argc, char *argv[])
{
	//Check for errors
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "pictest.uir", PANEL)) < 0)
		return -1;
	
	//Run GUI
	DisplayPanel (panelHandle);
	
	RunUserInterface ();

	DiscardPanel (panelHandle);

	
	return 0;
}

int CVICALLBACK COM_button_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	
	
	if (event!=EVENT_COMMIT) return 0;
	
	//Get user input COM port number
	GetCtrlVal(panel, PANEL_USER_COM_PORT, &com_port_no);
	
	//Open specified COM port
	RS232_Init(com_port_no);
	InstallComCallback(com_port_no, LWRS_RECEIVE, 2, 10, ComCallback, CallbackData);
	set_distortion(1);
	switch_distortion(0);
	
	return 0;
}

int CVICALLBACK quit_button_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
    if (event!=EVENT_COMMIT) return 0;
	
	//Close COM Port
	RS232_Close();
	
	//Close GUI
    QuitUserInterface (0);
    return 0;
}

int CVICALLBACK distortion_button_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{   int button_state;

    if (event!=EVENT_COMMIT) return 0; 
    
	//Get value (on or off)
    GetCtrlVal(panel, control, &button_state);
    
    switch_distortion (button_state);
	return 0;
}


int CVICALLBACK distortionChange (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	unsigned short int distortionValue;
	if (event!=EVENT_COMMIT) return 0;  
	
	GetCtrlVal(panel, control, &distortionValue);
	
	set_distortion(distortionValue);
	return 0;
}

void ComCallback(int portNumber, int eventMask,void *CallbackData)
{
	static unsigned char volValue = 0; 
	
	if (eventMask & LWRS_RECEIVE)
	{
		get_vol(&volValue);
		
		SetCtrlVal(panelHandle, PANEL_output_volume_slide, volValue - 120);
	}
}

/*int CVICALLBACK trebleChange (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	double trebleValue;
	if (event!=EVENT_COMMIT) return 0;  
	
	GetCtrlVal(panel, control, &trebleValue);
	
	set_treble(trebleValue);
	
	return 0;
}*/

int CVICALLBACK changeVolume (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int volumeValue;
	if (event!=EVENT_COMMIT) return 0;  
	
	GetCtrlVal(panel, control, &volumeValue);
	
	set_volume(volumeValue);
	return 0;
}
