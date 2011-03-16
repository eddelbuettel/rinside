// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4;  tab-width: 8; -*-
//
// Qt usage example for RInside, inspired by the standard 'density
// sliders' example for other GUI toolkits
//
// Copyright (C) 2011  Dirk Eddelbuettel and Romain Francois

#include <QtGui>

#include "qtdensity.h"

QtDensity::QtDensity(RInside & R) : m_R(R)
{
    m_bw = 100;
    m_kernel = 0;
    m_kernelstrings.push_back("gaussian");
    m_kernelstrings.push_back("epanechnikov");
    m_kernelstrings.push_back("rectangular");
    m_kernelstrings.push_back("triangular");
    m_kernelstrings.push_back("cosine");

    m_mixparams["n1"] = "100";
    m_mixparams["n2"] =  "50";
    m_mixparams["m1"] =   "0";
    m_mixparams["m2"] =   "5";
    m_mixparams["sd1"] =  "1";
    m_mixparams["sd2"] =  "0.5";

    m_R["bw"] = m_bw;
    m_tempfile = Rcpp::as<std::string>(m_R.parseEval("tfile <- tempfile()"));
    draw();
    //std::cout << "Used R-assigned tempfile " << m_tempfile << std::endl;

    QWidget *window = new QWidget;
    window->setWindowTitle("Qt and embedded R demo: density estimation");

    QSpinBox *spinBox = new QSpinBox;
    QSlider *slider = new QSlider(Qt::Horizontal);
    spinBox->setRange(5, 200);
    slider->setRange(5, 200);

    QObject::connect(spinBox, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));
    QObject::connect(slider, SIGNAL(valueChanged(int)), spinBox, SLOT(setValue(int)));
    spinBox->setValue(m_bw);

    QObject::connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(getBandwidth(int)));

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
    kernelRadioBox->setMinimumSize(300,140);
    kernelRadioBox->setMaximumSize(300,140);
    kernelRadioBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    kernelRadioBox->setLayout(vbox);

    QButtonGroup *kernelGroup = new QButtonGroup;
    kernelGroup->addButton(radio1, 0);
    kernelGroup->addButton(radio2, 1);
    kernelGroup->addButton(radio3, 2);
    kernelGroup->addButton(radio4, 3);
    kernelGroup->addButton(radio5, 4);
    QObject::connect(kernelGroup, SIGNAL(buttonClicked(int)), this, SLOT(getKernel(int)));

    QLabel *labelN1 = new QLabel("Nobs 1");
    QLabel *labelM1 = new QLabel("Mean 1");
    QLabel *labelSD1 = new QLabel("StdDev 1");
    QLineEdit *entryN1 = new QLineEdit(QString(m_mixparams["n1"].c_str()));
    QLineEdit *entryM1 = new QLineEdit(QString(m_mixparams["m1"].c_str()));
    QLineEdit *entrySD1 = new QLineEdit(QString(m_mixparams["sd1"].c_str()));
    QLabel *labelN2 = new QLabel("Nobs 2");
    QLabel *labelM2 = new QLabel("Mean 2");
    QLabel *labelSD2 = new QLabel("StdDev 2");
    QLineEdit *entryN2 = new QLineEdit( QString(m_mixparams["n2"].c_str()));
    QLineEdit *entryM2 = new QLineEdit(QString(m_mixparams["m2"].c_str()));
    QLineEdit *entrySD2 = new QLineEdit(QString(m_mixparams["sd2"].c_str()));
    QWidget *gridbox = new QWidget();
    QGridLayout *grid = new QGridLayout(gridbox);
    grid->addWidget(labelN1, 0, 0);
    grid->addWidget(labelM1, 0, 1);
    grid->addWidget(labelSD1, 0, 2);
    grid->addWidget(entryN1, 1, 0);
    grid->addWidget(entryM1, 1, 1);
    grid->addWidget(entrySD1, 1, 2);
    grid->addWidget(labelN2, 2, 0);
    grid->addWidget(labelM2, 2, 1);
    grid->addWidget(labelSD2, 2, 2);
    grid->addWidget(entryN2, 3, 0);
    grid->addWidget(entryM2, 3, 1);
    grid->addWidget(entrySD2, 3, 2);
    gridbox->setMinimumSize(300,140);
    gridbox->setMaximumSize(300,140);
    gridbox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QObject::connect(entryN1,  SIGNAL(textEdited(QString)), this, SLOT(getN1(QString)));
    QObject::connect(entryM1,  SIGNAL(textEdited(QString)), this, SLOT(getM1(QString)));
    QObject::connect(entrySD1, SIGNAL(textEdited(QString)), this, SLOT(getSD1(QString)));
    QObject::connect(entryN2,  SIGNAL(textEdited(QString)), this, SLOT(getN2(QString)));
    QObject::connect(entryM2,  SIGNAL(textEdited(QString)), this, SLOT(getM2(QString)));
    QObject::connect(entrySD2, SIGNAL(textEdited(QString)), this, SLOT(getSD2(QString)));

    imageLabel = new QLabel;
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    image = new QImage(QString(m_tempfile.c_str()));

    plot();			// now that we have an image, we can diplay

    QVBoxLayout *outer = new QVBoxLayout;
    QHBoxLayout *upperlayout = new QHBoxLayout;
    upperlayout->addWidget(spinBox);
    upperlayout->addWidget(slider);

    QHBoxLayout *middlelayout = new QHBoxLayout;
    middlelayout->addWidget(kernelRadioBox);
    middlelayout->addWidget(gridbox);

    QHBoxLayout *lowerlayout = new QHBoxLayout;
    lowerlayout->addWidget(imageLabel);

    outer->addLayout(upperlayout);
    outer->addLayout(middlelayout);
    outer->addLayout(lowerlayout);
    window->setLayout(outer);
    window->show();
    window->resize(650, 750);

}

QtDensity::~QtDensity() {
    std::cerr << "Dtor" << std::endl;
    m_R.parseEvalQ("q('no')");	// we never needed that before -- but maybe the Qt threads get in the way
    std::cerr << "Dtor R stopped" << std::endl; // not reached !!
}

void QtDensity::plot(void) {
    m_R["bw"] = m_bw;
    m_R["kernel"] = m_kernelstrings[m_kernel]; // that passes the string to R
    std::string cmd1 = "png(tfile,600,600); plot(density(y, bw=bw/100, kernel=kernel), xlim=range(y)+c(-2,2), main=\"Kernel: ";
    std::string cmd2 = "\"); points(y, rep(0, length(y)), pch=16, col=rgb(0,0,0,1/4));  dev.off()";
    std::string cmd = cmd1 + m_kernelstrings[m_kernel] + cmd2;
    m_R.parseEvalQ(cmd);
    image->load(QString(m_tempfile.c_str()));
    imageLabel->setPixmap(QPixmap::fromImage(*image));
}

void QtDensity::draw(void) {
    m_R["n1"] = m_mixparams["n1"];
    m_R["m1"] = m_mixparams["m1"];
    m_R["sd1"] = m_mixparams["sd1"];
    m_R["n2"] = m_mixparams["n2"];
    m_R["m2"] = m_mixparams["m2"];
    m_R["sd2"] = m_mixparams["sd2"];
    m_R.parseEvalQ("y <- c(rnorm(as.numeric(n1), as.numeric(m1), as.numeric(sd1)), rnorm(as.numeric(n2), as.numeric(m2), as.numeric(sd2)));");
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

void QtDensity::getN1(QString txt) {
    if (txt != "") {
	m_mixparams["n1"] = txt.toStdString();
	draw();
	plot();
    }
}

void QtDensity::getM1(QString txt) {
    if (txt != "") {
	m_mixparams["m1"] = txt.toStdString();
	draw();
	plot();
    }
}

void QtDensity::getSD1(QString txt) {
    if (txt != "") {
	m_mixparams["sd1"] = txt.toStdString();
	draw();
	plot();
    }
}

void QtDensity::getN2(QString txt) {
    if (txt != "") {
	m_mixparams["n2"] = txt.toStdString();
	draw();
	plot();
    }
}

void QtDensity::getM2(QString txt) {
    if (txt != "") {
	m_mixparams["m2"] = txt.toStdString();
	draw();
	plot();
    }
}

void QtDensity::getSD2(QString txt) {
    m_mixparams["sd2"] = txt.toStdString();
    draw();
    plot();
}
