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
	ui.qvtkWidget->GetRenderWindow()->AddRenderer(m_pRenderer);
	isCT = false;
	isDRR = false;
	isXray = false;
	m_pImageViewer->SetInputData(imageBlend->GetOutput());
	m_pImageViewer->UpdateDisplayExtent();
	m_pImageViewer->SetRenderWindow(ui.qvtkWidget->GetRenderWindow());
	m_pImageViewer->SetRenderer(m_pRenderer);
	m_pImageViewer->SetupInteractor(ui.qvtkWidget->GetRenderWindow()->GetInteractor());
	m_pImageViewer->SetSliceOrientationToXY();//Ĭ�Ͼ����������
	m_pImageViewer->GetImageActor()->InterpolateOff();


	connect(ui.m_ct_btn, SIGNAL(clicked()), this, SLOT(openCT_clicked()));	//��CT
	connect(ui.m_xray_btn, SIGNAL(clicked()), this, SLOT(openXRay_clicked()));//��X-ray

	//��������

	//�Ƚ�����

	//ȡ������

	connect(ui.m_ddr_box, SIGNAL(clicked(bool)), this, SLOT(viewDRR()));//��ʾdrr
	connect(ui.m_xray_box, SIGNAL(clicked(bool)), this, SLOT(viewXRay()));//��ʾx-ray

	//ddrͼ͸��ռ��

	//����ƽ��

	//��ת��
	//���Դ���
}

void MainApplication::openCT_clicked() {
	QFileDialog *fileDialog = new QFileDialog(this);
	fileDialog->setWindowTitle(QStringLiteral("ѡ��CTͼ��"));
	fileDialog->setDirectory(".");
	fileDialog->setNameFilter(tr("Images (*.png *.dcm *.jpg *.tif)"));
	fileDialog->setFileMode(QFileDialog::ExistingFiles);
	fileDialog->setViewMode(QFileDialog::Detail);
	QString filename;
	if (fileDialog->exec()) {
		filename = fileDialog->selectedFiles()[0];
	}
	if (filename.isEmpty()) return;
	else {
		QImage* img = new QImage;
		if (!(img->load(filename))) {
			QMessageBox::information(this, tr("Failed to open CT"), tr("Failed to open CT"));
			delete img;
			return;
		}
		ct_img = QPixmap::fromImage(*img);
		QGraphicsOpacityEffect *opcaityEffect = new QGraphicsOpacityEffect;
		ui.drr_label->setPixmap(ct_img);
		opcaityEffect->setOpacity(0.5);
		ui.drr_label->setGraphicsEffect(opcaityEffect);
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
		vtkSmartPointer<vtkDICOMImageReader>reader = vtkSmartPointer<vtkDICOMImageReader>::New();
		reader->SetFileName(filename.toStdString().data());
		std::string s = filename.toStdString().data();
		reader->Update();
		imageBlend->AddInputData(reader->GetOutput());
		imageBlend->SetOpacity(0, 0.5);
		imageBlend->Update();
		m_pImageViewer->SetInputData(imageBlend->GetOutput());
		m_pImageViewer->UpdateDisplayExtent();
		m_pImageViewer->SetRenderWindow(ui.qvtkWidget->GetRenderWindow());
		m_pImageViewer->SetRenderer(m_pRenderer);

		m_pRenderer->ResetCamera();
		m_pRenderer->DrawOn();
		ui.qvtkWidget->SetRenderWindow(m_pImageViewer->GetRenderWindow());
		ui.qvtkWidget->GetRenderWindow()->Render();

	}
}

void MainApplication::viewDRR() {
	if (ui.m_ddr_box->isChecked()) 	ui.drr_label->setVisible(true);
	else ui.drr_label->setVisible(false);
}

void MainApplication::viewXRay() {
	if (ui.m_xray_box->isChecked()) {
		ui.xray_label->setVisible(true);
	}
	else {
		ui.xray_label->setVisible(false);
	}
}

