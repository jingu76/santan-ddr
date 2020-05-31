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
	QPixmap xray_img;
	QPixmap ct_img;

public slots:
	void openCT_clicked();
	void openXRay_clicked();
	void viewDRR();
	void viewXRay();

private:
    Ui::MainApplication ui;
	vtkSmartPointer<vtkImageViewer2> m_pImageViewer;
	vtkSmartPointer<vtkRenderer> m_pRenderer;
	vtkSmartPointer<vtkImageBlend> imageBlend;
	bool isDRR;
	bool isXray;
	bool isCT;
};
