#include "QtUtil.h"
#include "vtkUnsignedCharArray.h"
#include "vtkPointData.h"
#include "vtkSmartPointer.h"
#include "vtkImageCast.h"
#include "vtkImageActor.h"

QtUtil::QtUtil()
{
}


QtUtil::~QtUtil()
{
}

QImage QtUtil::createQImage(vtkImageData * imageData)
{
	if (!imageData)
		return QImage();

	int width = imageData->GetDimensions()[0];
	int height = imageData->GetDimensions()[1];
	vtkDataArray* arr = imageData->GetPointData()->GetScalars();
	vtkUnsignedCharArray* scalars =
		vtkUnsignedCharArray::SafeDownCast(imageData->GetPointData()->GetScalars());

	if (!width || !height || !scalars)
		return QImage();

	switch (scalars->GetNumberOfComponents())
	{
	case 1:
		return createQImage1(width, height, scalars);
	case 2:
		return createQImage2(width, height, scalars);
	case 3:
		return createQImage3(width, height, scalars);
	case 4:
		return createQImage4(width, height, scalars);
	}

	return QImage();
}

QImage QtUtil::createQImage1(int width, int height, vtkUnsignedCharArray * scalars)
{
	QImage qImage(width, height, QImage::Format_ARGB32);
	vtkIdType tupleIndex = 0;
	int qImageBitIndex = 0;

	QRgb* qImageBits = (QRgb*)qImage.bits();
	unsigned char* scalarTuples = scalars->GetPointer(0);

	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			unsigned char* tuple = scalarTuples + (tupleIndex++);

			QRgb color = qRgba(tuple[0], tuple[0], tuple[0], 255);
			*(qImageBits + (qImageBitIndex++)) = color;
		}
	}

	return qImage;
}

QImage QtUtil::createQImage2(int width, int height, vtkUnsignedCharArray * scalars)
{
	QImage qImage(width, height, QImage::Format_ARGB32);
	vtkIdType tupleIndex = 0;
	int qImageBitIndex = 0;

	QRgb* qImageBits = (QRgb*)qImage.bits();
	unsigned char* scalarTuples = scalars->GetPointer(0);

	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			unsigned char* tuple = scalarTuples + (tupleIndex++ * 2);

			QRgb color = qRgba(tuple[0], tuple[0], tuple[0], tuple[1]);
			*(qImageBits + (qImageBitIndex++)) = color;
		}
	}

	return qImage;
}

QImage QtUtil::createQImage3(int width, int height, vtkUnsignedCharArray * scalars)
{
	QImage qImage(width, height, QImage::Format_ARGB32);
	vtkIdType tupleIndex = 0;
	int qImageBitIndex = 0;

	QRgb* qImageBits = (QRgb*)qImage.bits();
	unsigned char* scalarTuples = scalars->GetPointer(0);

	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			unsigned char* tuple = scalarTuples + (tupleIndex++ * 3);

			QRgb color = qRgba(tuple[0], tuple[1], tuple[2], 255);
			*(qImageBits + (qImageBitIndex++)) = color;
		}
	}

	return qImage;
}

QImage QtUtil::createQImage4(int width, int height, vtkUnsignedCharArray * scalars)
{
	QImage qImage(width, height, QImage::Format_ARGB32);
	vtkIdType tupleIndex = 0;
	int qImageBitIndex = 0;

	QRgb* qImageBits = (QRgb*)qImage.bits();
	unsigned char* scalarTuples = scalars->GetPointer(0);

	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			unsigned char* tuple = scalarTuples + (tupleIndex++ * 4);

			QRgb color = qRgba(tuple[0], tuple[1], tuple[2], tuple[3]);
			*(qImageBits + (qImageBitIndex++)) = color;
		}
	}

	return qImage;
}

void QtUtil::readImage(std::string filename)
{

}
