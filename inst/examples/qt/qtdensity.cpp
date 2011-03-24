// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4;  tab-width: 8; -*-
//
// Qt usage example for RInside, inspired by the standard 'density
// sliders' example for other GUI toolkits
//
// Copyright (C) 2011  Dirk Eddelbuettel and Romain Francois

#include <QtGui>
//#include <QSvgWidget>

#include "qtdensity.h"

QtDensity::QtDensity(RInside & R) : m_R(R)
{
    m_bw = 100;			// initial bandwidth, will be scaled by 100 so 1.0
    m_kernel = 0;		// initial kernel: gaussian
    m_cmd = "c(rnorm(100,0,1), rnorm(50,5,1))"; // simple mixture

    m_R["bw"] = m_bw;		// pass bandwidth to R, and have R compute a temp.file name
    m_tempfile = Rcpp::as<std::string>(m_R.parseEval("tfile <- tempfile()"));
    //m_R.parseEvalQ("library(cairoDevice)");

    setupDisplay();
}

void QtDensity::setupDisplay(void)  {
    QWidget *window = new QWidget;
    window->setWindowTitle("Qt and RInside demo: density estimation");

    QSpinBox *spinBox = new QSpinBox;
    QSlider *slider = new QSlider(Qt::Horizontal);
    spinBox->setRange(5, 200);
    slider->setRange(5, 200);
    QObject::connect(spinBox, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));
    QObject::connect(slider, SIGNAL(valueChanged(int)), spinBox, SLOT(setValue(int)));
    spinBox->setValue(m_bw);
    QObject::connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(getBandwidth(int)));

    QLabel *cmdLabel = new QLabel("R command for random data creation");
    QLineEdit *cmdEntry = new QLineEdit(QString(m_cmd.c_str()));
    QObject::connect(cmdEntry,  SIGNAL(textEdited(QString)), this, SLOT(getRandomDataCmd(QString)));
    QObject::connect(cmdEntry,  SIGNAL(editingFinished()), this, SLOT(runRandomDataCmd()));

    QGroupBox *kernelRadioBox = new QGroupBox("Density Estimation kernel");
    QRadioButton *radio1 = new QRadioButton("&Gaussian");
    QRadioButton *radio2 = new QRadioButton("&Epanechnikov");
    QRadioButton *radio3 = new QRadioButton("&Rectangular");
    QRadioButton *radio4 = new QRadioButton("&Triangular");
    QRadioButton *radio5 = new QRadioButton("&Cosine");
    radio1->setChecked(true);
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(radio1);
    vbox->addWidget(radio2);
    vbox->addWidget(radio3);
    vbox->addWidget(radio4);
    vbox->addWidget(radio5);
    kernelRadioBox->setMinimumSize(260,140);
    kernelRadioBox->setMaximumSize(260,140);
    kernelRadioBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    kernelRadioBox->setLayout(vbox);

    QButtonGroup *kernelGroup = new QButtonGroup;
    kernelGroup->addButton(radio1, 0);
    kernelGroup->addButton(radio2, 1);
    kernelGroup->addButton(radio3, 2);
    kernelGroup->addButton(radio4, 3);
    kernelGroup->addButton(radio5, 4);
    QObject::connect(kernelGroup, SIGNAL(buttonClicked(int)), this, SLOT(getKernel(int)));

    imageLabel = new QLabel;
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    image = new QImage(QString(m_tempfile.c_str()));
    runRandomDataCmd();
    plot();			// now that we have an image, we can diplay

    QGroupBox *estimationBox = new QGroupBox("Density estimation bandwidth (scaled by 100)");
    QHBoxLayout *spinners = new QHBoxLayout;
    spinners->addWidget(spinBox);
    spinners->addWidget(slider);
    QVBoxLayout *topright = new QVBoxLayout;
    topright->addLayout(spinners);
    topright->addWidget(cmdLabel);
    topright->addWidget(cmdEntry);
    estimationBox->setMinimumSize(360,140);
    estimationBox->setMaximumSize(360,140);
    estimationBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    estimationBox->setLayout(topright);
    QHBoxLayout *upperlayout = new QHBoxLayout;
    upperlayout->addWidget(kernelRadioBox);
    upperlayout->addWidget(estimationBox);

    QHBoxLayout *lowerlayout = new QHBoxLayout;
    lowerlayout->addWidget(imageLabel);

    //QSvgWidget *svg = new QSvgWidget("/tmp/foo.svg");
    //QHBoxLayout *svglayout = new QHBoxLayout;
    //svglayout->addWidget(svg);

    QVBoxLayout *outer = new QVBoxLayout;
    outer->addLayout(upperlayout);
    outer->addLayout(lowerlayout);
    //outer->addLayout(svglayout);
    window->setLayout(outer);
    window->show();
    window->resize(625, 725);
}

QtDensity::~QtDensity() {
    //std::cerr << "Dtor" << std::endl;
    m_R.parseEvalQ("q('no')");	// we never needed that before -- but maybe the Qt threads get in the way
    //std::cerr << "Dtor R stopped" << std::endl; // never reached ?
}

void QtDensity::plot(void) {
    const char *kernelstrings[] = { "gaussian", "epanechnikov", "rectangular", "triangular", "cosine" };
    m_R["bw"] = m_bw;
    m_R["kernel"] = kernelstrings[m_kernel]; // that passes the string to R
    std::string cmd1 = "png(tfile,600,600); plot(density(y, bw=bw/100, kernel=kernel), xlim=range(y)+c(-2,2), main=\"Kernel: ";
    //std::string cmd1 = "Cairo_svg(tfile,6,6); plot(density(y, bw=bw/100, kernel=kernel), xlim=range(y)+c(-2,2), main=\"Kernel: ";
    std::string cmd2 = "\"); points(y, rep(0, length(y)), pch=16, col=rgb(0,0,0,1/4));  dev.off()";
    std::string cmd = cmd1 + kernelstrings[m_kernel] + cmd2;
    m_R.parseEvalQ(cmd);
    image->load(QString(m_tempfile.c_str()));
    imageLabel->setPixmap(QPixmap::fromImage(*image));
}

void QtDensity::getBandwidth(int bw) {
    if (bw != m_bw) {
	m_bw = bw;
	//std::cout << "Bandwidth now " << m_bw << std::endl;
	plot();
    }
}

void QtDensity::getKernel(int kernel) {
    if (kernel != m_kernel) {
	m_kernel = kernel;
	//std::cout << "Kernel now " << kernel << std::endl;
	plot();
    }
}

void QtDensity::getRandomDataCmd(QString txt) {
    //std::cout << "Cmd: " << txt.toStdString() << std::endl;
    m_cmd = txt.toStdString();
}

void QtDensity::runRandomDataCmd(void) {
    std::string cmd = "y <- " + m_cmd;
    //std::cout << "Running: " << cmd << std::endl;
    m_R.parseEvalQ(cmd);
    plot();
}
