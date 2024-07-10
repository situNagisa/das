/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2022. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1
#define  PANEL_QUITBUTTON                 2       /* control type: command, callback function: QuitCallback */
#define  PANEL_LED                        3       /* control type: LED, callback function: (none) */
#define  PANEL_GRAPH_2                    4       /* control type: graph, callback function: (none) */
#define  PANEL_GRAPH                      5       /* control type: graph, callback function: (none) */
#define  PANEL_RING_3                     6       /* control type: ring, callback function: DisplayChDimmSetCB */
#define  PANEL_RING_4                     7       /* control type: ring, callback function: (none) */
#define  PANEL_RING_2                     8       /* control type: ring, callback function: (none) */
#define  PANEL_NUMERIC_12                 9       /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_13                 10      /* control type: numeric, callback function: TotalPointNumCheckCB */
#define  PANEL_NUMERIC_34                 11      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_17                 12      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_20                 13      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_35                 14      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_2                  15      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_26                 16      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_27                 17      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_25                 18      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_38                 19      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_37                 20      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_36                 21      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_24                 22      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_4                  23      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC                    24      /* control type: numeric, callback function: (none) */
#define  PANEL_TEXTMSG_3                  25      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_5                  26      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_4                  27      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG                    28      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_2                  29      /* control type: textMsg, callback function: (none) */
#define  PANEL_TOGGLEBUTTON               30      /* control type: textButton, callback function: StartStopCB */
#define  PANEL_PICTURE                    31      /* control type: picture, callback function: (none) */
#define  PANEL_RADIOBUTTON_6              32      /* control type: radioButton, callback function: (none) */
#define  PANEL_RADIOBUTTON_5              33      /* control type: radioButton, callback function: (none) */
#define  PANEL_RADIOBUTTON_3              34      /* control type: radioButton, callback function: (none) */
#define  PANEL_LED_2                      35      /* control type: LED, callback function: (none) */
#define  PANEL_OKBUTTON_3                 36      /* control type: command, callback function: RdOffsetDataCB */
#define  PANEL_OKBUTTON_5                 37      /* control type: command, callback function: RD_REG_CB */
#define  PANEL_OKBUTTON_2                 38      /* control type: command, callback function: WrOffsetDataCB */
#define  PANEL_OKBUTTON_4                 39      /* control type: command, callback function: WR_REG_CB */
#define  PANEL_OKBUTTON                   40      /* control type: command, callback function: PlayDataCB */
#define  PANEL_BINARYSWITCH_7             41      /* control type: binary, callback function: RefClkSrcSelCB */
#define  PANEL_BINARYSWITCH_4             42      /* control type: binary, callback function: (none) */
#define  PANEL_BINARYSWITCH_8             43      /* control type: binary, callback function: (none) */
#define  PANEL_BINARYSWITCH_6             44      /* control type: binary, callback function: (none) */
#define  PANEL_BINARYSWITCH_5             45      /* control type: binary, callback function: (none) */
#define  PANEL_DECORATION                 46      /* control type: deco, callback function: (none) */
#define  PANEL_TEXTMSG_6                  47      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_7                  48      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_8                  49      /* control type: textMsg, callback function: (none) */
#define  PANEL_DECORATION_2               50      /* control type: deco, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK DisplayChDimmSetCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PlayDataCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK QuitCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RD_REG_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RdOffsetDataCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RefClkSrcSelCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK StartStopCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TotalPointNumCheckCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK WR_REG_CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK WrOffsetDataCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
