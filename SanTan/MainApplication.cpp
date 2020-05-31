#include "MainApplication.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QtGlobal>
#include <QPaintEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsOpacityEffect>
#include "QtUtil.h"


MainApplication::MainApplication(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	//���ֿؼ���ʼֵ����
	ui.m_ddr_box->setChecked(true);
	ui.m_xray_box->setChecked(true);
	m_pImageViewer = vtkSmartPointer<vtkImageViewer2>::New();
	m_pRenderer = vtkSmartPointer<vtkRenderer>::New();
	imageBlend = vtkSmartPointer<vtkImageBlend>::New();
	imageBlend->SetBlendModeToCompound();
	ui.qvtkWidget->GetRenderWindow()->AddRenderer(m_pRenderer);
	isCT = false;
	isDRR = false;
	isXray = false;
	DRR_visible = true;
	XRay_visible = true;
	layer = 0;
	m_pImageViewer->SetInputData(imageBlend->GetOutput());
	m_pImageViewer->UpdateDisplayExtent();
	m_pImageViewer->SetRenderWindow(ui.qvtkWidget->GetRenderWindow());
	m_pImageViewer->SetRenderer(m_pRenderer);
	m_pImageViewer->SetupInteractor(ui.qvtkWidget->GetRenderWindow()->GetInteractor());
	m_pImageViewer->SetSliceOrientationToXY();//Ĭ�Ͼ����������
	m_pImageViewer->GetImageActor()->InterpolateOff();
	m_pRenderer->ResetCamera();
	m_pRenderer->DrawOn();

	connect(ui.m_ct_btn, SIGNAL(clicked()), this, SLOT(openCT_clicked()));	//��CT
	connect(ui.m_xray_btn, SIGNAL(clicked()), this, SLOT(openXRay_clicked()));//��X-ray

	//��������

	//�Ƚ�����

	//ȡ������

	connect(ui.m_ddr_box, SIGNAL(clicked(bool)), this, SLOT(viewDRR()));//��ʾdrr
	connect(ui.m_xray_box, SIGNAL(clicked(bool)), this, SLOT(viewXRay()));//��ʾx-ray
	connect(ui.opacity_text, SIGNAL(editingFinished()), this, SLOT(changeOpacity()));//ddrͼ͸��ռ��

	//����ƽ��

	//��ת��
	//���Դ���
}

void MainApplication::openCT_clicked() {
	QFileDialog *fileDialog = new QFileDialog(this);
	fileDialog->setWindowTitle(QStringLiteral("ѡ��CTͼ��"));
	fileDialog->setDirectory(".");
	fileDialog->setNameFilter(tr("Images (*.dcm)"));
	fileDialog->setFileMode(QFileDialog::ExistingFiles);
	fileDialog->setViewMode(QFileDialog::Detail);
	QString filename;
	if (fileDialog->exec()) {
		filename = fileDialog->selectedFiles()[0];
	}
	if (filename.isEmpty()) return;
	else {
		viewImage(0);
	}
}

void MainApplication::openXRay_clicked() {
	QFileDialog *fileDialog = new QFileDialog(this);
	fileDialog->setWindowTitle(QStringLiteral("ѡ��X-Rayͼ��"));
	fileDialog->setDirectory(".");
	fileDialog->setNameFilter(tr("Images (*.dcm)"));
	fileDialog->setFileMode(QFileDialog::ExistingFiles);
	fileDialog->setViewMode(QFileDialog::Detail);
	QString filename;
	if (fileDialog->exec()) {
		filename = fileDialog->selectedFiles()[0];
	}
	if (filename.isEmpty()) return;
	else {
		viewImage(2);
	}
}

void MainApplication::viewDRR() {
	if (DRR_visible) {
		DRR_visible = false;
	}
	else
		DRR_visible = true;
	if (isXray || isDRR) {
		chooseReader();
	}
}

void MainApplication::viewXRay() {
	if (XRay_visible)
		XRay_visible = false;
	else
		XRay_visible = true;
	if (isXray || isDRR) {
		chooseReader();
	}
}

void MainApplication::chooseReader() {
	if (XRay_visible && DRR_visible && isXray && isDRR) { //��ʾ���ͼ��
		m_pImageViewer->SetInputData(imageBlend->GetOutput());
	}
	else if (DRR_visible && isDRR) {	//ֻ��ʾDRR
		m_pImageViewer->SetInputData(CTReader->GetOutput());
	}
	else if (XRay_visible && isXray) {	//ֻ��ʾXRay
		m_pImageViewer->SetInputData(XRayReader->GetOutput());
	}
	else {				//������ʾ
		m_pImageViewer->SetInputData(vtkImageData::New());
	}
	m_pImageViewer->UpdateDisplayExtent();
	m_pImageViewer->SetRenderWindow(ui.qvtkWidget->GetRenderWindow());
	m_pImageViewer->SetRenderer(m_pRenderer);
	ui.qvtkWidget->GetRenderWindow()->Render();
}

void MainApplication::changeOpacity()
{
	if (isDRR) {
		imageBlend->SetOpacity(cur_DRR, getOpacity());
		imageBlend->Update();
		ui.qvtkWidget->GetRenderWindow()->Render();
	}
	if (isXray) {
		imageBlend->SetOpacity(cur_Xray, 1.0-getOpacity());
		imageBlend->Update();
		ui.qvtkWidget->GetRenderWindow()->Render();
	}
}

int MainApplication::getLayer() {
	if (layer == 0 || layer == 1)
		return layer;
	else
		return 0;
}

void MainApplication::clearLayer() {
	layer = 0;
	imageBlend = vtkSmartPointer<vtkImageBlend>::New();
	isCT = false;
	isDRR = false;
	isXray = false;
}

double MainApplication::getOpacity()
{
	double opcaity = ui.opacity_text->text().toDouble();
	if (0 <= opcaity && opcaity <= 100) {
		return opcaity / 100.0;
	}
	return 0;
}

void MainApplication::viewImage(int flag) {
	if (flag == 0) {	//����CT
		// ����CTͼ��֮��ʹ��DRR�㷨����DRRͼ��
		// ��δʵ��
		isCT = true;
		viewImage(1);
	}
	else if (flag == 1) { // չʾDRRͼ��
		isDRR = true;
		// ��ʱ����CTͼ��������
		CTReader = vtkSmartPointer<vtkDICOMImageReader>::New();
		//CTReader->SetFileName(filename.toStdString().data());
		CTReader->SetFileName("D:\\QT\\1-yaozhui5\\1-yaozhui5\\CT164326_651423_00004_00081_216_W400L40.DCM");
		CTReader->Update();
		imageBlend->AddInputData(CTReader->GetOutput());
		imageBlend->SetOpacity(getLayer(), getOpacity());
		cur_DRR = getLayer();
		imageBlend->Update();
		m_pImageViewer->SetInputData(imageBlend->GetOutput());
		m_pImageViewer->UpdateDisplayExtent();
		m_pImageViewer->SetRenderWindow(ui.qvtkWidget->GetRenderWindow());
		m_pImageViewer->SetRenderer(m_pRenderer);
		ui.qvtkWidget->GetRenderWindow()->Render();
		layer++;
	}
	else if (flag == 2) { // չʾXRayͼ��
		isXray = true;
		XRayReader = vtkSmartPointer<vtkDICOMImageReader>::New();
		//CTReader->SetFileName(filename.toStdString().data());
		XRayReader->SetFileName("D:\\QT\\1-yaozhui5\\1-yaozhui5\\CT164326_651423_00004_00147_282_W400L40.DCM");
		XRayReader->Update();
		imageBlend->AddInputData(XRayReader->GetOutput());
		imageBlend->SetOpacity(getLayer(), 1.0 - getOpacity());
		cur_Xray = getLayer();
		imageBlend->Update();
		m_pImageViewer->SetInputData(imageBlend->GetOutput());
		m_pImageViewer->UpdateDisplayExtent();
		m_pImageViewer->SetRenderWindow(ui.qvtkWidget->GetRenderWindow());
		m_pImageViewer->SetRenderer(m_pRenderer);
		ui.qvtkWidget->GetRenderWindow()->Render();
		layer++;
	}
}