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
#define  PANEL_volume_slide              2
#define  PANEL_quit_button               3       /* callback function: quit_button_hit */
#define  PANEL_read_analog_button        4       /* callback function: read_analog_button_hit */
#define  PANEL_distortion_button         5       /* callback function: distortion_button_hit */
#define  PANEL_USER_COM_PORT             6
#define  PANEL_COM_connect               7       /* callback function: COM_button_hit */
#define  PANEL_distortion                8       /* callback function: distortionChange */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK COM_button_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK distortion_button_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK distortionChange(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK quit_button_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK read_analog_button_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
