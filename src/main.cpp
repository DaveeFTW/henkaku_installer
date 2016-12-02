
#include <psp2/kernel/threadmgr.h>

//#### TODO: move to SDK
extern "C"
{
	unsigned int sleep(unsigned int seconds)
	{
		sceKernelDelayThread(seconds*1000*1000);
		return 0;
	}
	
	int usleep(unsigned int usec)
	{
		sceKernelDelayThread(usec);
		return 0;
	}
}

int main(int argc, char *argv[])
{
}