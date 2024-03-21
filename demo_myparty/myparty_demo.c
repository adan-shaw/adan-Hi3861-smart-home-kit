#include <stdio.h>
#include "ohos_init.h"
#include "ohos_types.h"

#include "myparty.h"

void myparty_demo (void)
{
	myparty_test ();
	printf ("___________>>>>>>>>>>>>>>>>>>>> [DEMO] Hello world.\n");
}

SYS_RUN (myparty_demo);
