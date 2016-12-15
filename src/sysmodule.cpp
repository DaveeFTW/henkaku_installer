/*
 * sysmodule.cpp - wrapper around vita sysmodule facility
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "sysmodule.h"

#include <cstring>

#include <psp2/sysmodule.h>

using namespace SysModule;

Module::Module(int id)
	: m_id(id)
{
	sceSysmoduleLoadModuleInternal(id);
}

Module::~Module(void)
{
	sceSysmoduleUnloadModuleInternal(m_id);
}

void Module::initWithArg(void *data, unsigned int size)
{
	struct Arg { unsigned int unk; Arg *ptr; unsigned int pad[0x100-8]; } optArg;
	std::memset(&optArg, 0, sizeof(Arg));
	optArg.ptr = &optArg;

	sceSysmoduleLoadModuleInternalWithArg(m_id, size, data, &optArg);
}
