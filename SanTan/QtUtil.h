#pragma once
#include <qimage.h>
#include "vtkImageData.h"

class QtUtil
{
public:
	QtUtil();
	~QtUtil();
	static QImage createQImage(vtkImageData* imageData);
	static QImage createQImage1(int width, int height, vtkUnsignedCharArray* scalars);
	static QImage createQImage2(int width, int height, vtkUnsignedCharArray* scalars);
	static QImage createQImage3(int width, int height, vtkUnsignedCharArray* scalars);
	static QImage createQImage4(int width, int height, vtkUnsignedCharArray* scalars);	
	static void readImage(std::string filename);
};

