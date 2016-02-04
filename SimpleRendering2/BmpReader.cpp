#include "pch.h"
#include "BmpReader.h"

typedef struct {
	unsigned char r;
	unsigned char b;
	unsigned char g;
} pixel24;

BmpReader::BmpReader()
{
}


BmpReader::~BmpReader()
{
}

POINT BmpReader::GetGreyScaleData(const char * filename, float * &output)
{
	FILE* f = fopen(filename, "rb");
	POINT size;

	// extract image height and width from header
	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, f);
	size.x = *(int*)&info[18];
	size.y = *(int*)&info[22];

	// allocate 3 bytes per pixel
	int numPixels = size.x * size.y;
	
	output = new float[numPixels];
	pixel24* data = new pixel24[size.x];
	//fread(data, sizeof(pixel24), numPixels, f);
	

	for (int j = 0; j < size.y; j++)
	{
		
		short lineEnd;

		//read a line of pixels
		fread(data, sizeof(pixel24), size.x, f);
		fread(&lineEnd, sizeof(unsigned char), 2, f);

		for (int i = 0; i < size.x; i++)
		{
			//Y = .2126 * R^gamma + .7152 * G^gamma + .0722 * B^gamma
			//gamma = 2.2
			float R = float(data[i].r) / 255;
			float G = float(data[i].g) / 255;
			float B = float(data[i].b) / 255;
			//output[i] = .2126 * pow(R, 2.2f) + .7152 * pow(G, 2.2f) + .0722 * pow(B, 2.2f);
			output[size.y*j + i] = R;

			//OutputDebugString(output[i])
		}
	}

	fclose(f);

	return size;
}
