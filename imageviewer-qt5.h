/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QMainWindow>
#ifndef QT_NO_PRINTER
#include <QPrinter>
#endif

#include "fstream"
#include <QLineEdit>

class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
class QTextEdit;
class QVBoxLayout;
class QTabWidget;
class QPushButton;
class QSpinBox;
class QSlider;
class QTableWidget;
class QComboBox;


class ImageViewer : public QMainWindow
{
    Q_OBJECT

private:

    // Beispiel für GUI Elemente
    QWidget*	m_option_panel1;
    QVBoxLayout*    m_option_layout1;

    QWidget*	m_option_panel2;
    QVBoxLayout*    m_option_layout2;

    QWidget*	m_option_panel3;
    QVBoxLayout*    m_option_layout3;

    QWidget* m_option_panel4;
    QVBoxLayout* m_option_layout4;

    QWidget* m_option_panel5;
    QVBoxLayout* m_option_layout5;




    QPushButton* button1;
    QPushButton* button2;
    QPushButton* button3;
    QPushButton* colorHistgoramButton;
    QPushButton* deleteCrossButton;
    QPushButton* buttonVarianz;
    QPushButton* buttonContrast;
    QPushButton* buttonGray;
    QPushButton* buttonHistogram;
    QPushButton* linearFilterButton;
    QPushButton*  prewittButton;
    QPushButton*  cannyEdgeButton;
    QSlider* sliderCross;
    QSlider* slider2;
    QSlider* sliderBright;
    QSlider* sliderBit;
    QSlider *sliderColorBrightness;
    QSlider* kontrastSlider;
    QSlider* sliderKontrastColor;
    QSlider* sliderColorBit;
    QSlider* autoKontrastColorSlider;
    QSpinBox* spinbox1;
    QLabel* labelMiddleBrightness;
    QLabel* labelVarianz;
    QLabel* labelSliderTick;
    QLabel* meinHistogram;
    QLabel* meinHistogramLog;
    QLabel* myHistogramColorLabel;
    QLabel* linearFilterxSliderLabel;
    QLabel* linearFilterySliderLabel;
    QSlider* yLinearFilterSlider;
    QSlider* xLinearFilterSlider;
    QPushButton* buttonGenerateLinearFilterTable;
    QTableWidget* linearFilterTable;
    QPushButton* linearFilterZeroButton;
    QPushButton* linearFilterKonstButton;
    QPushButton* linearFilterReflectionButton;
    QLineEdit* sigmaInput;
    QLineEdit* sigmaInput1;
    QLineEdit* sigmaInputUSM;
    QPushButton*  buttonUSM;
    QLineEdit* aUSM;
    QPushButton * gaussFilterButton;
    QPushButton* sobelButton;


    // hier können weitere GUI Objekte hin wie Buttons Slider etc.


private slots:

    // Beispiel für einen Algorithmus
    void applyCross(int kreuzBreite);
    void convertToGrayscale();
    void averageBrightness();
    void varianz();
    void histogram();
    void helligkeit(int helligkeitswert);
    void kontrast(int konstante);
    void kontrastColor(int konstante);
    void bitDynamik(int bits);
    void autoKontrast(int kontrastWert);
    void createImageBlackCopy();
    void createImageColor();
    void bitDynamikColor(int bits);
    void helligkeitColor(int helligkeitswert);
    void histogramColor();
    void autoKontrastColor(int kontrastWert);
    void generateFilterTable();
    void setLinearXValue(int x);
    void setLinearYValue(int y);
    int getLinearXValue();
    int getLinearYValue();
    void linearFilter();
    int convertYcbcrToRgb(int y, int cb, int cr);
    std::vector<int> convertToYcbcr(QRgb color);
    int getYfromRGB(int color);
    void linearFilterZeroPadding();
    void linearFilterKonstPadding();
    void linearFilterReflectionPadding();
    void doubleDGauss();
    void sobel();
    void cannyEdge();
    void deleteCross();
    void kantePrewitt();
    void USM();
    void histogramLog();




    // hier können weitere als SLOTS definierte Funktionen hin, die auf Knopfdruck etc. aufgerufen werden.


    void open();
    void print();
    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();
    void about();






public:
    ImageViewer();
    bool loadFile(const QString &);
    void updateImageDisplay();

protected:

    void resizeEvent(QResizeEvent * event);

private:

    // in diesen Beiden Methoden sind Änderungen nötig bzw. sie dienen als
    // Vorlage für eigene Methoden.
    void generateControlPanels();


    // Ab hier technische Details die nicht für das Verständnis notwendig sind.
    void startLogging();
    void generateMainGui();
    
    void createActions();
    void createMenus();
    void updateActions();
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    void renewLogging();

    QTabWidget* tabWidget;
    QTextEdit* logBrowser;
    QWidget* centralwidget;
    QLabel* imageLabel;
    QScrollArea* scrollArea;
    double scaleFactor;
    QImage* image;
    QImage* imageCopy;
    QImage* imageGray;

    QImage* myHistogram;
    QImage* myHistogramLog;
    QImage* myHistogramColor;
    QImage* mirrorImage;
    QImage* imageGauss;
    QImage* imageEdgeDir;
    QImage* imageGradient;
    QImage* imageSobel;
    QImage* imageMask;


    std::fstream logFile;

#ifndef QT_NO_PRINTER
    QPrinter printer;
#endif

    QAction *openAct;
    QAction *printAct;
    QAction *exitAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *fitToWindowAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;
};

#endif
