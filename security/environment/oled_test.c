
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

static void OledTask (void *arg)
{
	(void) arg;
	OledInit ();

	OledFillScreen (0x00);
	OledShowString (0, 0, "Hello OpenHarmony!", 2);
	sleep (30);
}

static void OledDemo (void)
{
	osThreadAttr_t attr;

	attr.name = "OledTask";
	attr.attr_bits = 0U;
	attr.cb_mem = NULL;
	attr.cb_size = 0U;
	attr.stack_mem = NULL;
	attr.stack_size = 4096;
	attr.priority = osPriorityNormal;

	if (osThreadNew (OledTask, NULL, &attr) == NULL)
	{
		printf ("[OledDemo] Falied to create OledTask!\n");
	}
}

APP_FEATURE_INIT (OledDemo);
