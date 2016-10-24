#include "computeimage.h"

ComputeImage::ComputeImage()
{

}

ComputeImage::ComputeImage(Image file)
{
    //initialisation of vars and constants.
    angleMaxValue = pow(2, 15);
    pi = atan(1) * 4;

    //The max angle in cfg file is store as degrees; converting it to radians.
    maxAngleX = 20 * pi / 180;
    maxAngleY = 20 * pi / 180;

    //Prepare the tan of the scan max angle.
    //With python it speeds up the calculus, but it seems C++ doesn't care!
    tanXScan = tan(maxAngleX);
    tanYScan = tan(maxAngleY);

    //get the values from image.
    image = file.getNegative();
    widthPix = file.getWidthPix();
    heightPix = file.getHeightPix();
    size = widthPix * heightPix;
    widthMm = file.getWidthMm();
    heightMm = file.getHeightMm();
    //TODO: distance and speed values need to be updated when modified in the GUI.
    speed = file.getSpeed();
    //distance = file.getDistance();
    //Temporary distance value for tests.
    distance = 900;

    supportWidth = file.getSupportWidth();
    supportHeight = file.getSupportHeight();

    //compute the minimum distance from laser to support,
    //given image size and angle value.
    if(widthPix > heightPix){
        minDistance = widthMm / (2 * tanXScan);
    } else {
        minDistance = heightMm / (2 * tanYScan);
    }

    //Get the ratio pix to mm, using the greatest image side to minimize errors.
    if (widthPix > heightPix)
    {
        ratioPixMm = double(widthMm) / widthPix;
    } else {
        ratioPixMm = double(heightMm) / heightPix;
    }

    cout << "ratio pix/mm: " << ratioPixMm << endl;
    cout << "width mm: " << widthMm << endl;
    cout << "distance: " << distance << endl;
    cout << "width pix: " << widthPix << endl;
    cout << "max angle X: " << maxAngleX << endl;
    cout << "max pos: " << angleMaxValue << endl;


}

//Update the max size of the picture, given:
//The distance from laser to support, from user or project file,
//The max angles of the laser, from the general config file.
void ComputeImage::updateMaxSize()
{
    halfMaxSizeX = distance * tanXScan;
    halfMaxSizeY = distance * tanYScan;
    maxSizeX = 2 * halfMaxSizeX;
    maxSizeY = 2 * halfMaxSizeY;
    cout << "max size X: " << maxSizeX << endl;

}

//Look at each pixel value of the image to create a string
//that will be sent to the laser.
//This string is like:
//data = IidLlvalueXxvalueYyvalueSspeede.getSe.getS
void ComputeImage::computeCoords(vector<QString>* serialData, QProgressBar* progressBar)
{
    //Get the last distance value and compute angle pos for every pixel.
    updateMaxSize();
    computeAngles();

    time_t timer;
    int debut = time(&timer);

//    int index = 0;
    double percent = 0;

    QString dataToSend = "";

    for(int j = 0; j<heightPix; j++)
    {
        dataToSend = "X";
        dataToSend += QString::number(angleValueX[0]);
        dataToSend += "Y";
        dataToSend += QString::number(angleValueY[j]);
        dataToSend += "L0";
        dataToSend += "M0";
        dataToSend += '\n';
        serialData->push_back(dataToSend);
        //uchar * line = image.scanLine(j);

//        cout << "début boucle" << endl;
//        cout << dataToSend.toStdString();


        for(int i = 0; i<widthPix; i++)
        {
            QRgb pix = qBlue(image.pixel(i, j));
            //TODO: See how to add a value to the pointer "cleanly".
            //QRgb pix = (QRgb)*(line + j * 4);

            //Pixels that are white don't need to be compute
            //TODO: verify the last value computed: if a pix value is 0
            // and the previous one was something else, it must be set to 0
            // to cut the laser.
            percent = ((double)j * widthPix + i) * 100 / size;
            progressBar->setValue(ceil(percent));

            if(pix == 0)
            {
                continue;
            }

            //Build the strings to be sent to the laser by serial.
//            QString dataToSend = "I";
//            dataToSend += QString::number(index++);
            dataToSend = "X";
            dataToSend += QString::number(angleValueX[i]);
//            dataToSend += "Y";
//            dataToSend += QString::number(angleValueY[j]);
            dataToSend += "L";
            dataToSend += QString::number(pix);
            dataToSend += "M0";
            dataToSend += "S";
            dataToSend += "5000";
            dataToSend += '\n';
            serialData->push_back(dataToSend);
//            cout << dataToSend.toStdString();
        }

        serialData->push_back("L0M0\n");

    }

/*
    string adresse = "sortie_compute.txt";
    ofstream sortieCompute(adresse);

    if(!sortieCompute)
    {
        WinInfo::info("ouverture impossible");
    }

    int taille = serialData->size();
    for(int i = 0; i < 2100; i++)
    {
        sortieCompute << serialData->at(i).toStdString();
    }

*/
    int duree = time(&timer) - debut;
    QString message = "Position values computed in ";
    message += QString::number(duree);
    message += " seconds";

//    WinInfo::info(message);

}

void ComputeImage::computeSupport(vector<QString> *serialData)
{
    double angleValue = 0;
    double angleRatio = 0;
    double halfSize = 0;
    int widthValue = 0;
    int heightValue = 0;

    halfSize = (supportWidth / 2) * ratioPixMm;
    angleValue = atan(halfSize * tanXScan / halfMaxSizeX);
    angleRatio = angleValue / maxAngleX;
    widthValue = angleMaxValue * angleRatio;

    halfSize = (supportHeight / 2) * ratioPixMm;
    angleValue = atan(halfSize * tanYScan / halfMaxSizeY);
    angleRatio = angleValue / maxAngleY;
    heightValue = angleMaxValue * angleRatio;

    QString dataToSend = "L255";
    dataToSend += "X";
    dataToSend += QString::number(widthValue);
    dataToSend += "M0";
    dataToSend += "L255";
    dataToSend += "\n";

    serialData->push_back(dataToSend);

    dataToSend = "Y";
    dataToSend += QString::number(heightValue);
    dataToSend += "M0";
    dataToSend += "L255";
    dataToSend += "\n";

    serialData->push_back(dataToSend);

    dataToSend = "X";
    dataToSend += QString::number(-widthValue);
    dataToSend += "M0";
    dataToSend += "L255";
    dataToSend += "\n";

    serialData->push_back(dataToSend);

    dataToSend = "Y";
    dataToSend += QString::number(-heightValue);
    dataToSend += "M0";
    dataToSend += "L255";
    dataToSend += "\n";

    serialData->push_back(dataToSend);

}

//This function precomputes the angles  corresponding to each vertical and horizontal pixel
//That way computeCoord() can get it ready to be used in two tables, instead of asking it on each iteration
void ComputeImage::computeAngles()
{
    //Clear the two angle tables before to populate it.
    angleValueX.clear();
    angleValueY.clear();

    time_t timer;
    int debut = time(&timer);

    //Initlize some vars for computing temporary values
    double angleValue = 0;
    double angleRatio = 0;
    double halfSize = 0;
    int posValue = 0;


    //First get the values for width
    //
    //First is compute the distance between image centerand pix pos, in mm.
    //Then this value is used to find the corresponding angle.
    //Compute a ration between this angle and the max angle.
    //Finally use this ratio to get the laser position, by multiplying it by greatest posible value.
    //Store this value into the angleValue array.
    for(int i = 0; i<widthPix; i++)
    {
        halfSize = (i - widthPix / 2) * ratioPixMm;
        angleValue = atan(halfSize * tanXScan / halfMaxSizeX);
        angleRatio = angleValue / maxAngleX;
        posValue = angleMaxValue * angleRatio;
        angleValueX.push_back(posValue);
    }

    //The get the values for height
    for(int i = 0; i<heightPix; i++)
    {
        halfSize = (i - heightPix / 2) * ratioPixMm;
        angleValue = atan(halfSize * tanYScan / halfMaxSizeY);
        angleRatio = angleValue / maxAngleY;
        posValue = angleMaxValue * angleRatio;
        angleValueY.push_back(posValue);

    }

    int duree = time(&timer) - debut;
    QString message = "Angles computed in ";
    message += QString::number(duree);
    message += " seconds";

   // WinInfo::info(message);

}

int ComputeImage::getMinDistance()
{
    return minDistance;
}
