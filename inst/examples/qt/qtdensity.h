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
    void getRandomDataCmd(QString txt);
    void runRandomDataCmd(void);

private:
    void setupDisplay(void);	// standard GUI boilderplate of arranging things
    void plot(void);		// run a density plot in R and update the display

    qlabel *imageLabel;
    QImage *image;

    RInside & m_R;		// reference to the R instance passed to constructor
    std::string m_tempfile;	// name of file used by R for plots
    int m_bw, m_kernel;		// parameters used to estimate the density
    std::string m_cmd;		// random draw command string
};

#endif
