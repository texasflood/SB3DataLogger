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
#define  PANEL_distortion_button         3       /* callback function: distortion_button_hit */
#define  PANEL_USER_COM_PORT             4
#define  PANEL_COM_connect               5       /* callback function: COM_button_hit */
#define  PANEL_distortion                6       /* callback function: distortionChange */
#define  PANEL_output_volume_slide       7
#define  PANEL_Treble                    8       /* callback function: trebleChange */
#define  PANEL_trebleSwitch              9


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK COM_button_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK distortion_button_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK distortionChange(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK quit_button_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK trebleChange(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
