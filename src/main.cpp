#include "installerview.h"

#include <framework/guiapplication.h>
#include <framework/view.h>

#include <psp2/kernel/threadmgr.h>

#include <easyloggingpp/easylogging++.h>
INITIALIZE_EASYLOGGINGPP

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

	void __sinit(struct _reent *);

	extern unsigned char clear_vert_gxp[];
	extern unsigned int clear_vert_gxp_len;
	extern unsigned char clear_frag_gxp[];
	extern unsigned int clear_frag_gxp_len;
}

__attribute__((constructor(101)))
void pthread_setup(void) 
{
	pthread_init();
	__sinit(_REENT);
}
//#### end TODO

int main(int argc, char *argv[])
{
	LOG(INFO) << "starting installer";

	GuiApplication app(argc, argv);
	{
		auto view = std::make_shared<InstallerView>();
		view->show();
		GuiApplication::addView(view);
		app.exec();
	}

	sceKernelDelayThread(5*1000*1000);
}
