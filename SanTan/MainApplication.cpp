#include "MainApplication.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QtGlobal>
#include <QPaintEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsOpacityEffect>
#include "QtUtil.h"

// 适配ITK
#include <winsock2.h> 
#pragma comment(lib, "WS2_32")
#pragma comment(lib, "Rpcrt4.lib")

MainApplication::MainApplication(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	//部分控件初始值设置
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
	m_pImageViewer->SetSliceOrientationToXY();//默认就是这个方向
	m_pImageViewer->GetImageActor()->InterpolateOff();
	m_pRenderer->ResetCamera();
	m_pRenderer->DrawOn();

	connect(ui.m_ct_btn, SIGNAL(clicked()), this, SLOT(openCT_clicked()));	//打开CT
	connect(ui.m_xray_btn, SIGNAL(clicked()), this, SLOT(openXRay_clicked()));//打开X-ray

	//划分区域

	//比较区域

	//取消区域

	connect(ui.m_ddr_box, SIGNAL(clicked(bool)), this, SLOT(viewDRR()));//显示drr
	connect(ui.m_xray_box, SIGNAL(clicked(bool)), this, SLOT(viewXRay()));//显示x-ray
	connect(ui.opacity_text, SIGNAL(editingFinished()), this, SLOT(changeOpacity()));//ddr图透明占比

	//像素平移
	//旋转度
	//参数调整
	connect(ui.confirm_btn, SIGNAL(clicked()), this, SLOT(adjustPara()));
	//测试代码
}

void MainApplication::openCT_clicked() {
	// 清除上一个CT序列
	clearLayer();
	// 读取序列图像
	// 进入ITKDRR处理
	QString filedic = QFileDialog::getExistingDirectory(
		this, QStringLiteral("选择CT图像序列"), "."
	);
	if (filedic.isEmpty()) return;
	else {
		// 使用ITKDRR类读入图像序列
		drr->setInputName(filedic.toStdString().data());
		viewImage(0);
	}
}

void MainApplication::openXRay_clicked() {
	QFileDialog *fileDialog = new QFileDialog(this);
	fileDialog->setWindowTitle(QStringLiteral("选择X-Ray图像"));
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
	if (XRay_visible && DRR_visible && isXray && isDRR) { //显示混合图像
		m_pImageViewer->SetInputData(imageBlend->GetOutput());
	}
	else if (DRR_visible && isDRR) {	//只显示DRR
		m_pImageViewer->SetInputData(drr->ivfilter->GetOutput());
	}
	else if (XRay_visible && isXray) {	//只显示XRay
		m_pImageViewer->SetInputData(XRayReader->GetOutput());
	}
	else {				//都不显示
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
	// 异步处理DRR图像
	// 暂未写异步方法
	// 先初始化
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
	//此方法中的所有步骤都应该采用多线程来避免UI卡顿
	if (flag == 0) {	//调用ITKDRR中的DRR算法
		// 导入CT图像之后使用DRR算法生成DRR图像
		//
		isCT = true;
		adjustPara();
		isDRR = true;
	}
	else if (flag == 1) { // 展示DRR图像
		return;
	}
	else if (flag == 2) { // 展示XRay图像
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