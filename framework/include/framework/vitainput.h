/*
 * vitainput.h - functionality for input from vita
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef VITAINPUT_H
#define VITAINPUT_H

#include <framework/input.h>
#include <shared_mutex>

#include <psp2/ctrl.h>
#include <psp2/touch.h>

class VitaInput : public Input
{
	template <typename T>
	struct SampledInput
	{
		mutable std::shared_mutex mutex;
		T *data{nullptr};
		unsigned int samples{0};
	};

	using TouchInput = SampledInput<SceTouchData>;
	using ButtonInput = SampledInput<SceCtrlData>;

public:
	VitaInput(void);
	~VitaInput(void);

	std::vector<TaskPtr> tasks(void) override;

	void setButtonSampleSize(unsigned int samples);
	void setFrontTouchSampleSize(unsigned int samples);
	void setRearTouchSampleSize(unsigned int samples);

private:
	template <typename T>
	void setInputSampleSize(SampledInput<T> *input, unsigned int samples)
	{
		std::unique_lock<std::shared_mutex> lock(input->mutex);

		if (input->data)
			delete[] input->data;

		input->samples = samples;
		input->data = new T[input->samples];
	}

	template <typename T>
	void deleteInputSample(SampledInput<T> *input)
	{
		std::unique_lock<std::shared_mutex> lock(input->mutex);

		if (input->data)
			delete[] input->data;
	}

	void readButtonInput(void);
	void readTouchInput(unsigned int port, TouchInput *data);

private:
	TouchInput m_frontTouchData, m_rearTouchData;
	ButtonInput m_buttonData;
	TaskPtr m_buttonTask, m_frontTouchTask, m_rearTouchTask;
};

#endif // VITAINPUT_H
