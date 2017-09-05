#include "stdafx.h"
#include "EasyLog.h"


std::auto_ptr<EasyLog> EasyLog::_instance;

void EasyLog::Log(std::string loginfo) {
	std::ofstream ofs;
	time_t t = time(0);
	char tmp[64];
	strftime(tmp, sizeof(tmp), "\t[%Y.%m.%d %X %A]", localtime(&t));
	ofs.open("EasyLog.log", std::ofstream::app);
	ofs.write(loginfo.c_str(), loginfo.size());
	ofs << tmp << '\n';
	ofs.close();
}
