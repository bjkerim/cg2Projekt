﻿/****************************************************************************
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

int xLinearFilterValue;
int yLinearFilterValue;
float sigma;

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


//  Helligkeitswert Y aus den RGB Kanälen errechnen
int ImageViewer::getYfromRGB(int color){
    int y = ((qRed(color)* 0.299) + (qGreen(color)* 0.587) + (qBlue(color)*0.114));
    return y;
}

// RGB in YcbCr umwandeln
std::vector<int> ImageViewer::convertToYcbcr(QRgb color){

    int y = ((qRed(color)* 0.299) + (qGreen(color)* 0.587) + (qBlue(color)*0.114));
    int cb = 128+((qRed(color) * -0.169) + (qGreen(color)* -0.331) + (qBlue(color)*0.5));
    int cr = 128+((qRed(color) * 0.5) + (qGreen(color)* -0.419) + (qBlue(color)*-0.081));

    return {y, cb, cr};

}

// YcbCr in RGB umwandeln
int ImageViewer::convertYcbcrToRgb(int y, int cb, int cr){

    int newR = (((y-16)*1.164) + ((cr-128)*1.596));
    int newG = (((y-16)*1.164) + ((cb-128)*(-0.392)) + ((cr-128)*(-0.813)));
    int newB = (((y-16)*1.164) + ((cb-128)*2.017));


    //Clampen
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

    int newColor = qRgb(newR,newG,newB);

    return newColor;
}

// Getter und Setter für die X und Y Werte aus den Slidern lesen und setzen, damit diese Werte in den Linearfiltern abgerufen werden können
void ImageViewer::setLinearXValue(int x){
    xLinearFilterValue = x;
}

void ImageViewer::setLinearYValue(int y){
    yLinearFilterValue = y;
}

int ImageViewer::getLinearXValue(){
    return xLinearFilterValue;
}

int ImageViewer::getLinearYValue(){
    return yLinearFilterValue;
}


// imageCopy ist eine Arbeitskopie in Farbe des Originalbildes

void ImageViewer::createImageColor() {

    imageCopy = new QImage(*image);
}

// imageCopy ist eine Arbeitskopie die in Graustufen umgewandelt wird (siehe converToGrayscale())

void ImageViewer::createImageBlackCopy() {

    imageGray = new QImage(*image);
}



// Übung 1
void ImageViewer::applyCross(int kreuzBreite)

{

    if(image!=NULL)
    {
        imageCopy = new QImage(*image);

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


    renewLogging();
}


// Entfernt das Kreuz aus dem Bild
void ImageViewer::deleteCross() {
    imageLabel->setPixmap(QPixmap::fromImage(*image));
}


// imageGray wird in Graustufen umgewandelt
void ImageViewer::convertToGrayscale(){

    if(image != NULL){
        int w = image->width();
        int h = image->height();
        createImageBlackCopy();


        for (int i = 0; i <= w-1; i++){
            for(int j = 0; j <= h-1; j++ ){
                QRgb color = image->pixel(i,j);
                int gray = ((qRed(color) *0.299) + (qGreen(color)* 0.587) + (qBlue(color)*0.114));
                imageGray->setPixel(i, j, qRgb(gray,gray,gray));
            }
        }
    }
    imageLabel->setPixmap(QPixmap::fromImage(*imageGray));

    renewLogging();
}

// Rechnet die mittlere Helligkeit für Graustufenbilder
void ImageViewer::mittlereHelligkeitGraustufen(){

    if(imageGray!=NULL){


        int w = image->width();
        int h = image->height();

        int averageBrightness = 0;
        long brightnessAddition = 0;
        long pixelCount = w*h;



        for (int i = 0; i < w; i++){
            for(int j = 0; j < h; j++ ){
                brightnessAddition += qRed(imageGray->pixel(i,j));
            }
        }
        averageBrightness = (brightnessAddition/pixelCount);

        labelMiddleBrightness->setText("Die Mittlere Helligkeit beträgt: " + QString::number(averageBrightness));

        renewLogging();

    }}

// Rechnet die Varianz für Graustufenbilder
void ImageViewer::varianzGraufstufe(){

    if(imageGray!=NULL){

        int w = image->width();
        int h = image->height();


        int averageBrightness;
        int brightnessSumAverage = 0;
        int pixelCount = w*h;

        long brightnessSum =0;
        long tempRes = 0;
        int tempBright = 0;


        for (int i = 0; i < w; i++){
            for(int j = 0; j < h; j++ ){
                brightnessSumAverage += qRed(imageGray->pixel(i,j));
            }
        }

        averageBrightness = (brightnessSumAverage/pixelCount);


        for (int i = 0; i < w; i++){
            for(int j = 0; j < h; j++ ){
                brightnessSum = qRed(imageGray->pixel(i,j));
                tempBright = brightnessSum - averageBrightness;
                tempRes = tempRes +(tempBright*tempBright);

            }
        }


        long myVarianz = tempRes / (h*w);
        labelVarianz->setText("Die Varianz beträgt: " + QString::number(myVarianz));
        renewLogging();
    }
}

// Setzt die Bits der Graustufenbilder nach Sliderwert
void ImageViewer::bitDynamikGraustufe(int bits){
    if(imageGray!=NULL){
        *imageGray = *image;
        int w = image->width();
        int h = image->height();



        int bit = bits;
        int stufen = (1<<bit); //Bitshift für direkte Berechnung der Stufen pro Bit (1 nach links "geschoben")
        int ausGrenze = 256/stufen;
        int pixelBereich;
        int neuWert;

        for (int i = 0; i < w; i++){
            for(int j = 0; j < h; j++ ){
                pixelBereich = ((qRed(imageGray->pixel(i,j)))/ausGrenze)+1;
                neuWert = (ausGrenze * pixelBereich)-1;
                imageGray->setPixel(i, j, qRgb(neuWert,neuWert,neuWert));


            }
        }
        imageLabel->setPixmap(QPixmap::fromImage(*imageGray));
        histogramGraustufe();
        mittlereHelligkeitGraustufen();
    }
    varianzGraufstufe();


}



// Setzt die Bits der Farbbilder nach Sliderwert
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
                QRgb color = imageCopy->pixel(i,j);
                int y = ((qRed(color) *0.299) + (qGreen(color)* 0.587) + (qBlue(color)*0.114));
                int cb = 128+((qRed(color) * -0.169) + (qGreen(color)* -0.331) + (qBlue(color)*0.5));
                int cr = 128+((qRed(color) * 0.5) + (qGreen(color)* -0.419) + (qBlue(color)*-0.081));

                pixelBereich = (y/ausGrenze)+1;
                yNew = (ausGrenze * pixelBereich)-1;

                int newR = ((yNew-16)*1.164) + ((cr-128)*1.596);
                int newG = ((yNew-16)*1.164) + ((cb-128)*(-0.392)) + ((cr-128)*(-0.813));
                int newB = ((yNew-16)*1.164) + ((cb-128)*2.017);

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
    histogramColor();

}

// Erstellt Histogramm in lograthmischer Skalierung
void ImageViewer::histogramLog(){
    if(imageGray!=NULL){

        QPixmap histogramMap(256,256);
        histogramMap.fill(QColor("white"));
        myHistogramLog = new QImage(256, 100, QImage::Format_RGB32);
        myHistogramLog->fill(Qt::white);

        int myArray[256]= {0};
        int myValue = 0;


        for (int i = 0; i<image->width() ;i++) {
            for (int j = 0; j<image->height(); j++){
                myValue = qRed(imageGray->pixel(i,j));
                myArray[myValue] += 1;

            }
        }
        int greatestNumber = 0;

        for(int c = 0; c<256; c++){
            if (myArray[c] > greatestNumber){
                greatestNumber = myArray[c];


            }}

        for (int k = 0; k<256 ;k++){
            float myValueInLog = std::log2(myArray[k]) ;
            float myMaxInLog = std::log2(greatestNumber);
            float endValue = myValueInLog*100.0/myMaxInLog;




            for (int h=100; h>(100-endValue); h--){

                myHistogramLog->setPixelColor(k,h, Qt::black);
            }
        }
    }
    meinHistogramLog->setPixmap(QPixmap::fromImage(*myHistogramLog));
    meinHistogramLog->show();

}



// Erstellt Histogramm in linearer Skalierung für Graustufenbilder

void ImageViewer::histogramGraustufe(){
    if(imageGray!=NULL){

        QPixmap histogramMap(256,256);
        histogramMap.fill(QColor("white"));
        myHistogram = new QImage(256, 100, QImage::Format_RGB32);
        myHistogram->fill(Qt::white);


        int myArray[256]= {0};
        int myValue = 0;


        for (int i = 0; i<image->width() ;i++) {
            for (int j = 0; j<image->height(); j++){
                myValue = qRed(imageGray->pixel(i,j));
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
                myHistogram->setPixelColor(z,h, Qt::black);
            }

        }
        meinHistogram->setPixmap(QPixmap::fromImage(*myHistogram));
        meinHistogram->show();

    }}




/************************************************/

// Erstellt Histogramm in linearer Skalierung für Farbbilder

void ImageViewer::histogramColor() {
    if (image !=NULL){


        myHistogramColor = new QImage(256, 100, QImage::Format_RGB32);
        myHistogramColor->fill(Qt::black);


        int myArrayRed[256]= {0};
        int myArrayGreen[256]= {0};
        int myArrayBlue[256]= {0};
        int myValue = 0;


        for (int i = 0; i<image->width() ;i++) {
            for (int j = 0; j<image->height(); j++){
                myValue = qRed(imageCopy->pixel(i,j));
                myArrayRed[myValue] += 1;


                myValue = qGreen(imageCopy->pixel(i,j));
                myArrayGreen[myValue] += 1;

                myValue = qBlue(imageCopy->pixel(i,j));
                myArrayBlue[myValue] += 1;

            }
        }

        int greatestNumber = 0;

        for(int c = 0; c<256; c++){
            if (myArrayRed[c] > greatestNumber){
                greatestNumber = myArrayRed[c];

            }

            else if(myArrayGreen[c] > greatestNumber){
                greatestNumber = myArrayGreen[c];

            }
            else if(myArrayBlue[c] > greatestNumber){
                greatestNumber = myArrayBlue[c];

            }

        }



        int endWertRed[256] = {0};
        for (int k = 0; k<256 ;k++){
            endWertRed[k] = (myArrayRed[k]/(greatestNumber/100));

        }

        int endWertGreen[256] = {0};
        for (int k = 0; k<256 ;k++){
            endWertGreen[k] = (myArrayGreen[k]/(greatestNumber/100));

        }

        int endWertBlue[256] = {0};
        for (int k = 0; k<256 ;k++){
            endWertBlue[k] = (myArrayBlue[k]/(greatestNumber/100));

        }


        for (int z = 0; z<256; z++){
            for (int h=0; h<(endWertRed[z]); h++){
                myHistogramColor->setPixelColor(z,h, Qt::red);
            }
        }

        for (int z = 0; z<256; z++){
            for (int h=0; h<(endWertGreen[z]); h++){
                //rot abfragen wenn mit grün gepainted wird
                if(qRed(myHistogramColor->pixel(z,h)) == 255 ){
                    myHistogramColor->setPixelColor(z,h, Qt::yellow);
                }

                else {
                    myHistogramColor->setPixelColor(z,h, Qt::green);
                }
            }
        }

        for (int z = 0; z<256; z++){
            for (int h=0; h<(endWertBlue[z]); h++){

                if((qRed(myHistogramColor->pixel(z,h)) == 255) && (qGreen(myHistogramColor->pixel(z,h)) == 0)){ //rot prüfen und magenta overpainten
                    myHistogramColor->setPixelColor(z,h, Qt::magenta);
                }

                else if((qRed(myHistogramColor->pixel(z,h)) == 0) && (qGreen(myHistogramColor->pixel(z,h)) == 255)){ //grün prüfen und cyan drüberballern
                    myHistogramColor->setPixelColor(z,h, Qt::cyan);

                }
                else if((qRed(myHistogramColor->pixel(z,h)) == 255) && (qGreen(myHistogramColor->pixel(z,h)) == 255)){
                    myHistogramColor->setPixelColor(z,h, Qt::gray);

                }

                else{
                    myHistogramColor->setPixelColor(z,h, Qt::blue);
                }


            }
        }

        QImage mirrorImage  = myHistogramColor->mirrored(false,true);
        myHistogramColorLabel->setPixmap(QPixmap::fromImage(mirrorImage));
        myHistogramColorLabel->show();

    }
}

/*******************************************************************/

// Helligkeitsregler für Graustufenbilderr

void ImageViewer::brightnessGraustufe(int helligkeitswert) {
    if(imageGray!=NULL){
        *imageGray = *image;

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
                imageGray->setPixel(i,j,color);

            }
        }

        imageLabel->setPixmap(QPixmap::fromImage(*imageGray));

        mittlereHelligkeitGraustufen();
        histogramGraustufe();
    }varianzGraufstufe();
}

/*************************************************************************/
// Helligkeitsregler für Farbbilder

void ImageViewer::brightnessColor(int helligkeitswert) {
    if (image!=NULL){
        *imageCopy = *image;

        int w = image->width();
        int h = image->height();


        for (int i = 0; i < w; i++){
            for(int j = 0; j < h; j++ ){
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


                imageCopy->setPixel(i, j, qRgb(newR,newG,newB));

            }
        }

        imageLabel->setPixmap(QPixmap::fromImage(*imageCopy));

    }
    histogramColor();

}


/***************************************************************************/




void ImageViewer::kontrast(int konstante) {
    if(imageGray!=NULL){
        *imageGray = *image;

        int w = image->width();
        int h = image->height();
        int low = 0;
        int high = 0;
        int mid = 0;
        low = qRed(imageGray->pixel(0,0));
        high = qRed(imageGray->pixel(0,0));
        for (int i=0; i <w; i++){
            for (int j = 0; j<h; j++){

                int aktuellerWert = qRed(imageGray->pixel(i,j));
                if (aktuellerWert < low){
                    low = aktuellerWert;
                }
                if (aktuellerWert > high){
                    low = aktuellerWert;

                }
            }
        }

        mid = (high+low)/2;

        for (int i=0; i <w; i++){
            for (int j = 0; j<h; j++){
                int newPixelValue = 0;

                // Hier überprüfen wir, ob unsere Variable Konstante größer oder kleiner 0 ist um entsprechend zu skalieren
                if (konstante > 0) {
                    newPixelValue = (((qRed(imageGray->pixel(i,j)) - mid) * konstante)) + 0.5;
                    newPixelValue = newPixelValue + mid;
                }

                else if (konstante < 0) {

                    newPixelValue = (((qRed(imageGray->pixel(i,j)) - mid) / std::abs(konstante))) + 0.5;
                    newPixelValue = newPixelValue + mid;
                }


                // Falls Konstante 0, manipuliere das Bild nicht
                else if (konstante == 0){
                    newPixelValue = qRed(imageGray->pixel(i,j));
                }


                if(newPixelValue > 255) {
                    newPixelValue = 255;
                }

                else if (newPixelValue < 0) {
                    newPixelValue = 0;
                }

                imageGray->setPixel(i, j, qRgb(newPixelValue,newPixelValue,newPixelValue));

            }
        }


        imageLabel->setPixmap(QPixmap::fromImage(*imageGray));


        renewLogging();
        mittlereHelligkeitGraustufen();
        histogramGraustufe();
    }varianzGraufstufe();

}
/*******************************************************/




void ImageViewer::kontrastColor(int konstante) {

    *imageCopy = *image;

    int w = imageCopy->width();
    int h = imageCopy->height();
    int low = 0;
    int high = 0;
    int mid = 0;
    low = ((qRed(image->pixel(0,0)) *0.299) + (qGreen(image->pixel(0,0))* 0.587) + (qBlue(image->pixel(0,0))*0.114));
    high = ((qRed(image->pixel(0,0)) *0.299) + (qGreen(image->pixel(0,0))* 0.587) + (qBlue(image->pixel(0,0))*0.114));

    for (int i = 0; i <= w-1; i++){
        for(int j = 0; j <= h-1; j++ ){
            QRgb color = image->pixel(i,j);
            int y = ((qRed(color) *0.299) + (qGreen(color)* 0.587) + (qBlue(color)*0.114));

            /*_---------------------------*/
            int aktuellerWert = y;
            if (aktuellerWert < low){
                low = aktuellerWert;
            }
            if (aktuellerWert > high){
                high = aktuellerWert;

            }

        }
    }
    mid = (high+low)/2;


    for (int i=0; i <w; i++){
        for (int j = 0; j<h; j++){

            // Konvertiere RGB Werte in YcbCr, um auf die Helligkeit zuzugreifen
            QRgb color = image->pixel(i,j);
            int y = ((qRed(color) *0.299) + (qGreen(color)* 0.587) + (qBlue(color)*0.114));
            int cb = 128+((qRed(color) * -0.169) + (qGreen(color)* -0.331) + (qBlue(color)*0.5));
            int cr = 128+((qRed(color) * 0.5) + (qGreen(color)* -0.419) + (qBlue(color)*-0.081));

            int newYValue = 0;
            if (konstante > 0) {
                newYValue = ((y - mid) * konstante) + 0.5;
                newYValue = newYValue + mid;
            }

            else if (konstante < 0) {

                newYValue = ((y - mid) / std::abs(konstante)) + 0.5;
                newYValue = newYValue + mid;

            }

            else if (konstante == 0) {
                newYValue = y;


            }
            //K Konvertiere YcbCr Werte in RGB, um sie später korrekt anzuzeigen
            int newR = ((newYValue-16)*1.164) + ((cr-128)*1.596);
            int newG = ((newYValue-16)*1.164) + ((cb-128)*(-0.392)) + ((cr-128)*(-0.813));
            int newB =  ((newYValue-16)*1.164) + ((cb-128)*2.017);


            // Clamping der Werte

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
    }
    imageLabel->setPixmap(QPixmap::fromImage(*imageCopy));
    histogramColor();

}
/************************************************************************/

void ImageViewer::autoKontrast(int kontrastWert) {

    if(imageGray!=NULL){
        *imageGray = *image;
        int w = image->width();
        int h = image->height();

        int neueHelligkeit = 0;
        int aMin = 0;
        int aMax = 255;


        float kontrastProzent = kontrastWert/100.0;
        int grenzeLow = kontrastProzent*(h*w);
        int grenzeHigh = (1-kontrastProzent)*(h*w);

        int aStrichLow = qRed(imageGray->pixel(0,0));
        int aStrichHigh = qRed(imageGray->pixel(0,0));;

        int histogram[256]= {0};
        int kHistogram[256] = {0};

        int aLow = 300;
        int aHigh = 0;

        /* Suche hellsten und dunkelsten Wert raus */
        for (int i=0; i<w; i++){
            for (int j=0; j<h; j++){
                if(qRed(imageGray->pixel(i,j)) < aLow){
                    aLow = qRed(imageGray->pixel(i,j));
                }
                else if (qRed(imageGray->pixel(i,j)) > aHigh){
                    aHigh = qRed(imageGray->pixel(i,j));
                }
            }
        }

        //    /* Create Array */
        for (int i = 0; i<w ;i++) {
            for (int j = 0; j<h; j++){
                int myValue = qRed(imageGray->pixel(i,j));
                histogram[myValue] += 1;
            }
        }
        // Erstelle kummulatives Histogram, aufaddieren des absoluten Vorkommens der Werte auf den vorherigen Wert
        kHistogram[0] = histogram[0];
        for(int i=1; i<256; i++){

            kHistogram[i] = kHistogram[i-1] + histogram[i];
        }




        // Höchsten und niedrigsten Wert im Histogram finden
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
                neueHelligkeit = qRed(imageGray->pixel(i,j));

                if (neueHelligkeit <= aStrichLow){
                    neueHelligkeit = aMin;
                }

                else if ( neueHelligkeit >= aStrichHigh ){
                    neueHelligkeit = aMax; }
                else {
                    neueHelligkeit =  aMin + (neueHelligkeit - aStrichLow) * ((aMax-aMin)/(aStrichHigh-aStrichLow));

                }

                imageGray->setPixel(i,j,qRgb(neueHelligkeit,neueHelligkeit,neueHelligkeit));

            }

        }    imageLabel->setPixmap(QPixmap::fromImage(*imageGray));
        mittlereHelligkeitGraustufen();

    }histogramGraustufe();
    varianzGraufstufe();
}

/***********************************************************************************************************/
void ImageViewer::autoKontrastColor(int kontrastWert) {


    *imageCopy = *image;
    int w = image->width();
    int h = image->height();

    int neueHelligkeit = 0;
    int aMin = 0;
    int aMax = 255;

    float kontrastProzent = kontrastWert/100.0;
    int grenzeLow = kontrastProzent*(h*w);
    int grenzeHigh = (1-kontrastProzent)*(h*w);

    int aStrichLow = ((qRed(image->pixel(0,0)) *0.299) + (qGreen(image->pixel(0,0))* 0.587) + (qBlue(image->pixel(0,0))*0.114));
    int aStrichHigh = ((qRed(image->pixel(0,0)) *0.299) + (qGreen(image->pixel(0,0))* 0.587) + (qBlue(image->pixel(0,0))*0.114));;

    int histogram[256]= {0};
    int kHistogram[256] = {0};

    int aLow = 300;
    int aHigh = 0;

    /* Suche hellsten und dunkelsten Wert raus */
    for (int i=0; i<w; i++){
        for (int j=0; j<h; j++){
            int tempPixelColor = (qRed(image->pixel(i,j)) *0.299) + (qGreen(image->pixel(i,j))* 0.587) + (qBlue(image->pixel(i,j))*0.114);
            if ( tempPixelColor < aLow){
                aLow = tempPixelColor;
            }
            else if (tempPixelColor > aHigh){
                aHigh = tempPixelColor;
            }
        }
    }

    //   Create Array
    for (int i = 0; i<w ;i++) {
        for (int j = 0; j<h; j++){
            int myValue = ((qRed(image->pixel(i,j)) *0.299) + (qGreen(image->pixel(i,j))* 0.587) + (qBlue(image->pixel(i,j))*0.114));
            histogram[myValue] += 1;
        }
    }




    // Erstelle kummulatives Histogram
    kHistogram[0] = histogram[0];
    for(int i=1; i<256; i++){

        kHistogram[i] = kHistogram[i-1] + histogram[i];
    }


    // Suche min-max Werte
    for (int i=0; i<256; i++){
        if(kHistogram[i] >= grenzeLow && i<aStrichLow){
            aStrichLow = i;
        }

        else if (kHistogram[i]<= grenzeHigh && i > aStrichHigh){
            aStrichHigh = i;
        }
    }



    // Konvertiere in YcbCr
    for (int i=0; i<w; i++){
        for (int j=0; j<h; j++){
            QRgb color = image->pixel(i,j);
            int cb = 128+((qRed(color) * -0.169) + (qGreen(color)* -0.331) + (qBlue(color)*0.5));
            int cr = 128+((qRed(color) * 0.5) + (qGreen(color)* -0.419) + (qBlue(color)*-0.081));

            neueHelligkeit = (qRed(image->pixel(i,j)) *0.299) + (qGreen(image->pixel(i,j))* 0.587) + (qBlue(image->pixel(i,j))*0.114);



            if (neueHelligkeit <= aStrichLow){
                neueHelligkeit = aMin;
            }

            else if ( neueHelligkeit >= aStrichHigh ){
                neueHelligkeit = aMax; }
            else {
                neueHelligkeit =  aMin + (neueHelligkeit - aStrichLow) * ((aMax-aMin)/(aStrichHigh-aStrichLow));

            }

            // Konvertiere YcbCr in RGB

            int newR = ((neueHelligkeit-16)*1.164) + ((cr-128)*1.596);
            int newG = ((neueHelligkeit-16)*1.164) + ((cb-128)*(-0.392)) + ((cr-128)*(-0.813));
            int newB =  ((neueHelligkeit-16)*1.164) + ((cb-128)*2.017);

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

    }
    imageLabel->setPixmap(QPixmap::fromImage(*imageCopy));
    histogramColor();
}

/****************************************************************************************
*
*  mit dieser Methode können sie sich pro Aufgabe ein  Tab anlegen, in der die Ein-
*  stellungen per Slider, Button, Checkbox etc. gemacht werden und die zu implemen-
*  tierenden Algorithmen gestatet werden.
*
*****************************************************************************************/

void ImageViewer::generateFilterTable(){
    int x = getLinearXValue();
    int y = getLinearYValue();


    linearFilterTable->setRowCount(x);
    linearFilterTable->setColumnCount(y);

}


void ImageViewer::linearFilter(){
    if (image !=NULL){
        *imageCopy = *image;

        int filterKoeffSum = 0;
        int filterArrayWidth = getLinearXValue();
        int filterArrayHeight = getLinearYValue();

        //erstelle 2D int array um unsere gewichtungen einzutragen
        //int filterArray[filterArrayWidth][filterArrayHeight];

        for(int i = 0; i < filterArrayWidth; ++i){
            for(int j = 0; j < filterArrayHeight; ++j){
                filterKoeffSum += linearFilterTable->item(j, i)->text().toInt();
            }
        }

        double sEinzelGewichtung = 1.0/filterKoeffSum;

        //schritte um den hotspot-pixel herum
        int LHotspotY = filterArrayHeight/2;
        int KHotspotx = filterArrayWidth/2;

        for (int v = LHotspotY; v <= (image->height() - LHotspotY - 1); v++){ //Lässt den Rand frei
            for (int u = KHotspotx; u <= (image->width() - KHotspotx -1); u++){

                int cbTemp = 128+((qRed(image->pixel(v,u)) * -0.169) + (qGreen(image->pixel(v,u))* -0.331) + (qBlue(image->pixel(v,u))*0.5));
                int crTemp = 128+((qRed(image->pixel(v,u)) * 0.5) + (qGreen(image->pixel(v,u))* -0.419) + (qBlue(image->pixel(v,u))*-0.081));


                int sum = 0;
                // j für schritte entlang der y achse, i für schritte entlang der x achse
                // iteriere durch filtertable für Gewichtung der Pixel
                for (int j= (-LHotspotY); j <= LHotspotY; j++){
                    for(int i =(-KHotspotx); i <= KHotspotx ; i++){

                        int yTemp = getYfromRGB(image->pixel(v+i, u+j));
                        int c = linearFilterTable->item((j + LHotspotY),(i + KHotspotx))->text().toInt();

                        sum += c * yTemp;

                    }
                }
                int newY = (int) ((sEinzelGewichtung* sum) + 0.5);


                QRgb color = convertYcbcrToRgb(newY, cbTemp, crTemp);

                imageCopy->setPixel(v, u, color);
            }

        }
        imageLabel->setPixmap(QPixmap::fromImage(*imageCopy));

    }}

void ImageViewer::linearFilterZeroPadding(){
    if (image !=NULL && getLinearXValue()!=NULL){
        *imageCopy = *image;

        int filterKoeffSum = 0;
        int filterArrayWidth = getLinearXValue();
        int filterArrayHeight = getLinearYValue();

        //erstelle 2D int array um unsere gewichtungen einzutragen
        //int filterArray[filterArrayWidth][filterArrayHeight];

        for(int i = 0; i < filterArrayWidth; ++i){
            for(int j = 0; j < filterArrayHeight; ++j){
                filterKoeffSum += linearFilterTable->item(j, i)->text().toInt();
            }
        }

        double sEinzelGewichtung = 1.0/filterKoeffSum;

        //schritte um den hotspot-pixel herum
        int LHotspotY = filterArrayHeight/2;
        int KHotspotx = filterArrayWidth/2;

        for (int v = 0; v < image->height(); v++){
            for (int u = 0; u < image->width(); u++){

                int cbTemp = 128+((qRed(image->pixel(v,u)) * -0.169) + (qGreen(image->pixel(v,u))* -0.331) + (qBlue(image->pixel(v,u))*0.5));
                int crTemp = 128+((qRed(image->pixel(v,u)) * 0.5) + (qGreen(image->pixel(v,u))* -0.419) + (qBlue(image->pixel(v,u))*-0.081));


                int sum = 0;
                // j für schritte entlang der y achse, i für schritte entlang der x achse
                //mini matrix um den Hotspot herum

                for (int j = (-LHotspotY); j <= LHotspotY; j++){
                    for(int i = (-KHotspotx); i <= KHotspotx ; i++){
                        int yTemp = 0;


                        if((v+j) < 0 || (v+j) > (image->height()-1)  || (u+i) < 0 || (u+i) > (image->width()-1)) {
                            // Falls Rand, färbe Grau
                            yTemp = getYfromRGB(127);
                        }

                        else{
                            // Falls kein Rand, Pixel relativ zum hotspot
                            yTemp = getYfromRGB(image->pixel(v+j, u+i));

                        }

                        // Entnehme aus der Filtertable die Gewichtung
                        int c = linearFilterTable->item((j + LHotspotY),(i + KHotspotx))->text().toInt();

                        sum += c * yTemp;

                    }
                }
                int newY = (int) ((sEinzelGewichtung* sum) + 0.5);


                QRgb color = convertYcbcrToRgb(newY, cbTemp, crTemp);

                imageCopy->setPixel(v, u, color);
            }
        }
        imageLabel->setPixmap(QPixmap::fromImage(*imageCopy));
    }}

void ImageViewer::linearFilterKonstPadding(){
    if (image !=NULL && getLinearXValue()!=NULL){
        *imageCopy = *image;

        int filterKoeffSum = 0;
        int filterArrayWidth = getLinearXValue();
        int filterArrayHeight = getLinearYValue();

        //erstelle 2D int array um unsere gewichtungen einzutragen
        //int filterArray[filterArrayWidth][filterArrayHeight];

        for(int i = 0; i < filterArrayWidth; ++i){
            for(int j = 0; j < filterArrayHeight; ++j){
                filterKoeffSum += linearFilterTable->item(j, i)->text().toInt();
            }
        }

        double sHotspotGewichtung = 1.0/filterKoeffSum;

        //schritte um den hotspot-pixel herum
        int LHotspotY = filterArrayHeight/2;
        int KHotspotx = filterArrayWidth/2;

        for (int v = 0; v < image->height(); v++){ //Lässt den Rand frei
            for (int u = 0; u < image->width(); u++){

                int cbTemp = 128+((qRed(image->pixel(v,u)) * -0.169) + (qGreen(image->pixel(v,u))* -0.331) + (qBlue(image->pixel(v,u))*0.5));
                int crTemp = 128+((qRed(image->pixel(v,u)) * 0.5) + (qGreen(image->pixel(v,u))* -0.419) + (qBlue(image->pixel(v,u))*-0.081));


                int sum = 0;
                // j für schritte entlang der y achse, i für schritte entlang der x achse
                //mini matrix um den Hotspot herum

                for (int j = (-LHotspotY); j <= LHotspotY; j++){
                    for(int i = (-KHotspotx); i <= KHotspotx ; i++){
                        int yTemp = 0;


                        if((v+j) < 0 || (v+j) >= (image->height())  || (u+i) < 0 || (u+i) >= (image->width())) {
                            // Falls Rand, nehme nächstmöglich vorhandenen Pixel
                            yTemp = getYfromRGB(image->pixel(v,u));
                        }

                        else{
                            // Falls kein Rand, Pixel relativ zum hotspot
                            yTemp = getYfromRGB(image->pixel(v+j, u+i));

                        }

                        // Entnehme aus der Filtertable die Gewichtung

                        int c = linearFilterTable->item((j + LHotspotY),(i + KHotspotx))->text().toInt();

                        sum += c * yTemp;

                    }
                }
                int newY = (int) ((sHotspotGewichtung* sum) + 0.5);


                QRgb color = convertYcbcrToRgb(newY, cbTemp, crTemp);

                imageCopy->setPixel(v, u, color);
            }
        }
        imageLabel->setPixmap(QPixmap::fromImage(*imageCopy));
    }
}

void ImageViewer::linearFilterReflectionPadding(){
    if (image !=NULL && getLinearXValue()!=NULL){
        *imageCopy = *image;

        int filterKoeffSum = 0;
        int filterArrayWidth = getLinearXValue();
        int filterArrayHeight = getLinearYValue();

        //erstelle 2D int array um unsere gewichtungen einzutragen
        //int filterArray[filterArrayWidth][filterArrayHeight];

        for(int i = 0; i < filterArrayWidth; ++i){
            for(int j = 0; j < filterArrayHeight; ++j){
                filterKoeffSum += linearFilterTable->item(j, i)->text().toInt();
            }
        }

        double sHotspotGewichtung = 1.0/filterKoeffSum;

        //schritte um den hotspot-pixel herum
        int LHotspotY = filterArrayHeight/2;
        int KHotspotx = filterArrayWidth/2;

        for (int v = 0; v < image->height(); v++){ //Lässt den Rand frei
            for (int u = 0; u < image->width(); u++){

                int cbTemp = 128+((qRed(image->pixel(v,u)) * -0.169) + (qGreen(image->pixel(v,u))* -0.331) + (qBlue(image->pixel(v,u))*0.5));
                int crTemp = 128+((qRed(image->pixel(v,u)) * 0.5) + (qGreen(image->pixel(v,u))* -0.419) + (qBlue(image->pixel(v,u))*-0.081));


                int sum = 0;
                // j für schritte entlang der y achse, i für schritte entlang der x achse
                //mini matrix um den Hotspot herum

                for (int j = (-LHotspotY); j <= LHotspotY; j++){
                    for(int i = (-KHotspotx); i <= KHotspotx ; i++){
                        int yTemp = 0;


                        if((v+j) < 0 || (v+j) >= (image->height())) {
                            // Falls Pixel entlang der Y Achse out of range, nehme Pixel in die andere Richtung entlang der Achse von v-u aus
                            yTemp = getYfromRGB(image->pixel(v-j,u));

                        }
                        else if((u+i) < 0 || (u+i) >= (image->width())){
                            // Falls Pixel entlang der X Achse out of range, nehme Pixel in die andere Richtung entlang der Achse von v-u aus

                            yTemp = getYfromRGB(image->pixel(v,u-i));
                        }

                        else{
                            // Falls kein Rand, Pixel relativ zum hotspot

                            yTemp = getYfromRGB(image->pixel(v+j, u+i));

                        }

                        int c = linearFilterTable->item((j + LHotspotY),(i + KHotspotx))->text().toInt();

                        sum += c * yTemp;

                    }
                }
                int newY = (int) ((sHotspotGewichtung* sum) + 0.5);


                QRgb color = convertYcbcrToRgb(newY, cbTemp, crTemp);

                imageCopy->setPixel(v, u, color);
            }
        }
        imageLabel->setPixmap(QPixmap::fromImage(*imageCopy));
    }}

void ImageViewer::doubleDGauss(){
    if (image !=NULL){
        *imageCopy = *image;
        float sigma = sigmaInput1->text().toFloat(); //Entnehme Werte aus der Tabelle
        int hotspotGewichtung = 0;

        //create the kernel h:
        int center = (int) ((3.0 * sigma)+0.5);

        std::vector<float> h((2*center)+1); // ungerade Werte, da Center + schritte rechts/links

        int hLength = h.size();
        int steps = hLength/2;

        //fill the kernel h:
        double sigmaQuadr = sigma *sigma; //sigma²

        for (int i = 0; i < hLength; i++){ //setzt die index werte in die gauss funktion ein, index verschiebt sich dass mitte des arrays auf null ist. sigma quadrat unverändert über alle schleifen
            double r = center -i;

            h[i] = ((float) std::exp((-0.5 * (r*r)) / sigmaQuadr)+0.5);
        }

        for(int g = 0; g < hLength; g++){

            hotspotGewichtung = hotspotGewichtung + h[g];
        }

        for(int spalte = 0; spalte < image->width(); spalte++){
            for(int zeile = 0; zeile < image->height(); zeile++){

                int cbTemp = 128+((qRed(image->pixel(spalte,zeile)) * -0.169) + (qGreen(image->pixel(spalte,zeile))* -0.331) + (qBlue(image->pixel(spalte,zeile))*0.5));
                int crTemp = 128+((qRed(image->pixel(spalte,zeile)) * 0.5) + (qGreen(image->pixel(spalte,zeile))* -0.419) + (qBlue(image->pixel(spalte,zeile))*-0.081));
                int sumY = 0;


                for (int s = (-steps); s <= steps; s++){
                    int yTemp = 0;

                    if((spalte+s) < 0 || (spalte+s) >= (image->height())) {

                        yTemp = getYfromRGB(image->pixel(spalte,zeile));

                    }

                    else{
                        yTemp = getYfromRGB(image->pixel(spalte+s,zeile));

                    }

                    int c = h[s+steps];

                    sumY += c * yTemp;

                }
                int newY = (int) (sumY/hotspotGewichtung);

                QRgb color = convertYcbcrToRgb(newY, cbTemp, crTemp);

                imageCopy->setPixel(spalte,zeile, color);
            }
        }
        //Erstelle neues Bild, Gauß in andere Richtung ausführen -> 2D
        imageGauss = new QImage(*imageCopy);
        for(int spalte = 0; spalte < image->width(); spalte++){
            for(int zeile = 0; zeile < image->height(); zeile++){

                int cbTemp = 128+((qRed(image->pixel(spalte,zeile)) * -0.169) + (qGreen(image->pixel(spalte,zeile))* -0.331) + (qBlue(image->pixel(spalte,zeile))*0.5));
                int crTemp = 128+((qRed(imageGauss->pixel(spalte,zeile)) * 0.5) + (qGreen(imageGauss->pixel(spalte,zeile))* -0.419) + (qBlue(imageGauss->pixel(spalte,zeile))*-0.081));
                int sumY = 0;


                for (int s = (-steps); s <= steps; s++){
                    int yTemp = 0;

                    if((zeile+s) < 0 || (zeile+s) >= (image->width())) {
                        yTemp = getYfromRGB(imageGauss->pixel(spalte,zeile));
                    }

                    else{
                        yTemp = getYfromRGB(imageGauss->pixel(spalte,zeile+s));

                    }
                    int c = h[s+steps];
                    sumY += c * yTemp;
                }
                int newY = (int) (sumY/hotspotGewichtung);

                QRgb color = convertYcbcrToRgb(newY, cbTemp, crTemp);
                imageCopy->setPixel(spalte,zeile,color);
            }
        }

        imageLabel->setPixmap(QPixmap::fromImage(*imageCopy));
    }}



void ImageViewer::kantePrewitt(){
    if (image != NULL){
        *imageCopy = *image;

        int arr1[3][3] = {{-1,0,1},{-1,0,1},{-1,0,1}};

        int arr2[3][3] = {{-1,-1,-1},{0,0,0},{1,1,1}};

        // Iterriere durch Bild pro Pixel beide Arrays, Gewichte Pixelwerte mit den Arrays
        for (int zeile = 0; zeile < image->height(); zeile++){
            for (int spalte = 0; spalte < image->width(); spalte++){

                int sum1 = 0;
                int sum2 = 0;
                int addWert = 0;


                for (int zeileArr = 0; zeileArr <= 2; zeileArr++){
                    for(int spalteArr = 0; spalteArr <= 2 ; spalteArr++){

                        if((zeile-1 <0) || (zeile+1 >= image->height()) || (spalte-1 <0) || (spalte+1 >= image->width()) ){
                            addWert =qRed(image->pixel(spalte, zeile));
                        }

                        else if(zeileArr == 0){
                            if (spalteArr == 0){addWert = qRed(image->pixel(spalte-1, zeile-1)); } //spalte ist x, zeiel ist y
                            else if(spalteArr ==1){addWert = qRed(image->pixel(spalte, zeile-1));}
                            else if(spalteArr == 2){addWert = qRed(image->pixel(spalte+1, zeile-1));}
                        }

                        else if(zeileArr == 1){
                            if (spalteArr == 0){addWert = qRed(image->pixel(spalte-1, zeile)); } //spalte ist x, zeiel ist y
                            else if(spalteArr ==1){addWert = qRed(image->pixel(spalte, zeile));}
                            else if(spalteArr == 2){addWert = qRed(image->pixel(spalte+1, zeile));}
                        }

                        else if(zeileArr == 2){
                            if (spalteArr == 0){addWert = qRed(image->pixel(spalte-1, zeile+1)); } //spalte ist x, zeiel ist y
                            else if(spalteArr ==1){addWert = qRed(image->pixel(spalte, zeile+1));}
                            else if(spalteArr == 2){addWert = qRed(image->pixel(spalte+1, zeile+1));}
                        }

                        int a1 = arr1[zeileArr][spalteArr];
                        int a2 = arr2[zeileArr][spalteArr];

                        sum1 += a1 * addWert;
                        sum2 += a2 * addWert;

                    }
                }
                int mittelung = std::sqrt((sum1*sum1) + (sum2*sum2));
                int richtung = std::atan2(sum1, sum2);

                imageCopy->setPixel(spalte, zeile, qRgb(mittelung,mittelung,mittelung));
            }
        }
        imageLabel->setPixmap(QPixmap::fromImage(*imageCopy));
    }



}


// Funktioniert wie kantePrewitt(),
void ImageViewer::sobel(){
    if (image != NULL){
        *imageCopy = *image;

        int arr1[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};

        int arr2[3][3] = {{-1,-2,-1},{0,0,0},{1,2,1}};


        for (int zeile = 0; zeile < image->height(); zeile++){
            for (int spalte = 1; spalte < image->width(); spalte++){

                int sum1 = 0;
                int sum2 = 0;
                int addWert = 0;


                for (int zeileArr = 0; zeileArr <= 2; zeileArr++){
                    for(int spalteArr = 0; spalteArr <= 2 ; spalteArr++){

                        if((zeile-1 <0) || (zeile+1 >= image->height()) || (spalte-1 <0) || (spalte+1 >= image->width()) ){
                            addWert =qRed(image->pixel(spalte, zeile));
                        }

                        else if(zeileArr == 0){
                            if (spalteArr == 0){addWert = qRed(image->pixel(spalte-1, zeile-1)); } //spalte ist x, zeiel ist y
                            else if(spalteArr ==1){addWert = qRed(image->pixel(spalte, zeile-1));}
                            else if(spalteArr == 2){addWert = qRed(image->pixel(spalte+1, zeile-1));}
                        }

                        else if(zeileArr == 1){
                            if (spalteArr == 0){addWert = qRed(image->pixel(spalte-1, zeile)); } //spalte ist x, zeiel ist y
                            else if(spalteArr ==1){addWert = qRed(image->pixel(spalte, zeile));}
                            else if(spalteArr == 2){addWert = qRed(image->pixel(spalte+1, zeile));}
                        }

                        else if(zeileArr == 2){
                            if (spalteArr == 0){addWert = qRed(image->pixel(spalte-1, zeile+1)); } //spalte ist x, zeiel ist y
                            else if(spalteArr ==1){addWert = qRed(image->pixel(spalte, zeile+1));}
                            else if(spalteArr == 2){addWert = qRed(image->pixel(spalte+1, zeile+1));}
                        }

                        int a1 = arr1[zeileArr][spalteArr];
                        int a2 = arr2[zeileArr][spalteArr];

                        sum1 += a1 * addWert;
                        sum2 += a2 * addWert;

                    }
                }
                int mittelung = std::sqrt((sum1*sum1) + (sum2*sum2));
                int richtung = std::atan2(sum1, sum2);

                imageCopy->setPixel(spalte, zeile, qRgb(mittelung,mittelung,mittelung));
            }
        }
        imageLabel->setPixmap(QPixmap::fromImage(*imageCopy));
    }}

// Unvollständig, einen Versuch war es wert..
void ImageViewer::cannyEdge(){


    *imageGray = *image;
    *imageCopy = *image;
    int w = image->width();
    int he = image->height();

    float sigma = sigmaInput->text().toFloat();
    int hotspotGewichtung = 0;

    //GRAUSTUFEN
    for (int i = 0; i <= w-1; i++){
        for(int j = 0; j <= he-1; j++ ){
            QRgb color = image->pixel(i,j);
            int gray = ((qRed(color) *0.299) + (qGreen(color)* 0.587) + (qBlue(color)*0.114));
            imageGray->setPixel(i, j, qRgb(gray,gray,gray));
        }
    }

    //GAUSS KERNEL
    int center = (int) ((3.0 * sigma)+0.5);

    std::vector<float> h((2*center)+1); //odd size weil

    int hLength = h.size();
    int steps = hLength/2;

    //fill the kernel h:
    double sigmaQuadr = sigma *sigma; //sigma²

    for (int i = 0; i < hLength; i++){ //setzt die index werte in die gauss funktion ein, index verschiebt sich dass mitte des arrays auf null ist. sigma quadrat unverändert über alle schleifen
        double r = center -i;

        h[i] = ((float) std::exp((-0.5 * (r*r)) / sigmaQuadr)+0.5);
    }

    for(int g = 0; g < hLength; g++){

        hotspotGewichtung = hotspotGewichtung + h[g];
    }

    for(int spalte = 0; spalte < image->width(); spalte++){
        for(int zeile = 0; zeile < image->height(); zeile++){

            int sumred = 0;


            for (int s = (-steps); s <= steps; s++){
                int yTemp = 0;

                if((spalte+s) < 0 || (spalte+s) >= (image->height())) {

                    yTemp = qRed(imageGray->pixel(spalte,zeile));
                }
                else{
                    yTemp = qRed(imageGray->pixel(spalte+s,zeile));

                }
                int c = h[s+steps];

                sumred += c * yTemp;

            }
            int newY = (int) (sumred/hotspotGewichtung);

            imageCopy->setPixel(spalte,zeile, qRgb(newY,newY,newY));

        }

    }

    imageEdgeDir = new QImage(*image);
    imageGradient = new QImage(*image); //=Gradient ist ergebnis des Sobel-Algorithmus mit sqrt(gx² + gy²)
    imageGauss = new QImage(*imageCopy);

    for(int spalte = 0; spalte < image->width(); spalte++){
        for(int zeile = 0; zeile < image->height(); zeile++){

            int sumred = 0;


            for (int s = (-steps); s <= steps; s++){
                int yTemp = 0;

                if((zeile+s) < 0 || (zeile+s) >= (image->width())) {
                    yTemp = qRed(imageCopy->pixel(spalte,zeile));
                }

                else{
                    yTemp = qRed(imageCopy->pixel(spalte,zeile+s));

                }
                int c = h[s+steps];
                sumred += c * yTemp;
            }
            int newY = (int) (sumred/hotspotGewichtung);

            imageGauss->setPixel(spalte,zeile,qRgb(newY,newY,newY));
        }
    }

    // bis hier: Gauß Graustufenbild wird erzeugt



    //-----------------------------------------------


    // Sobel (funktioniert wie sobel() )

    int arr1[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
    int arr2[3][3] = {{-1,-2,-1},{0,0,0},{1,2,1}};
    for (int zeile = 0; zeile < image->height(); zeile++){
        for (int spalte = 1; spalte < image->width(); spalte++){

            int sum1 = 0;
            int sum2 = 0;
            int addWert = 0;


            for (int zeileArr = 0; zeileArr <= 2; zeileArr++){
                for(int spalteArr = 0; spalteArr <= 2 ; spalteArr++){

                    if((zeile -1 <0) || (zeile >= image->height()) || (spalte -1 <0) || (spalte >= image->width()) ){
                        addWert =qRed(imageGauss->pixel(spalte, zeile));
                    }

                    else if(zeileArr == 0){
                        if (spalteArr == 0){addWert = qRed(imageGauss->pixel(spalte-1, zeile-1)); } //spalte ist x, zeiel ist y
                        else if(spalteArr ==1){addWert = qRed(imageGauss->pixel(spalte, zeile-1));}
                        else if(spalteArr == 2){addWert = qRed(imageGauss->pixel(spalte+1, zeile-1));}
                    }

                    else if(zeileArr == 1){
                        if (spalteArr == 0){addWert = qRed(imageGauss->pixel(spalte-1, zeile)); } //spalte ist x, zeiel ist y
                        else if(spalteArr ==1){addWert = qRed(imageGauss->pixel(spalte, zeile));}
                        else if(spalteArr == 2){addWert = qRed(imageGauss->pixel(spalte+1, zeile));}
                    }

                    else if(zeileArr == 2){
                        if (spalteArr == 0){addWert = qRed(imageGauss->pixel(spalte-1, zeile+1)); } //spalte ist x, zeiel ist y
                        else if(spalteArr ==1){addWert = qRed(imageGauss->pixel(spalte, zeile+1));}
                        else if(spalteArr == 2){addWert = qRed(imageGauss->pixel(spalte+1, zeile+1));}
                    }

                    int a1 = arr1[zeileArr][spalteArr];
                    int a2 = arr2[zeileArr][spalteArr];

                    sum1 += a1 * addWert;
                    sum2 += a2 * addWert;

                }
            }
            int mittelung = std::sqrt((sum1*sum1) + (sum2*sum2));
            int richtung = std::atan2(sum1, sum2);

            imageEdgeDir->setPixel(spalte, zeile, qRgb(richtung, richtung, richtung)); //gradient

            imageGradient->setPixel(spalte, zeile, qRgb(mittelung,mittelung,mittelung)); //sobel ergebnis
        }
    }

    //SOBEL FERTIG; JETZT EDGETRACING----------------------------------------------------------------------------------
    // höchsten und niedrigsten grauwert finden


    // ab hier sind wir leider nicht weiter gekommen

    int max = 0;
    int min = 1000;

    for (int zeile = 0; zeile < image->height(); zeile++){
        for (int spalte = 0; spalte < image->width(); spalte++){

            if(qRed(imageGradient->pixel(zeile, spalte)) < min){
                min = (qRed(imageGradient->pixel(zeile, spalte)));
            }
            else if(qRed(imageGradient->pixel(zeile, spalte)) > max){
                max = (qRed(imageGradient->pixel(zeile, spalte)));
            }
        }
    }

    int Temp = -1;

    //    for (int zeile = 0; zeile < image->height(); zeile++){
    //            for (int spalte = 0; spalte < image->width(); spalte++){

    //                if(qRed(imageGradient->pixel(zeile, spalte)) >
    //            }
    //            }



    imageLabel->setPixmap(QPixmap::fromImage(*imageEdgeDir));
}


// Unsharp Masking, Ergebnis nicht optimal
void ImageViewer::USM(){


    *imageGray = *image;
    *imageCopy = *image;
    imageMask = new QImage(*image);

    int w = image->width();
    int he = image->height();

    float sigma = sigmaInputUSM->text().toFloat();
    int hotspotGewichtung = 0;

    //Graustufen
    for (int i = 0; i <= w-1; i++){
        for(int j = 0; j <= he-1; j++ ){
            QRgb color = image->pixel(i,j);
            int gray = ((qRed(color) *0.299) + (qGreen(color)* 0.587) + (qBlue(color)*0.114));
            imageGray->setPixel(i, j, qRgb(gray,gray,gray));
        }
    }

    //Gaus Kernell
    int center = (int) ((3.0 * sigma)+0.5);

    std::vector<float> h((2*center)+1); //odd size weil

    int hLength = h.size();
    int steps = hLength/2;

    //fill the kernel h:
    double sigmaQuadr = sigma *sigma; //sigma²

    for (int i = 0; i < hLength; i++){ //setzt die index werte in die gauss funktion ein, index verschiebt sich dass mitte des arrays auf null ist. sigma quadrat unverändert über alle schleifen
        double r = center -i;

        h[i] = ((float) std::exp((-0.5 * (r*r)) / sigmaQuadr)+0.5);
    }

    for(int g = 0; g < hLength; g++){

        hotspotGewichtung = hotspotGewichtung + h[g];
    }

    for(int spalte = 0; spalte < image->width(); spalte++){
        for(int zeile = 0; zeile < image->height(); zeile++){

            int sumred = 0;


            for (int s = (-steps); s <= steps; s++){
                int yTemp = 0;

                if((spalte+s) < 0 || (spalte+s) >= (image->height())) {

                    yTemp = qRed(imageGray->pixel(spalte,zeile));
                }
                else{
                    yTemp = qRed(imageGray->pixel(spalte+s,zeile));

                }
                int c = h[s+steps];

                sumred += c * yTemp;

            }
            int newY = (int) (sumred/hotspotGewichtung);

            imageCopy->setPixel(spalte,zeile, qRgb(newY,newY,newY));

        }

    }

    // ziehe imagecopy=geglättet von imagegrey ab
    for(int spalte = 0; spalte < image->width(); spalte++){
        for(int zeile = 0; zeile < image->height(); zeile++){
            int subWert = qRed(imageGray->pixel(spalte, zeile)) - qRed(imageCopy->pixel(spalte, zeile));
            imageMask->setPixel(spalte, zeile, qRgb(subWert,subWert,subWert));
        }
    }
    // erstelle USM-Matrix, ähnlich dem Laplace (siehe Vorlesungsfolien) , (Quelle: https://en.wikipedia.org/wiki/Unsharp_masking)
    float aUSMfloat = aUSM->text().toFloat();
    float arr1[3][3] = {{0.0f,(-1.0f * aUSMfloat), 0.0f},
                        {(-1.0f * aUSMfloat),(4.0f * aUSMfloat)+1.0f,(-1.0f * aUSMfloat)},
                        {0.0f,(-1.0f * aUSMfloat), 0.0f}};

    for (int zeile = 0; zeile < image->height(); zeile++){
        for (int spalte = 1; spalte < image->width(); spalte++){

            float sum1 = 0.0;
            int addWert = 0;


            for (int zeileArr = 0; zeileArr <= 2; zeileArr++){
                for(int spalteArr = 0; spalteArr <= 2 ; spalteArr++){

                    if((zeile -1 <0) || ((zeile+1) >= image->height()) || (spalte -1 < 0) || ((spalte+1) >= image->width()) ){
                        addWert = qRed(imageMask->pixel(spalte, zeile));
                    }
                    else if(zeileArr == 0){
                        if (spalteArr == 0){addWert = qRed(imageMask->pixel(spalte-1, zeile-1)); } //spalte ist x, zeile ist y
                        else if(spalteArr ==1){addWert = qRed(imageMask->pixel(spalte, zeile-1));}
                        else if(spalteArr == 2){addWert = qRed(imageMask->pixel(spalte+1, zeile-1));}
                    }
                    else if(zeileArr == 1){
                        if (spalteArr == 0){addWert = qRed(imageMask->pixel(spalte-1, zeile)); } //spalte ist x, zeile ist y
                        else if(spalteArr ==1){addWert = qRed(imageMask->pixel(spalte, zeile));}
                        else if(spalteArr == 2){addWert = qRed(imageMask->pixel(spalte+1, zeile));}
                    }
                    else if(zeileArr == 2){
                        if (spalteArr == 0){addWert = qRed(imageMask->pixel(spalte-1, zeile+1)); } //spalte ist x, zeile ist y
                        else if(spalteArr ==1){addWert = qRed(imageMask->pixel(spalte, zeile+1));}
                        else if(spalteArr == 2){addWert = qRed(imageMask->pixel(spalte+1, zeile+1));}
                    }
                    float a1 = arr1[zeileArr][spalteArr];
                    sum1 += a1 * addWert;
                }
            }

            // Addiere gewichteten Maskenpixel zu Originalbild
            int gewWert = sum1 + qRed(imageGray->pixel(spalte, zeile));
            imageCopy->setPixel(spalte, zeile, qRgb(gewWert, gewWert,gewWert));
        }
    }
    imageLabel->setPixmap(QPixmap::fromImage(*imageCopy));
}



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

    deleteCrossButton = new QPushButton();
    deleteCrossButton->setText("Kreuz entfernen");
    QObject::connect(deleteCrossButton, SIGNAL (clicked()), this, SLOT (deleteCross()));

    QObject::connect(sliderCross, SIGNAL(valueChanged(int)),this,SLOT(applyCross(int)));

    m_option_layout1->addWidget(sliderCross);
    m_option_layout1->addWidget(deleteCrossButton);

    tabWidget->addTab(m_option_panel1,"Aufgabenblatt 1");


    //  ******************************    Aufgabe 2    ******************************  //

    m_option_panel2 = new QWidget();
    m_option_layout2 = new QVBoxLayout();
    m_option_panel2->setLayout(m_option_layout2);
    m_option_layout2->setSpacing(0);
    m_option_layout2->setContentsMargins(0,0,0,0);

    buttonVarianz = new QPushButton();
    buttonVarianz ->setText("Varianz");
    QObject::connect(buttonVarianz, SIGNAL (clicked()), this, SLOT (varianzGraufstufe()));


    labelMiddleBrightness = new QLabel();
    labelVarianz = new QLabel();

    buttonHistogram = new QPushButton();
    buttonHistogram->setText("Draw Logarithmical Histogram");
    QObject::connect(buttonHistogram, SIGNAL (clicked()), this, SLOT (histogramLog()));

    sliderBit = new QSlider(Qt::Orientation::Horizontal, this);
    sliderBit->setRange(1,8);
    sliderBit->setToolTip("Bit Slider");
    sliderBit->setTickPosition(QSlider::TicksBelow);
    sliderBit->setTickInterval(1);

    QObject::connect(sliderBit, SIGNAL(valueChanged(int)),this,SLOT(bitDynamikGraustufe(int)));
    meinHistogram = new QLabel(this);
    meinHistogramLog = new QLabel(this);


    QSlider *sliderBright = new QSlider(Qt::Horizontal,0);
    sliderBright->setRange(-255,255);
    sliderBright->setToolTip("Helligkeitsslider");
    sliderBright->setTickPosition(QSlider::TicksBelow);
    sliderBright->setTickInterval(50);
    connect(sliderBright, SIGNAL(valueChanged(int)),this, SLOT(brightnessGraustufe(int)));

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


    m_option_layout2->addWidget(meinHistogram);
    QFrame *line=new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    m_option_layout2->addWidget(meinHistogramLog);
    m_option_layout2->addWidget(labelMiddleBrightness);
    m_option_layout2->addWidget(labelVarianz);
    m_option_layout2->addWidget(buttonVarianz);
    m_option_layout2->addWidget(buttonHistogram);
    m_option_layout2->addWidget(sliderBit);
    m_option_layout2->addWidget(sliderBright);
    m_option_layout2->addWidget(sliderKontrast);
    m_option_layout2->addWidget(autoKontrastSlider);

    tabWidget->addTab(m_option_panel2,"Aufgabenblatt 2");
    tabWidget->show();


    //  ******************************    Aufgabe 3    ******************************  //

    m_option_panel3 = new QWidget();
    m_option_layout3 = new QVBoxLayout();
    m_option_panel3 ->setLayout(m_option_layout3);

    sigmaInput1 = new QLineEdit();
    sigmaInput1->setText("Bitte einen Wert zwischen 1 und 5 für den Gauß Filter eintragen");


    QSlider *sliderColorBit = new QSlider(Qt::Horizontal,0);
    sliderColorBit->setRange(1,8);
    sliderColorBit->setTickPosition(QSlider::TicksBelow);
    sliderColorBit->setTickInterval(1);
    sliderColorBit->setToolTip("Bit-Dynamik Farbe");
    connect(sliderColorBit, SIGNAL(valueChanged(int)),this, SLOT(bitDynamikColor(int)));

    QSlider *sliderColorBrightness = new QSlider(Qt::Horizontal,0);
    sliderColorBrightness->setRange(-255,255);
    sliderColorBrightness->setTickPosition(QSlider::TicksBelow);
    sliderColorBrightness->setTickInterval(10);
    sliderColorBrightness->setToolTip("Helligkeitsslider für Farben");
    connect(sliderColorBrightness, SIGNAL(valueChanged(int)),this, SLOT(brightnessColor(int)));

    QSlider *sliderKontrastColor = new QSlider(Qt::Horizontal,0);
    sliderKontrastColor->setRange(-10,10);
    sliderKontrastColor->setTickPosition(QSlider::TicksBelow);
    sliderKontrastColor->setTickInterval(1);
    sliderKontrastColor->setToolTip("Kontrastslider Farbe");
    connect(sliderKontrastColor, SIGNAL(valueChanged(int)),this, SLOT(kontrastColor(int)));

    QSlider *autoKontrastColorSlider = new QSlider(Qt::Horizontal,0);
    autoKontrastColorSlider->setRange(1,100);
    autoKontrastColorSlider->setTickPosition(QSlider::TicksBelow);
    autoKontrastColorSlider->setTickInterval(1);
    autoKontrastColorSlider->setToolTip("Autokontrastslider Farbe");
    connect(autoKontrastColorSlider, SIGNAL(valueChanged(int)),this, SLOT(autoKontrastColor(int)));

    linearFilterxSliderLabel = new QLabel(this);
    linearFilterxSliderLabel->setText("Bewege Slider um den X Wert zu setzen");

    linearFilterySliderLabel = new QLabel(this);
    linearFilterySliderLabel->setText("Bewege Slider um den Y Wert zu setzen");

    QSlider *xLinearFilterSlider = new QSlider(Qt::Horizontal,0);
    xLinearFilterSlider->setRange(1,15);
    xLinearFilterSlider->setTickPosition(QSlider::TicksBelow);
    xLinearFilterSlider->setTickInterval(1);
    xLinearFilterSlider->setToolTip("X-Achse Linearer Filter");
    connect(xLinearFilterSlider, SIGNAL(valueChanged(int)),this, SLOT(setLinearXValue(int)));



    QSlider *yLinearFilterSlider = new QSlider(Qt::Horizontal,0);
    yLinearFilterSlider->setRange(1,15);
    yLinearFilterSlider->setTickPosition(QSlider::TicksBelow);
    yLinearFilterSlider->setTickInterval(1);
    yLinearFilterSlider->setToolTip("Y-Achse Linearer Filter");
    connect(yLinearFilterSlider, SIGNAL(valueChanged(int)),this, SLOT(setLinearYValue(int)));

    buttonGenerateLinearFilterTable = new QPushButton();
    buttonGenerateLinearFilterTable->setText("Generiere Tabelle");
    QObject::connect(buttonGenerateLinearFilterTable, SIGNAL (clicked()), this, SLOT (generateFilterTable()));

    linearFilterTable = new QTableWidget();
    linearFilterTable = new QTableWidget(0,0,this);


    myHistogramColorLabel = new QLabel(this);


    colorHistgoramButton = new QPushButton();
    colorHistgoramButton->setText("colorHistgoramButton");
    QObject::connect(colorHistgoramButton, SIGNAL (clicked()), this, SLOT (histogramColor()));

    linearFilterButton = new QPushButton();
    linearFilterButton->setText("linearFilterButton");
    QObject::connect(linearFilterButton, SIGNAL (clicked()), this, SLOT (linearFilter()));

    linearFilterZeroButton = new QPushButton();
    linearFilterZeroButton->setText("linearFilterZeroButton");
    QObject::connect(linearFilterZeroButton, SIGNAL (clicked()), this, SLOT (linearFilterZeroPadding()));

    linearFilterKonstButton = new QPushButton();
    linearFilterKonstButton->setText("linearFilterKonstButton");
    QObject::connect(linearFilterKonstButton, SIGNAL (clicked()), this, SLOT (linearFilterKonstPadding()));

    linearFilterReflectionButton = new QPushButton();
    linearFilterReflectionButton->setText("linearFilterReflectionButton");
    QObject::connect(linearFilterReflectionButton, SIGNAL (clicked()), this, SLOT (linearFilterReflectionPadding()));

    gaussFilterButton = new QPushButton();
    gaussFilterButton->setText("Gauss2DButton");
    QObject::connect(gaussFilterButton, SIGNAL (clicked()), this, SLOT (doubleDGauss()));



    m_option_layout3->addWidget(sliderColorBit);
    m_option_layout3->addWidget(sliderColorBrightness);
    m_option_layout3->addWidget(sliderKontrastColor);
    m_option_layout3->addWidget(myHistogramColorLabel);
    m_option_layout3->addWidget(colorHistgoramButton);
    m_option_layout3->addWidget(autoKontrastColorSlider);


    tabWidget->addTab(m_option_panel3,"Aufgabenblatt 3");


    m_option_panel4 = new QWidget();
    m_option_layout4 = new QVBoxLayout();
    m_option_panel4 ->setLayout(m_option_layout4);

    m_option_layout4->addWidget(linearFilterxSliderLabel);
    m_option_layout4->addWidget(xLinearFilterSlider);
    m_option_layout4->addWidget(linearFilterySliderLabel);
    m_option_layout4->addWidget(yLinearFilterSlider);
    m_option_layout4->addWidget(buttonGenerateLinearFilterTable);
    m_option_layout4->addWidget(linearFilterTable);
    m_option_layout4->addWidget(linearFilterButton);
    m_option_layout4->addWidget(linearFilterZeroButton);
    m_option_layout4->addWidget(linearFilterZeroButton);
    m_option_layout4->addWidget(linearFilterKonstButton);
    m_option_layout4->addWidget(linearFilterReflectionButton);
    m_option_layout4->addWidget(sigmaInput1);
    m_option_layout4->addWidget(gaussFilterButton);
    tabWidget->addTab(m_option_panel4,"Aufgabenblatt 3.2");


    //****************************************ÜBUNG 4************************************************//
    m_option_panel5 = new QWidget();
    m_option_layout5 = new QVBoxLayout();
    m_option_panel5 ->setLayout(m_option_layout5);


    prewittButton = new QPushButton();
    prewittButton->setText("Prewitt");
    QObject::connect(prewittButton, SIGNAL (clicked()), this, SLOT (kantePrewitt()));


    sobelButton = new QPushButton();
    sobelButton->setText("Sobel");
    QObject::connect(sobelButton, SIGNAL (clicked()), this, SLOT (sobel()));

    sigmaInput = new QLineEdit();
    sigmaInput->setText("Bitte einen Wert zwischen 1 und 5 für den Canny Edge Filter eintragen");


    cannyEdgeButton = new QPushButton();
    cannyEdgeButton->setText("canny Edge");
    QObject::connect(cannyEdgeButton, SIGNAL (clicked()), this, SLOT (cannyEdge()));

    sigmaInputUSM = new QLineEdit();
    sigmaInputUSM->setText("Bitte einen Wert zwischen 1 und 5 für den USM Filter eintragen");


    aUSM = new QLineEdit();
    aUSM->setText("Bitte einen Wert zwischen 0.2 und 4 für den USM Filter eintragen");


    buttonUSM = new QPushButton();
    buttonUSM->setText("USM");
    QObject::connect(buttonUSM, SIGNAL (clicked()), this, SLOT (USM()));

    m_option_layout5->addWidget(prewittButton);
    m_option_layout5->addWidget(sobelButton);
    m_option_layout5->addWidget(sigmaInput);
    m_option_layout5->addWidget(cannyEdgeButton);
    m_option_layout5->addWidget(sigmaInputUSM);
    m_option_layout5->addWidget(aUSM);
    m_option_layout5->addWidget(buttonUSM);
    tabWidget->addTab(m_option_panel5,"Aufgabenblatt 4");



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
    createImageBlackCopy();
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
