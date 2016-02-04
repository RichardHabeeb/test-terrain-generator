#pragma once
class BmpReader
{
public:
	BmpReader();
	~BmpReader();

	static POINT GetGreyScaleData(const char* filename, float* &output);
};

