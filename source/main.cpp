#include "c4d.h"
#include "main.h"


#define PLUGIN_VERSION	String("Switch Object 0.6")


Bool PluginStart()
{
	if (!RegisterSwitchObject())
		return FALSE;
	
	GePrint(PLUGIN_VERSION);

	return TRUE;
}

void PluginEnd()
{}

Bool PluginMessage(Int32 id, void *data)
{
	// use the following lines to set a plugin priority
	// 
	switch (id)
	{
		case C4DPL_INIT_SYS:
			return resource.Init(); // don't start plugin without resource
	}

	return FALSE;
}
