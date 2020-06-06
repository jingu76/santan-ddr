#pragma once
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkResampleImageFilter.h"
#include "itkCenteredEuler3DTransform.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkImageSeriesReader.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkRayCastInterpolateImageFunction.h"
#include "itkBMPImageIOFactory.h"
#include "itkGiplImageIOFactory.h"
#include "itkImageToVTKImageFilter.h"
class itkDrr
{
public:
	itkDrr();
	~itkDrr();
	void myDrr(float rX, float rY, float rZ,
		float tX, float tY, float tZ,
		float cX, float cY, float cZ,
		float Sid, int dX, int dY, double thres);
	void setInputName(const char* input);
	itk::ImageToVTKImageFilter<itk::Image<unsigned char, 3>>::Pointer ivfilter;

private:
	bool isRead;
	itk::ImageSeriesReader< itk::Image< signed short, 3 > >::Pointer reader;
	itk::Image<short, 3>::Pointer image;
	char* input_name;


};

