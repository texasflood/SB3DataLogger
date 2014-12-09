/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2014. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                           1
#define  PANEL_quit_button               2       /* callback function: quit_button_hit */
#define  PANEL_USER_COM_PORT             3
#define  PANEL_COM_connect               4       /* callback function: COM_button_hit */
#define  PANEL_distortion                5       /* callback function: distortionChange */
#define  PANEL_output_volume_slide       6
#define  PANEL_NUMERICSLIDE              7       /* callback function: changeVolume */
#define  PANEL_Echo                      8       /* callback function: changeEcho */
#define  PANEL_distortion_button         9       /* callback function: distortion_button_hit */
#define  PANEL_echoSwitch                10      /* callback function: echo_button_hit */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK changeEcho(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK changeVolume(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK COM_button_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK distortion_button_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK distortionChange(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK echo_button_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK quit_button_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
