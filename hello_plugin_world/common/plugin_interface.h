/**
 * This file contains interface between main executable and plugin code
 * That is functions and variables from main executable that can be called by the plugin
 * and the structure used by the plugin to export its entry points
 */

#ifndef PLUGIN_INTERFACE_H
#define PLUGIN_INTERFACE_H

struct PluginPointers {
	/* Interface version */
	uint32_t version;

	/* Some function called by the main binary */
	int32_t (*doTheJob)();
};

#define PluginPointers_VERSION 1

/* Some sample function in main binary returning an int */
int getSomeMainInt();

/* Some sample variable */
extern int somePublicValueForPlugin;

#endif
