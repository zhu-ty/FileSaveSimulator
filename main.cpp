


#include "INIReader.h"
#include "ControlQueue.h"
#include "Common.hpp"
#include "logger.hpp"


int main(int argc, char* argv[])
{
    std::cout << std::endl;
	//printf("%lld\n", SysUtil::getCurrentTimeMicroSecond());
	//SysUtil::sleep(10);
	//printf("%lld\n", SysUtil::getCurrentTimeMicroSecond());
	//system("pause");
    if(argc <= 1)
    {
		SysUtil::errorOutput("Please specify the config file. Example: ./FileSaveSimulator config.ini");
		return 0;
    }
	INIReader reader(argv[1]);
	if (reader.ParseError() < 0)
	{
		SysUtil::errorOutput(std::string("Can't load :") + argv[1]);
		return 0;
	}
	long A_write_thread = reader.GetInteger("Control", "A_write_thread", -1);
	double B_write_frame_rate_max = reader.GetReal("Control", "B_write_frame_rate_max", -1.0);
	long C_data_size_KB = reader.GetInteger("Control", "C_data_size_KB", -1);
	long D_queue_lenth = reader.GetInteger("Control", "D_queue_lenth", -1);
	long E_read_thread = reader.GetInteger("Control", "E_read_thread", -1);
	std::string F_save_path = reader.Get("Control", "F_save_path", ".");

	if (A_write_thread < 0 || B_write_frame_rate_max < 0 || C_data_size_KB < 0 || D_queue_lenth < 0 || E_read_thread < 0 || F_save_path == "")
	{
		SysUtil::errorOutput("config file parse error, something's wrong");
		return 0;
	}

	CQ::ControlQueue queue;
	SysUtil::mkdir(F_save_path);
	queue.init(D_queue_lenth, C_data_size_KB * 1024);
	CLog::setlogfile(F_save_path + "/log" + SysUtil::getTimeString() + ".txt");
	queue.start_read_threads(E_read_thread, F_save_path);
	queue.start_write_threads(A_write_thread, B_write_frame_rate_max);
	getchar();
	queue.stop_write_threads();
	queue.stop_read_threads();
	
    return 0;
}
