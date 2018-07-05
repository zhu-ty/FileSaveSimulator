/**
@brief ControlQueue.cpp
The data queue class of the project
@author zhu-ty
@date Jun 22, 2018
*/

#include "ControlQueue.h"
#include <numeric>
#include "logger.hpp"

#define FRAME_STATIC_COUNT 100

void CQ::ControlQueue::_write_thread_function(int thread_idx)
{
	int frame_count = 0;
	int full_happen = 0;
	unsigned char * tmp_data = new unsigned char[this->_data_len];
	for (int i = 0; i < this->_data_len; i++)
		tmp_data[i] = i % 256;
	unsigned char * tmp_data2 = new unsigned char[this->_data_len];
	int64_t last_print_time = SysUtil::getCurrentTimeMicroSecond();
	while (true)
	{
		double this_frame_start_time = SysUtil::getCurrentTimeMicroSecond() / 1000000.0;
		if (_write_thread_shall_stop == true)
			break;
		int found = -1;
		for (int i = 0; i < _data_queue.size(); i++)
		{
			if (_data_queue[i].status == QueueStatus::Avalible)
			{
				_data_queue[i].status = QueueStatus::Writing;
				found = i;
				break;
			}
		}
		if (found == -1)
		{
			SysUtil::warningOutput(SysUtil::format(
				"Writing thread id = %d, queue full! (This is the %d times happening)",
				thread_idx, full_happen));
			CLog::WriteLog(SysUtil::format(
				"Writing thread id = %d, queue full! (This is the %d times happening)",
				thread_idx, full_happen));
			full_happen++;
			continue;
		}
		memcpy(tmp_data2, tmp_data, this->_data_len);// <- time costing
		memcpy(_data_queue[found].data, tmp_data2, this->_data_len);// <- time costing
		_data_queue[found].birth_stamp = SysUtil::getCurrentTimeMicroSecond();
		_data_queue[found].status = QueueStatus::Occupied;
		frame_count++;
		if (frame_count % FRAME_STATIC_COUNT == 0)
		{
			double diff_print_time_s = (SysUtil::getCurrentTimeMicroSecond() - last_print_time) / 1000000.0;
			SysUtil::infoOutput(SysUtil::format(
				"Writing thread id = %d, queue writing frame rate : %f",
				thread_idx, FRAME_STATIC_COUNT / diff_print_time_s));
			CLog::WriteLog(SysUtil::format(
				"Writing thread id = %d, queue writing frame rate : %f",
				thread_idx, FRAME_STATIC_COUNT / diff_print_time_s));
			last_print_time = SysUtil::getCurrentTimeMicroSecond();
		}
		double this_frame_end_time = SysUtil::getCurrentTimeMicroSecond() / 1000000.0;
		double frame_rate_diff = 1.0 / _write_frame_rate_max - (this_frame_end_time - this_frame_start_time);
		if (frame_rate_diff > 0)
		{
			SysUtil::sleep(frame_rate_diff * 1000.0);
		}
	}
	delete[] tmp_data;
	delete[] tmp_data2;
}

void CQ::ControlQueue::_read_thread_function(int thread_idx)
{
	int empty_happen = 0;
	int frame_count = 0;
	std::vector<int64_t> diff_time;
	int64_t last_print_time = SysUtil::getCurrentTimeMicroSecond();
	while (true)
	{
		if (_read_thread_shall_stop == true)
			break;
		int found = -1;
		for (int i = 0; i < _data_queue.size(); i++)
		{
			if (_data_queue[i].status == QueueStatus::Occupied)
			{
				_data_queue[i].status = QueueStatus::Reading;
				found = i;
				break;
			}
		}
		if (found == -1)
		{
			if(empty_happen % (200 * FRAME_STATIC_COUNT) == 0)
				SysUtil::infoOutput(SysUtil::format(
					"Reading thread id = %d, queue empty reached, sleep for some time. (This is the %d times happening)",
					thread_idx, empty_happen));
			SysUtil::sleep(this->_read_wait_sleep_ms);
			empty_happen++;
			continue;
		}
		diff_time.push_back(SysUtil::getCurrentTimeMicroSecond() - _data_queue[found].birth_stamp);
		if (diff_time.size() >= FRAME_STATIC_COUNT)
		{
			int64_t total_diff = std::accumulate(diff_time.begin(), diff_time.end(), (int64_t)0);
			double average_diff = (double)total_diff / diff_time.size();
			diff_time.clear();
			SysUtil::infoOutput(SysUtil::format(
				"Reading thread id = %d, last %d save diff time average = %f us",
				thread_idx, FRAME_STATIC_COUNT, average_diff));
			CLog::WriteLog(SysUtil::format(
				"Reading thread id = %d, last %d save diff time average = %f us",
				thread_idx, FRAME_STATIC_COUNT, average_diff));
		}
		std::string file_name = this->_save_path + SysUtil::format("id%d_count%d.tmp", thread_idx, frame_count);
		FILE *out = fopen(file_name.c_str(), "wb");
		int file_size_wrote = 0;
		while (file_size_wrote != _data_queue[found].data_len)
		{
			file_size_wrote += fwrite(
				_data_queue[found].data + file_size_wrote,
				sizeof(unsigned char),
				_data_queue[found].data_len - file_size_wrote,
				out); // <- time costing
		}
		fclose(out);
		_data_queue[found].status = QueueStatus::Avalible;
		frame_count++;
		if (frame_count % FRAME_STATIC_COUNT == 0)
		{
			double diff_print_time_s = (SysUtil::getCurrentTimeMicroSecond() - last_print_time) / 1000000.0;
			SysUtil::infoOutput(SysUtil::format(
				"Reading thread id = %d, file writing frame rate : %f",
				thread_idx, FRAME_STATIC_COUNT / diff_print_time_s));
			CLog::WriteLog(SysUtil::format(
				"Reading thread id = %d, file writing frame rate : %f",
				thread_idx, FRAME_STATIC_COUNT / diff_print_time_s));
			last_print_time = SysUtil::getCurrentTimeMicroSecond();
		}
	}
}

int CQ::ControlQueue::init(int queue_lenth, int64_t data_lenth)
{
	_data_len = data_lenth;
	for (int i = 0; i < queue_lenth; i++)
	{
		QueueNode node;
		node.status = QueueStatus::Avalible;
		node.birth_stamp = SysUtil::getCurrentTimeMicroSecond();
		node.data_len = _data_len;
		node.data = new unsigned char[_data_len];
		_data_queue.push_back(node);
	}
	return 0;
}

int CQ::ControlQueue::start_write_threads(int thread_count, double frame_rate_max)
{
	if (_write_thread_running)
	{
		SysUtil::warningOutput("ControlQueue::start_write_threads can't start the writing thread while it's still running!");
		return -1;
	}
	_write_threads.resize(thread_count);
	_write_frame_rate_max = frame_rate_max;
	_write_thread_shall_stop = false;
	for (int i = 0; i < _write_threads.size(); i++)
	{
		_write_threads[i] = std::thread(&ControlQueue::_write_thread_function, this, i);
	}
	_write_thread_running = true;
	return 0;
}

int CQ::ControlQueue::start_read_threads(int thread_count, std::string save_path, double wait_sleep_time_ms)
{
	if (_read_thread_running)
	{
		SysUtil::warningOutput("ControlQueue::start_read_threads can't start the reading thread while it's still running!");
		return -1;
	}
	_read_threads.resize(thread_count);
	_save_path = save_path + "/" + SysUtil::getTimeString() + "/";
	SysUtil::mkdir(_save_path);
	_read_wait_sleep_ms = wait_sleep_time_ms;
	_read_thread_shall_stop = false;
	for (int i = 0; i < _read_threads.size(); i++)
	{
		_read_threads[i] = std::thread(&ControlQueue::_read_thread_function, this, i);
	}
	_read_thread_running = true;
	return 0;
}

int CQ::ControlQueue::stop_write_threads()
{
	if (!_write_thread_running)
	{
		SysUtil::warningOutput("ControlQueue::stop_write_threads can't stop the writing thread while it's not running!");
		return -1;
	}
	_write_thread_shall_stop = true;
	for (int i = 0; i < _write_threads.size(); i++)
	{
		_write_threads[i].join();
	}
	SysUtil::infoOutput("Writing threads stop success.");
	_write_thread_running = false;
	return 0;
}

int CQ::ControlQueue::stop_read_threads()
{
	if (!_read_thread_running)
	{
		SysUtil::warningOutput("ControlQueue::stop_read_threads can't stop the reading thread while it's not running!");
		return -1;
	}
	_read_thread_shall_stop = true;
	for (int i = 0; i < _read_threads.size(); i++)
	{
		_read_threads[i].join();
	}
	SysUtil::infoOutput("Reading threads stop success.");
	_read_thread_running = false;
	return 0;
}

int CQ::ControlQueue::clear()
{
	if (_write_thread_running || _read_thread_running)
	{
		SysUtil::warningOutput("ControlQueue::clear Clearing the data queue while the write/read thread is still running!");
		return -1;
	}
	for (int i = 0; i < _data_queue.size(); i++)
	{	
		delete[] _data_queue[i].data;
	}
	return 0;
}
