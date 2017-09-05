#pragma once
#include <memory>  
#include <ctime>  
#include <iostream>  
#include <fstream>  

class EasyLog
{
public:
	static EasyLog * Inst() {
		if (0 == _instance.get()) {
			_instance.reset(new EasyLog);
		}
		return _instance.get();
	}

	void Log(std::string msg); // 写日志的方法  
private:
	EasyLog(void) {}
	virtual ~EasyLog(void) {}
	friend class std::auto_ptr<EasyLog>;
	static std::auto_ptr<EasyLog> _instance;
};

