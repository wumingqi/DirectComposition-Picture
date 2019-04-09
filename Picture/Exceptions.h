#pragma once
#include <stdexcept>

class FileNotSupportException :public std::exception
{
public:
	std::wstring msg;

	FileNotSupportException(std::wstring msg) {
		this->msg = msg;
	}
};