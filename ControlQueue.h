/**
@brief ControlQueue.h
The data queue class of the project
@author zhu-ty
@date Jun 22, 2018
*/

#ifndef __FILE_SAVE_SIMULATOR_CONTROL_QUEUE__
#define __FILE_SAVE_SIMULATOR_CONTROL_QUEUE__
#pragma once

#include "Common.hpp"

namespace CQ
{
	enum class QueueStatus
	{
		Avalible = 0,
		Writing = 1,
		Reading = 2,
		Occupied = 3,
		Undefined = 4
	};

	struct QueueNode
	{
		QueueStatus status;
		int64_t birth_stamp;
		int64_t data_len;
		unsigned char * data;
	};

	class ControlQueue
	{
	private:
		std::vector<QueueNode> _data_queue;
		int64_t _data_len = 0;

		std::vector<std::thread> _write_threads;
		std::vector<std::thread> _read_threads;
		std::string _save_path = "./";
		double _write_frame_rate_max = 10.0;
		double _read_wait_sleep_ms = 1.0;
		bool _write_thread_running = false;
		bool _read_thread_running = false;
		bool _write_thread_shall_stop = false;
		bool _read_thread_shall_stop = false;

		void _write_thread_function(int thread_idx);
		void _read_thread_function(int thread_idx);

	public:
		ControlQueue() {};
		~ControlQueue() { clear(); };

		int init(int queue_lenth, int64_t data_lenth);

		int start_write_threads(int thread_count, double frame_rate_max);
		int start_read_threads(int thread_count, std::string save_path, double wait_sleep_time_ms = 1.0);

		int stop_write_threads();
		int stop_read_threads();

		int clear();

	};
}

#endif // !__FILE_SAVE_SIMULATOR_CONTROL_QUEUE__