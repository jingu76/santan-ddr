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
//Ϊ���ܹ�ʹ��vtk��Ӵ���
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
	bool isDRR;											//�Ƿ��Ѿ�����DRR
	bool isXray;										//�Ƿ���Xrayͼ��
	bool isCT;											//�Ƿ���CTͼ��
	bool DRR_visible;									//�Ƿ���ʾDRR
	bool XRay_visible;									//�Ƿ���ʾXRay
	int layer;											//ͼ����
	int cur_Xray;										//Xray����ͼ��
	int cur_DRR;										//DRR����ͼ��

private:
	int getLayer();
	void clearLayer();
	double getOpacity();

};
