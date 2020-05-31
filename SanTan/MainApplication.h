#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainApplication.h"
/*vtk File*/
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkImageViewer2.h>
#include <QVTKWidget.h>
#include <vtkDICOMImageReader.h>
#include <vtkJPEGReader.h>
#include <vtkPNGReader.h>
#include <vtkImageActor.h>
#include <vtkEventQtSlotConnect.h>
#include "vtkCommand.h"
#include <vtkOutputWindow.h>
#include "vtkSmartPointer.h"
#include "vtkImageBlend.h"
//为了能够使用vtk添加代码
#include "vtkAutoInit.h"
VTK_MODULE_INIT(vtkRenderingOpenGL2); // VTK was built with vtkRenderingOpenGL2
VTK_MODULE_INIT(vtkInteractionStyle);

class MainApplication : public QMainWindow
{
    Q_OBJECT

public:
    MainApplication(QWidget *parent = Q_NULLPTR);

public slots:
	void openCT_clicked();
	void openXRay_clicked();
	void viewDRR();
	void viewXRay();
	void viewImage(int flag);
	void chooseReader();
	void changeOpacity();

private:
    Ui::MainApplication ui;
	vtkSmartPointer<vtkImageViewer2> m_pImageViewer;	
	vtkSmartPointer<vtkRenderer> m_pRenderer;
	vtkSmartPointer<vtkImageBlend> imageBlend;
	vtkSmartPointer<vtkDICOMImageReader> CTReader;
	vtkSmartPointer<vtkDICOMImageReader> XRayReader;
	bool isDRR;											//是否已经生成DRR
	bool isXray;										//是否传入Xray图像
	bool isCT;											//是否传入CT图像
	bool DRR_visible;									//是否显示DRR
	bool XRay_visible;									//是否显示XRay
	int layer;											//图层数
	int cur_Xray;										//Xray所在图层
	int cur_DRR;										//DRR所在图层

private:
	int getLayer();
	void clearLayer();
	double getOpacity();

};
