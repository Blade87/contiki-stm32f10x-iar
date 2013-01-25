#include <string.h>
#include <stdio.h>

#include "contiki.h"
#include "ctk/ctk.h"


/* The main window. */
static struct ctk_window mainwindow;

static struct ctk_label message =
  {CTK_LABEL(0, 0, 15, 1, "Latest requests")};

PROCESS(webserver_process, "Web server");

#if WITH_CTK
AUTOSTART_PROCESSES(&webserver_process);
#endif

#define LOG_WIDTH  20
#define LOG_HEIGHT 15
static char log[LOG_WIDTH*LOG_HEIGHT];

static struct ctk_label loglabel =
{CTK_LABEL(0, 1, LOG_WIDTH, LOG_HEIGHT, log)};
/*-----------------------------------------------------------------------------------*/
PROCESS_THREAD(webserver_process, ev, data)
{
  PROCESS_BEGIN();
  
  ctk_window_new(&mainwindow, 20, 10, "Web server");
  
 // CTK_WIDGET_ADD(&mainwindow, &message);
  //CTK_WIDGET_ADD(&mainwindow, &loglabel);
  
  ctk_window_open(&mainwindow);

  while(1) {
    PROCESS_WAIT_EVENT();
    if(ev == ctk_signal_window_close ||
       ev == PROCESS_EVENT_EXIT) {
      ctk_window_close(&mainwindow);
      process_exit(&webserver_process);
      LOADER_UNLOAD();    
    } 
  }

  PROCESS_END();
}