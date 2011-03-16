// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4;  tab-width: 8; -*-
//
// Qt usage example for RInside, inspired by the standard 'density
// sliders' example for other GUI toolkits
//
// Copyright (C) 2011  Dirk Eddelbuettel and Romain Francois

#ifndef QTDENSITY_H
#define QTDENSITY_H

#include <RInside.h>

#include <QMainWindow>
#include <QHBoxLayout>
#include <QSlider>
#include <QSpinBox>
#include <QLabel>

class QtDensity : public QMainWindow
{
    Q_OBJECT

public:
    QtDensity(RInside & R);
    ~QtDensity();

private slots:
    void getBandwidth(int bw);
    void getKernel(int kernel);
    void getN1(QString txt);
    void getM1(QString txt);
    void getSD1(QString txt);
    void getN2(QString txt);
    void getM2(QString txt);
    void getSD2(QString txt);

private:
    void plot(void);
    void draw(void);

    QLabel *imageLabel;
    QImage *image;

    RInside & m_R;		// reference to the R instance passed to constructor
    std::string m_tempfile;	// name of file used by R for plots
    int m_bw, m_kernel;
    std::vector< std::string > m_kernelstrings;
    std::map<std::string, std::string> m_mixparams; 	// simple map for n1,n2,m1,m2,sd1,sd2 
};

#endif
