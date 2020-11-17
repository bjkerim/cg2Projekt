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

#include <QtWidgets>
#ifndef QT_NO_PRINTER
#include <QPrintDialog>
#endif
#include<iostream>


#include "imageviewer-qt5.h"

ImageViewer::ImageViewer()
{

    image=NULL;
    imageGray = NULL;
    resize(1600, 600);

    startLogging();

    generateMainGui();
    renewLogging();

    generateControlPanels();
    createActions();
    createMenus();

    resize(QGuiApplication::primaryScreen()->availableSize() * 0.85 );

}


void ImageViewer::createImageColor() {

    imageCopy = new QImage(*image);
}


void ImageViewer::createImageBlackCopy() {

    imageCopy = new QImage(*imageGray);
}



void ImageViewer::applyCross(int kreuzBreite)

{
    imageCopy = new QImage(*image);
    if(image!=NULL)
    {

        for(int i=0;i<((kreuzBreite*std::min(image->width(), image->height())/ 100) / 2);i++)
        {

            imageCopy->setPixelColor(image->width()/2+i,image->height()/2,QColor(255,0,0,0));
            imageCopy->setPixelColor(image->width()/2-i,image->height()/2,QColor(255,0,0,0));
            imageCopy->setPixelColor(image->width()/2,image->height()/2+i,QColor(255,0,0,0));
            imageCopy->setPixelColor(image->width()/2,image->height()/2-i,QColor(255,0,0,0));


        }
        imageLabel->setPixmap(QPixmap::fromImage(*imageCopy));
        logFile << ((kreuzBreite*std::min(image->width(), image->height())/ 100) / 2)<< std::endl;

    }

    logFile << "applyCross algorithm applied " << std::endl;




    renewLogging();
}


void ImageViewer::convertToGrayscale(){

    if(image != NULL){
        int w = image->width();
        int h = image->height();
        imageGray = new QImage(*image);


        for (int i = 0; i <= w-1; i++){
            for(int j = 0; j <= h-1; j++ ){
                QRgb color = image->pixel(i,j);
                int gray = ((qRed(color) *0.299) + (qGreen(color)* 0.587) + (qBlue(color)*0.114));
                imageGray->setPixel(i, j, qRgb(gray,gray,gray));
            }
        }
    }
    imageLabel->setPixmap(QPixmap::fromImage(*imageGray));
    logFile << "convertToGrayscale algorithm applied " << std::endl;

    renewLogging();
    //    if(imageGray != NULL){
    //        delete imageGray;
    //        imageGray = NULL;
    //    }

    createImageBlackCopy();
}

void ImageViewer::averageBrightness(){

    if(image!=NULL){


        int w = image->width();
        int h = image->height();

        int middleBrightness;
        long brightnessAddition = 0;
        long pixelCount = w*h;



        for (int i = 0; i < w; i++){
            for(int j = 0; j < h; j++ ){
                brightnessAddition += qRed(imageCopy->pixel(i,j));
            }
        }
        middleBrightness = (brightnessAddition/pixelCount);

        labelMiddleBrightness->setText(QString::number(middleBrightness));

        logFile << "middleBrightness algorithm applied " << std::endl;
        renewLogging();

    }}

void ImageViewer::varianz(){
    imageCopy = new QImage(*imageGray);
    if(imageCopy != NULL){

        int w = image->width();
        int h = image->height();

        int averageBrightness;
        int brightnessSumAverage = 0;
        int pixelCount = w*h;

        long brightnessSum =0;
        int tempRes = 0;


        for (int i = 0; i < w; i++){
            for(int j = 0; j < h; j++ ){
                brightnessSumAverage += qRed(imageCopy->pixel(i,j));
            }
        }

        averageBrightness = (brightnessSumAverage/pixelCount);


        for (int i = 0; i < w; i++){
            for(int j = 0; j < h; j++ ){
                brightnessSum = qRed(image->pixel(i,j)) - averageBrightness;
                tempRes += (brightnessSum*brightnessSum);

            }
        }

        int myVarianz = tempRes/pixelCount;

        brightnessSum = brightnessSum*brightnessSum;
        labelVarianz->setText(QString::number(brightnessSum));
        logFile << "varianz algorithm applied " << std::endl;
        renewLogging();
    }
}

void ImageViewer::bitDynamik(int bits){
    if(image != NULL){

        int w = image->width();
        int h = image->height();
        *imageCopy = *imageGray;


        int bit = bits;
        int stufen = (1<<bit);
        int ausGrenze = 256/stufen;
        int pixelBereich;
        int neuWert;

        for (int i = 0; i < w; i++){
            for(int j = 0; j < h; j++ ){
                pixelBereich = ((qRed(imageCopy->pixel(i,j)))/ausGrenze)+1;
                neuWert = (ausGrenze * pixelBereich)-1;
                imageCopy->setPixel(i, j, qRgb(neuWert,neuWert,neuWert));


            }
        }
        imageLabel->setPixmap(QPixmap::fromImage(*imageCopy));

    }

}




void ImageViewer::bitDynamikColor(int bits){
    if(image != NULL){

        int w = image->width();
        int h = image->height();
        *imageCopy = *image;


        int bit = bits;
        int stufen = (1<<bit);
        int ausGrenze = 256/stufen;
        int pixelBereich;
        int yNew;

        for (int i = 0; i <= w-1; i++){
            for(int j = 0; j <= h-1; j++ ){
                QRgb color = image->pixel(i,j);
                int y = ((qRed(color) *0.299) + (qGreen(color)* 0.587) + (qBlue(color)*0.114));
                int cb = 128+((qRed(color) * -0.169) + (qGreen(color)* -0.331) + (qBlue(color)*0.5));
                int cr = 128+((qRed(color) * 0.5) + (qGreen(color)* -0.419) + (qBlue(color)*-0.081));

                pixelBereich = (y/ausGrenze)+1;
                yNew = (ausGrenze * pixelBereich)-1;

                int newR = ((yNew-16)*1.164) + ((cr-128)*1.596);
                int newG = ((yNew-16)*1.164) + ((cb-128)*(-0.392)) + ((cr-128)*(-0.813));
                int newB =  ((yNew-16)*1.164) + ((cb-128)*2.017);

                if(newR > 255) {
                    newR = 255;
                }
                else if (newR < 0) {
                    newR = 0;
                }
                if(newG > 255) {
                    newG = 255;
                }
                else if (newG < 0) {
                    newG = 0;
                }
                if(newB > 255) {
                    newB = 255;
                }
                else if (newB < 0) {
                    newB = 0;
                }

                imageCopy->setPixel(i, j, qRgb(newR,newG,newB));

            }


            imageLabel->setPixmap(QPixmap::fromImage(*imageCopy));

        }

    }

}

void ImageViewer::histogram() {



    QPixmap histogramMap(256,256);
    histogramMap.fill(QColor("white"));
    myHistogram = new QImage(256, 100, QImage::Format_RGB32);
    myHistogram->fill(Qt::black);


    int myArray[256]= {0};
    int prozentArray[256] = {0};
    int myValue = 0;


    for (int i = 0; i<image->width() ;i++) {
        for (int j = 0; j<image->height(); j++){
            myValue = qRed(imageCopy->pixel(i,j));
            myArray[myValue] += 1;

        }
    }
    int greatestNumber = 0;

    for(int c = 0; c<256; c++){
        if (myArray[c] > greatestNumber){
            greatestNumber = myArray[c];
            // logFile << "Position: " << c << " " << "Wert: " <<myArray[c]<< std::endl;

        }}

    int endWert[256] = {0};
    for (int k = 0; k<256 ;k++){
        endWert[k] = (myArray[k]/(greatestNumber/100));
        //*logFile << "Position: " << k << " " << "Wert: " <<endWert[k]<< std::endl;
    }

    for (int z = 0; z<256; z++){
        for (int h=0; h<(100-endWert[z]); h++){
            myHistogram->setPixelColor(z,h, Qt::white);
        }

    }
    meinHistogram->setPixmap(QPixmap::fromImage(*myHistogram));
    meinHistogram->show();

}
void ImageViewer::helligkeit(int helligkeitswert) {

    *imageCopy = *imageGray;

    int w = image->width();
    int h = image->height();


    for (int i=0; i<w; i++){
        for (int j=0; j<h; j++){

            QRgb myValue = imageGray->pixel(i,j);

            int newColor = ((qRed(myValue)*0.299) + (qGreen(myValue)*0.587) + (qBlue(myValue)*0.114)) + helligkeitswert; //* Gewichtung miteinberechnen

            if (newColor < 0){
                newColor = 0;}
            else if (newColor > 255 ){
                newColor = 255; }

            QRgb color = qRgb(newColor, newColor, newColor);
            imageCopy->setPixel(i,j,color);

        }
    }

    imageLabel->setPixmap(QPixmap::fromImage(*imageCopy));

}



void ImageViewer::helligkeitColor(int helligkeitswert) {

    *imageCopy = *image;

    int w = image->width();
    int h = image->height();


    for (int i = 0; i <= w-1; i++){
        for(int j = 0; j <= h-1; j++ ){
            QRgb color = imageCopy->pixel(i,j);

            int y = ((qRed(color) *0.299) + (qGreen(color)* 0.587) + (qBlue(color)*0.114));
            int cb = 128+((qRed(color) * -0.169) + (qGreen(color)* -0.331) + (qBlue(color)*0.5));
            int cr = 128+((qRed(color) * 0.5) + (qGreen(color)* -0.419) + (qBlue(color)*-0.081));

            int newR = (((y-16)*1.164) + ((cr-128)*1.596)) +  helligkeitswert;
            int newG = (((y-16)*1.164) + ((cb-128)*(-0.392)) + ((cr-128)*(-0.813))) +  helligkeitswert ;
            int newB =  (((y-16)*1.164) + ((cb-128)*2.017)) +  helligkeitswert ;

            if(newR > 255) {
                newR = 255;
            }
            else if (newR < 0) {
                newR = 0;
            }
            if(newG > 255) {
                newG = 255;
            }
            else if (newG < 0) {
                newG = 0;
            }
            if(newB > 255) {
                newB = 255;
            }
            else if (newB < 0) {
                newB = 0;
            }

            QColor newColor = qRgb(newR,newG,newB);

            imageCopy->setPixelColor(i, j, newColor);

        }

        imageLabel->setPixmap(QPixmap::fromImage(*imageCopy));

    }

}


void ImageViewer::kontrast(int konstante) {

    convertToGrayscale();

    int w = image->width();
    int h = image->height();
    int low = 0;
    int high = 0;
    int mid = 0;
    low = qRed(image->pixel(0,0));
    high = qRed(image->pixel(0,0));
    for (int i=0; i <w; i++){
        for (int j = 0; j<h; j++){

            int aktuellerWert = qRed(image->pixel(i,j));
            if (aktuellerWert < low){
                low = aktuellerWert;
            }
            if (aktuellerWert > high){
                low = aktuellerWert;

            }
        }
    }    mid = (high+low)/2;


    for (int i=0; i <w; i++){
        for (int j = 0; j<h; j++){
            int newPixelValue = 0;
            if (konstante > 0) {
                newPixelValue = (((qRed(image->pixel(i,j)) - mid) * konstante)) + 0.5;
            }

            else if (konstante < 0) {

                newPixelValue = (((qRed(image->pixel(i,j)) - mid) / std::abs(konstante))) + 0.5;
            }

            else if (konstante = 0) {
                imageLabel->setPixmap(QPixmap::fromImage(*imageGray));

            }
            newPixelValue = newPixelValue + mid;


            if(newPixelValue > 255) {
                newPixelValue = 255;
            }

            else if (newPixelValue < 0) {
                newPixelValue = 0;
            }

            imageCopy->setPixel(i, j, qRgb(newPixelValue,newPixelValue,newPixelValue));

        }
    }

    imageLabel->setPixmap(QPixmap::fromImage(*imageCopy));

    /* logFile << "High: " << high << " " << "low: " <<low<< std::endl;*/
    renewLogging();
}

void ImageViewer::autoKontrast(int kontrastWert) {


    *imageCopy = *image;
    int w = image->width();
    int h = image->height();

    int neueHelligkeit = 0;
    int aMin = 0;
    int aMax = 255;


    float kontrastProzent = kontrastWert/100.0;
    int grenzeLow = kontrastProzent*(h*w); //  5242
    int grenzeHigh = (1-kontrastProzent)*(h*w); // 256901

    int aStrichLow = qRed(imageCopy->pixel(0,0));
    int aStrichHigh = qRed(imageCopy->pixel(0,0));;

    int histogram[256]= {0};
    int kHistogram[256] = {0};

    int aLow = 300;
    int aHigh = 0;

    /* Suche hellsten und dunkelsten Wert raus */
    for (int i=0; i<w; i++){
        for (int j=0; j<h; j++){
            if(qRed(imageCopy->pixel(i,j)) < aLow){
                aLow = qRed(imageCopy->pixel(i,j));
            }
            else if (qRed(imageCopy->pixel(i,j)) > aHigh){
                aHigh = qRed(imageCopy->pixel(i,j));
            }
        }
    }

    //    /* Create Array */
    for (int i = 0; i<w ;i++) {
        for (int j = 0; j<h; j++){
            int myValue = qRed(imageCopy->pixel(i,j));
            histogram[myValue] += 1;
        }
    }

    kHistogram[0] = histogram[0];
    for(int i=1; i<256; i++){

        kHistogram[i] = kHistogram[i-1] + histogram[i];
    }

    for (int i=0; i<256; i++){
        if(kHistogram[i] >= grenzeLow && i<aStrichLow){
            aStrichLow = i;
        }

        else if (kHistogram[i]<= grenzeHigh && i > aStrichHigh){
            aStrichHigh = i;
        }
    }

    for (int i=0; i<w; i++){
        for (int j=0; j<h; j++){
            neueHelligkeit = qRed(imageCopy->pixel(i,j));

            if (neueHelligkeit <= aStrichLow){
                neueHelligkeit = aMin;
            }

            else if ( neueHelligkeit >= aStrichHigh ){
                neueHelligkeit = aMax; }
            else {
                neueHelligkeit =  aMin + (neueHelligkeit - aStrichLow) * ((aMax-aMin)/(aStrichHigh-aStrichLow));

            }

            imageCopy->setPixel(i,j,qRgb(neueHelligkeit,neueHelligkeit,neueHelligkeit));

        }

    }    imageLabel->setPixmap(QPixmap::fromImage(*imageCopy));
    renewLogging();
}


//void ImageViewer::convertToYcbcr() {
//    int yNew = 0;
//    int h = image->height();
//    int w = image->width();

//    for (int i = 0; i <= w-1; i++){
//        for(int j = 0; j <= h-1; j++ ){
//            QRgb color = image->pixel(i,j);
//            int y = ((qRed(color) *0.299) + (qGreen(color)* 0.587) + (qBlue(color)*0.114));
//            int cb = 128+((qRed(color) * -0.169) + (qGreen(color)* -0.331) + (qBlue(color)*0.5));
//            int cr = 128+((qRed(color) * 0.5) + (qGreen(color)* -0.419) + (qBlue(color)*-0.081));
//        }


//    }





    /****************************************************************************************
*
*  mit dieser Methode können sie sich pro Aufgabe ein  Tab anlegen, in der die Ein-
*  stellungen per Slider, Button, Checkbox etc. gemacht werden und die zu implemen-
*  tierenden Algorithmen gestatet werden.
*
*****************************************************************************************/




    void ImageViewer::generateControlPanels()
    {
        //  ******************************    Aufgabe 1    ******************************  //

        m_option_panel1 = new QWidget();
        m_option_layout1 = new QVBoxLayout();
        m_option_panel1->setLayout(m_option_layout1);

        sliderCross = new QSlider(Qt::Orientation::Horizontal, this);
        sliderCross->setRange(1,100);
        sliderCross->setToolTip("Cross Slider");
        sliderCross->setTickPosition(QSlider::TicksBelow);
        sliderCross->setTickInterval(1);

        QObject::connect(sliderCross, SIGNAL(valueChanged(int)),this,SLOT(applyCross(int)));

        m_option_layout1->addWidget(sliderCross);

        tabWidget->addTab(m_option_panel1,"Aufgabenblatt 1");


        //  ******************************    Aufgabe 2    ******************************  //

        m_option_panel2 = new QWidget();
        m_option_layout2 = new QVBoxLayout();
        m_option_panel2->setLayout(m_option_layout2);



        buttonMittlereHelligkeit = new QPushButton();
        buttonMittlereHelligkeit ->setText("Mittlere Helligkeit");
        QObject::connect(buttonMittlereHelligkeit, SIGNAL (clicked()), this, SLOT (averageBrightness()));

        buttonVarianz = new QPushButton();
        buttonVarianz ->setText("Varianz");
        QObject::connect(buttonVarianz, SIGNAL (clicked()), this, SLOT (varianz()));


        labelMiddleBrightness = new QLabel();
        labelVarianz = new QLabel();

        buttonGray = new QPushButton();
        buttonGray->setText("Graustufen");
        QObject::connect(buttonGray, SIGNAL (clicked()), this, SLOT (convertToGrayscale()));

        buttonHistogram = new QPushButton();
        buttonHistogram->setText("Draw Histogram");
        QObject::connect(buttonHistogram, SIGNAL (clicked()), this, SLOT (histogram()));

        sliderBit = new QSlider(Qt::Orientation::Horizontal, this);
        sliderBit->setRange(1,8);
        sliderBit->setToolTip("Bit Slider");
        sliderBit->setTickPosition(QSlider::TicksBelow);
        sliderBit->setTickInterval(1);


        QObject::connect(sliderBit, SIGNAL(valueChanged(int)),this,SLOT(bitDynamik(int)));
        meinHistogram = new QLabel(this);


        QSlider *sliderBright = new QSlider(Qt::Horizontal,0);
        sliderBright->setRange(-255,255);
        sliderBright->setToolTip("Helligkeitsslider");
        sliderBright->setTickPosition(QSlider::TicksBelow);
        sliderBright->setTickInterval(50);
        connect(sliderBright, SIGNAL(valueChanged(int)),this, SLOT(helligkeit(int)));


        QSlider *sliderKontrast = new QSlider(Qt::Horizontal,0);
        sliderKontrast->setRange(-10,10);
        sliderKontrast->setTickPosition(QSlider::TicksBelow);
        sliderKontrast->setTickInterval(1);
        sliderKontrast->setToolTip("Kontrastslider");
        connect(sliderKontrast, SIGNAL(valueChanged(int)),this, SLOT(kontrast(int)));


        QSlider *autoKontrastSlider = new QSlider(Qt::Horizontal,0);
        autoKontrastSlider->setRange(1,100);
        autoKontrastSlider->setTickPosition(QSlider::TicksBelow);
        autoKontrastSlider->setTickInterval(1);
        autoKontrastSlider->setToolTip("Autokontrastslider");
        connect(autoKontrastSlider, SIGNAL(valueChanged(int)),this, SLOT(autoKontrast(int)));



        m_option_layout2->addWidget(buttonGray);
        m_option_layout2->addWidget(sliderBit);
        //m_option_layout2->addWidget(labelSliderTick);
        m_option_layout2->addWidget(buttonHistogram);
        m_option_layout2->addWidget(meinHistogram);
        m_option_layout2->addWidget(sliderBright);
        m_option_layout2->addWidget(buttonMittlereHelligkeit);
        m_option_layout2->addWidget(labelMiddleBrightness);
        m_option_layout2->addWidget(buttonVarianz);
        m_option_layout2->addWidget(labelVarianz);
        m_option_layout2->addWidget(sliderKontrast);
        m_option_layout2->addWidget(autoKontrastSlider);

        tabWidget->addTab(m_option_panel2,"Aufgabenblatt 2");
        tabWidget->show();




        //  ******************************    Aufgabe 3    ******************************  //

        m_option_panel3 = new QWidget();
        m_option_layout3 = new QVBoxLayout();
        m_option_panel3 ->setLayout(m_option_layout3);

        QSlider *sliderColorBit = new QSlider(Qt::Horizontal,0);
        sliderColorBit->setRange(1,8);
        sliderColorBit->setTickPosition(QSlider::TicksBelow);
        sliderColorBit->setTickInterval(1);
        sliderColorBit->setToolTip("Autokontrastslider");
        connect(sliderColorBit, SIGNAL(valueChanged(int)),this, SLOT(bitDynamikColor(int)));


        QSlider *sliderColorBrightness = new QSlider(Qt::Horizontal,0);
        sliderColorBrightness->setRange(-255,255);
        sliderColorBrightness->setTickPosition(QSlider::TicksBelow);
        sliderColorBrightness->setTickInterval(10);
        sliderColorBrightness->setToolTip("Helligkeitsslider");
        connect(sliderColorBrightness, SIGNAL(valueChanged(int)),this, SLOT(helligkeitColor(int)));





        m_option_layout3->addWidget(sliderColorBit);
        m_option_layout3->addWidget(sliderColorBrightness);

        tabWidget->addTab(m_option_panel3,"Aufgabenblatt 3");



        // Hinweis: Es bietet sich an pro Aufgabe jeweils einen solchen Tab zu erstellen

    }

    /****************************************************************************************
*
*   ab hier kommen technische Details, die nicht notwenig für das Verständnis und die
*   Bearbeitung sind.
*
*
*****************************************************************************************/



    void ImageViewer::startLogging()
    {
        //LogFile
        logFile.open("log.txt", std::ios::out);
        logFile << "Logging: \n" << std::endl;
    }

    void ImageViewer::renewLogging()
    {
        QFile file("log.txt"); // Create a file handle for the file named
        QString line;
        file.open(QIODevice::ReadOnly); // Open the file

        QTextStream stream( &file ); // Set the stream to read from myFile
        logBrowser->clear();
        while(!stream.atEnd()){

            line = stream.readLine(); // this reads a line (QString) from the file
            logBrowser->append(line);
        }
    }


    void ImageViewer::resizeEvent(QResizeEvent * event)
    {
        QMainWindow::resizeEvent(event);
        centralwidget->setMinimumWidth(width());
        centralwidget->setMinimumHeight(height());
        centralwidget->setMaximumWidth(width());
        centralwidget->setMaximumHeight(height());
        logBrowser->setMinimumWidth(width()-40);
        logBrowser->setMaximumWidth(width()-40);
    }

    void ImageViewer::updateImageDisplay()
    {

        imageLabel->setPixmap(QPixmap::fromImage(*image));
    }


    void ImageViewer::generateMainGui()
    {
        /* Tab widget */
        tabWidget = new QTabWidget(this);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));



        /* Center widget */
        centralwidget = new QWidget(this);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        //centralwidget->setFixedSize(200,200);
        //setCentralWidget(centralwidget);

        imageLabel = new QLabel;
        imageLabel->setBackgroundRole(QPalette::Base);
        imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        imageLabel->setScaledContents(true);


        /* Center widget */
        scrollArea = new QScrollArea;
        scrollArea->setBackgroundRole(QPalette::Dark);
        scrollArea->setWidget(imageLabel);


        setCentralWidget(scrollArea);

        /* HBox layout */
        QGridLayout* gLayout = new QGridLayout(centralwidget);
        gLayout->setObjectName(QStringLiteral("hboxLayout"));
        gLayout->addWidget(new QLabel(),1,1);
        gLayout->setVerticalSpacing(50);
        gLayout->addWidget(tabWidget,2,1);
        gLayout->addWidget(scrollArea,2,2);

        logBrowser= new QTextEdit(this);
        logBrowser->setMinimumHeight(100);
        logBrowser->setMaximumHeight(200);
        logBrowser->setMinimumWidth(width());
        logBrowser->setMaximumWidth(width());
        gLayout->addWidget(logBrowser,3,1,1,2);
        gLayout->setVerticalSpacing(50);
    }


    bool ImageViewer::loadFile(const QString &fileName)
    {
        if(image!=NULL)
        {
            delete image;
            image=NULL;
        }

        image = new QImage(fileName);




        if (image->isNull()) {
            QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                     tr("Cannot load %1.").arg(QDir::toNativeSeparators(fileName)));
            setWindowFilePath(QString());
            imageLabel->setPixmap(QPixmap());
            imageLabel->adjustSize();
            return false;
        }

        scaleFactor = 1.0;


        updateImageDisplay();

        printAct->setEnabled(true);
        fitToWindowAct->setEnabled(true);
        updateActions();

        if (!fitToWindowAct->isChecked())
            imageLabel->adjustSize();

        setWindowFilePath(fileName);
        logFile << "geladen: " << fileName.toStdString().c_str()  << std::endl;
        createImageColor();
        renewLogging();



        return true;
    }




    void ImageViewer::open()
    {
        QStringList mimeTypeFilters;
        foreach (const QByteArray &mimeTypeName, QImageReader::supportedMimeTypes())
            mimeTypeFilters.append(mimeTypeName);
        mimeTypeFilters.sort();
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        QFileDialog dialog(this, tr("Open File"),
                           picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.first());
        dialog.setAcceptMode(QFileDialog::AcceptOpen);
        dialog.setMimeTypeFilters(mimeTypeFilters);
        dialog.selectMimeTypeFilter("image/png");

        while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
    }

    void ImageViewer::print()
    {
        Q_ASSERT(imageLabel->pixmap());
#if !defined(QT_NO_PRINTER) && !defined(QT_NO_PRINTDIALOG)
        QPrintDialog dialog(&printer, this);
        if (dialog.exec()) {
            QPainter painter(&printer);
            QRect rect = painter.viewport();
            QSize size = imageLabel->pixmap()->size();
            size.scale(rect.size(), Qt::KeepAspectRatio);
            painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
            painter.setWindow(imageLabel->pixmap()->rect());
            painter.drawPixmap(0, 0, *imageLabel->pixmap());
        }
#endif
    }

    void ImageViewer::zoomIn()
    {
        scaleImage(1.25);
    }

    void ImageViewer::zoomOut()
    {
        scaleImage(0.8);
    }

    void ImageViewer::normalSize()
    {
        imageLabel->adjustSize();
        scaleFactor = 1.0;
    }

    void ImageViewer::fitToWindow()
    {
        bool fitToWindow = fitToWindowAct->isChecked();
        scrollArea->setWidgetResizable(fitToWindow);
        if (!fitToWindow) {
            normalSize();
        }
        updateActions();
    }

    void ImageViewer::about()
    {
        QMessageBox::about(this, tr("About Image Viewer"),
                           tr("<p>The <b>Image Viewer</b> example shows how to combine QLabel "
                              "and QScrollArea to display an image. QLabel is typically used "
                              "for displaying a text, but it can also display an image. "
                              "QScrollArea provides a scrolling view around another widget. "
                              "If the child widget exceeds the size of the frame, QScrollArea "
                              "automatically provides scroll bars. </p><p>The example "
                              "demonstrates how QLabel's ability to scale its contents "
                              "(QLabel::scaledContents), and QScrollArea's ability to "
                              "automatically resize its contents "
                              "(QScrollArea::widgetResizable), can be used to implement "
                              "zooming and scaling features. </p><p>In addition the example "
                              "shows how to use QPainter to print an image.</p>"));
    }

    void ImageViewer::createActions()
    {
        openAct = new QAction(tr("&Open..."), this);
        openAct->setShortcut(tr("Ctrl+O"));
        connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

        printAct = new QAction(tr("&Print..."), this);
        printAct->setShortcut(tr("Ctrl+P"));
        printAct->setEnabled(false);
        connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

        exitAct = new QAction(tr("E&xit"), this);
        exitAct->setShortcut(tr("Ctrl+Q"));
        connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

        zoomInAct = new QAction(tr("Zoom &In (25%)"), this);
        zoomInAct->setShortcut(tr("Ctrl++"));
        zoomInAct->setEnabled(false);
        connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

        zoomOutAct = new QAction(tr("Zoom &Out (25%)"), this);
        zoomOutAct->setShortcut(tr("Ctrl+-"));
        zoomOutAct->setEnabled(false);
        connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

        normalSizeAct = new QAction(tr("&Normal Size"), this);
        normalSizeAct->setShortcut(tr("Ctrl+S"));
        normalSizeAct->setEnabled(false);
        connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

        fitToWindowAct = new QAction(tr("&Fit to Window"), this);
        fitToWindowAct->setEnabled(false);
        fitToWindowAct->setCheckable(true);
        fitToWindowAct->setShortcut(tr("Ctrl+F"));
        connect(fitToWindowAct, SIGNAL(triggered()), this, SLOT(fitToWindow()));

        aboutAct = new QAction(tr("&About"), this);
        connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

        aboutQtAct = new QAction(tr("About &Qt"), this);
        connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    }

    void ImageViewer::createMenus()
    {
        fileMenu = new QMenu(tr("&File"), this);
        fileMenu->addAction(openAct);
        fileMenu->addAction(printAct);
        fileMenu->addSeparator();
        fileMenu->addAction(exitAct);

        viewMenu = new QMenu(tr("&View"), this);
        viewMenu->addAction(zoomInAct);
        viewMenu->addAction(zoomOutAct);
        viewMenu->addAction(normalSizeAct);
        viewMenu->addSeparator();
        viewMenu->addAction(fitToWindowAct);

        helpMenu = new QMenu(tr("&Help"), this);
        helpMenu->addAction(aboutAct);
        helpMenu->addAction(aboutQtAct);

        menuBar()->addMenu(fileMenu);
        menuBar()->addMenu(viewMenu);
        menuBar()->addMenu(helpMenu);
    }

    void ImageViewer::updateActions()
    {
        zoomInAct->setEnabled(!fitToWindowAct->isChecked());
        zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
        normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
    }

    void ImageViewer::scaleImage(double factor)
    {
        Q_ASSERT(imageLabel->pixmap());
        scaleFactor *= factor;
        imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

        adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
        adjustScrollBar(scrollArea->verticalScrollBar(), factor);

        zoomInAct->setEnabled(scaleFactor < 10.0);
        zoomOutAct->setEnabled(scaleFactor > 0.05);
    }

    void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor)
    {
        scrollBar->setValue(int(factor * scrollBar->value()
                                + ((factor - 1) * scrollBar->pageStep()/2)));
    }
