/*
 * vitainput.cpp - functionality for input from vita
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <framework/vitainput.h>
#include <framework/task.h>

VitaInput::VitaInput(void)
{
	setButtonSampleSize(5);
	setFrontTouchSampleSize(5);
	setRearTouchSampleSize(5);

	m_buttonTask = std::make_shared<Task>();
	m_frontTouchTask = std::make_shared<Task>();
	m_rearTouchTask = std::make_shared<Task>();

	// create the input processing task
	m_buttonTask->set(std::bind(&VitaInput::readButtonInput, this));
	m_frontTouchTask->set(std::bind(&VitaInput::readTouchInput, this, SCE_TOUCH_PORT_FRONT, &m_frontTouchData));
	m_rearTouchTask->set(std::bind(&VitaInput::readTouchInput, this, SCE_TOUCH_PORT_BACK, &m_rearTouchData));
}

VitaInput::~VitaInput(void)
{
	m_buttonTask->set(nullptr);
	m_rearTouchTask->set(nullptr);
	m_frontTouchTask->set(nullptr);

	deleteInputSample(&m_buttonData);
	deleteInputSample(&m_frontTouchData);
	deleteInputSample(&m_rearTouchData);
}

std::vector<TaskPtr> VitaInput::tasks(void)
{
	return {m_buttonTask, m_frontTouchTask, m_rearTouchTask };
}

void VitaInput::setButtonSampleSize(unsigned int samples)
{
	setInputSampleSize(&m_buttonData, samples);
}

void VitaInput::setFrontTouchSampleSize(unsigned int samples)
{
	setInputSampleSize(&m_frontTouchData, samples);
}

void VitaInput::setRearTouchSampleSize(unsigned int samples)
{
	setInputSampleSize(&m_rearTouchData, samples);
}

void VitaInput::readButtonInput(void)
{
	std::shared_lock<std::shared_mutex> lock(m_buttonData.mutex);
	auto samples = sceCtrlReadBufferPositive(0, m_buttonData.data, m_buttonData.samples);

	// stop if we have an error
	if (samples < 0)
		return;

	auto buttons = 0;

	// we shouldn't be lagged behind, but if we are we will combine samples
	for (auto i = 0u; i < m_buttonData.samples; ++i)
	{
		buttons |= m_buttonData.data[i].buttons;
	}

	// send event
	//ButtonEvent event(buttons);
	//GuiApplication::sendEvent(&event);
}

void VitaInput::readTouchInput(unsigned int port, TouchInput *data)
{
	std::shared_lock<std::shared_mutex> lock(data->mutex);
	auto state = 0lu;

	// if error we just stop
	if (sceTouchGetSamplingState(port, &state) < 0)
		return;

	// dont read data if input is disabled
	if (state == SCE_TOUCH_SAMPLING_STATE_STOP)
		return;

	auto samples = sceTouchRead(port, data->data, data->samples);

	if (samples < 0)
		return;

	// TODO: implement
}
