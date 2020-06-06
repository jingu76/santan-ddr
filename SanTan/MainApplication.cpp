#include "MainApplication.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QtGlobal>
#include <QPaintEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsOpacityEffect>
#include "QtUtil.h"

// ����ITK
#include <winsock2.h> 
#pragma comment(lib, "WS2_32")
#pragma comment(lib, "Rpcrt4.lib")

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
	drr = new itkDrr();
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
	//��������
	connect(ui.confirm_btn, SIGNAL(clicked()), this, SLOT(adjustPara()));
	//���Դ���
}

void MainApplication::openCT_clicked() {
	// �����һ��CT����
	clearLayer();
	// ��ȡ����ͼ��
	// ����ITKDRR����
	QString filedic = QFileDialog::getExistingDirectory(
		this, QStringLiteral("ѡ��CTͼ������"), "."
	);
	if (filedic.isEmpty()) return;
	else {
		// ʹ��ITKDRR�����ͼ������
		drr->setInputName(filedic.toStdString().data());
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
		m_pImageViewer->SetInputData(drr->ivfilter->GetOutput());
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

void MainApplication::vtkImitateDRR()
{
}

void MainApplication::adjustPara()
{
	if (!isCT) {
		return;
	}
	float xp = ui.xp_label->text().toFloat();
	float yp = ui.yp_label->text().toFloat();
	float zp = ui.zp_label->text().toFloat();
	float xr = ui.xr_label->text().toFloat();
	float yr = ui.yr_label->text().toFloat();
	float zr = ui.zr_label->text().toFloat();
	float cx = ui.xt_label->text().toFloat();
	float cy = ui.yt_label->text().toFloat();
	float cz = ui.zt_label->text().toFloat();
	float sid = ui.sid_label->text().toFloat();
	int dx = ui.dx_label->text().toInt();
	int dy = ui.dy_label->text().toInt();
	double threshold = ui.threshold_label->text().toDouble();
	// �첽����DRRͼ��
	// ��δд�첽����
	// �ȳ�ʼ��
	layer = 0;
	imageBlend = nullptr;
	imageBlend = vtkSmartPointer<vtkImageBlend>::New();
	drr->myDrr(xr, yr, zr, xp, yp, zp, cx, cy, cz, sid, dx, dy, threshold);
	imageBlend->AddInputData(drr->ivfilter->GetOutput());
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
	//�˷����е����в��趼Ӧ�ò��ö��߳�������UI����
	if (flag == 0) {	//����ITKDRR�е�DRR�㷨
		// ����CTͼ��֮��ʹ��DRR�㷨����DRRͼ��
		//
		isCT = true;
		adjustPara();
		isDRR = true;
	}
	else if (flag == 1) { // չʾDRRͼ��
		return;
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