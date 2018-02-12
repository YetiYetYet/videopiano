#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <iostream>
#include <opencv/cv.hpp>
#include <SFML/Audio.hpp>

using namespace std;
using namespace cv;

bool piano[] = {1,0,1,1,0,1,0,1,
                1,0,1,0,1,0,1,1,
                0,1,0,1,1,0,1,0,
                1,0,1,1,0,1,0,1,
                1,0,1,0,1,0,1,1,
                0,1,0,1,1,0,1,0,
                1,0,1,1,0,1,0,1,
                1,0,1,0,1,0,1,1,
                0,1,0,1,1,0,1,0,
                1,0,1,1,0,1,0,1,
                1,0,1,0,1,0,1,1};

#define DEBUTCLAVIER 0

Vec3b intensity;
uchar blue;
uchar green;
uchar red;

bool initLine(Mat frame, float line[][4]){
    for (int i = 0; i < frame.cols; i++) {
        //cout << "colone : " << i << ", R : " << line[i][0] << ", G : " << line[i][1] << ", B : " << line[i][2] << endl;
        if (line[i][0] > 220 && line[i][1] > 220 && line[i][2] > 220) {
            //printf("i%d\n", i);
            line[i][3] = 1;
        }
        if (line[i][0] < 150 && line[i][1] < 150 && line[i][2] < 150) {
            line[i][3] = 2;
        }
    }

    int blanc = 0;
    int noir = 0;
    for (int i = 0; i < frame.cols; i++) {
        if (line[i][3] == 1) {
            blanc++;
        }
        if (line[i][3] == 2) {
            noir++;
        }
    }
    int verif = frame.cols/3;
    if(blanc < verif || noir < verif)
        return false;
    return true;
}

void initCoord(Mat frame, float line[][4], int coord[]){
    bool start = false;
    int tailletoucheblanche = 0;
    int debutclavier = DEBUTCLAVIER;
    for (int i = debutclavier; i < frame.cols - debutclavier; i++) {
        if (!start) {
            if (line[i][3] == 1) {
                start = !start;
                tailletoucheblanche++;
            }
        } else {
            if (line[i][3] == 2) {
                break;
            } else {
                tailletoucheblanche++;
            }
        }
    }
    cout << tailletoucheblanche << endl;
    int pointblanc = tailletoucheblanche / 2;
    int pointnoir = pointblanc;
    cout << pointblanc << " " << pointnoir << endl;

    bool startt = true;
    int startacc = 0;
    int acctouche = 0;
    for (int i = debutclavier; i < frame.cols - DEBUTCLAVIER; i++) {
        if (startt) {
            if (piano[acctouche] == true) {
                if (startacc > pointblanc) {
                    coord[acctouche] = i;
                    acctouche++;
                    startt = !startt;
                    startacc = 0;
                }
            }
            if (piano[acctouche] == false) {
                if (startacc > pointnoir) {
                    coord[acctouche] = i;
                    acctouche++;
                    startt = !startt;
                    startacc = 0;
                }
            }
            startacc++;
        } else {
            if (line[i][3] == 2) {
                startt = !startt;
            }
        }
    }

    for (int i = 0; i < 88; i++)
        cout << "i : " << i << " : " << coord[i] << ", ";
    cout << endl;
}

void detectMusic(Mat frame, int coord[], bool *present, int frows, sf::Music music[88]){
    bool futur[88];
    float anotherline[88][3];
    bool idetectsomething = false;

    for(int i = 0; i < 88; i++) {
        Vec3b intensity = frame.at<Vec3b>(Point(coord[i], frows));
        blue = intensity[0];
        green = intensity[1];
        red = intensity[2];
        anotherline[i][0] =  red;
        anotherline[i][1] =  green;
        anotherline[i][2] =  blue;
        if(((int)red < 90 && (int)blue < 90 && (int)green < 90) || ((int)red > 210 && (int)blue > 210 && (int)green > 210)) {
            futur[i] = false;
        }
        else {
            //cout << (int)red << " " << (int)green << " " << (int)blue << endl;
            futur[i] = true;
        }
    }

    for(int i = 0; i < 88; i++){
        if(!futur[i]==present[i]){
            idetectsomething = true;
            if(futur[i]) {
                printf("P : %d|", i);
                music[i].play();
            }
            else {
                //music[i].stop();
                printf("R : %d|", i);
            }
        }
        present[i]=futur[i];
    }
    if(idetectsomething)
        cout << endl;
    return;
}

void drawLine(Mat frame, int coord[], int frows){
    for (int i = 0; i < 88; i++) {
        for (int j = frows-20; j < frows+20; j++) {
            intensity.val[0] = 0;
            intensity.val[1] = 255;
            intensity.val[2] = 255;
            frame.at<Vec3b>(Point(coord[i], j)) = intensity;
        }
    }

    for (int i = 0; i < frame.cols; i++) {
        intensity.val[0]=0;
        intensity.val[1]=255;
        intensity.val[2]=255;
        frame.at<Vec3b>(Point(i,frows)) = intensity;
    }
    return;
}

void initMusic(sf::Music music[88]){
    string song;
    for(int i = 10; i < 78; i++){
        song = "/home/marjorie/CLionProjects/Piano_Alexis_BEHIER_14501367/song/" + to_string(i-10) + ".waw";
        music[i].openFromFile(song);
    }
    return;
}


int main(int argc, char** argv)
{
    VideoCapture cap("/home/marjorie/CLionProjects/Piano_Alexis_BEHIER_14501367/video2.mp4");
    Mat frame;
    Mat resizedFrame;

    sf::Music music[88];
    initMusic(music);

    int coord[88];
    bool s = true;
    bool present[88];
    float mul = 2;
    bool lineDetected;
    bool pause = false;

    while(true){

        // Capture frame-by-frame
        if(!pause) {
            cap >> frame;
            if (s) {
                for (int i = 0; i < 100; i++) {
                    cap >> frame;
                }
            }

            int frows = frame.rows - (frame.rows / 6);
            //cout << frows << endl;

            // If the frame is empty, break immediately
            if (frame.empty()) {
                printf("ugh\n");
                break;
            }


            float line[frame.cols][4];

            for (int i = 0; i < frame.cols; i++) {
                Vec3b intensity = frame.at<Vec3b>(Point(i, frows));
                if (s) {
                    blue = intensity[0];
                    green = intensity[1];
                    red = intensity[2];
                    line[i][0] = (float) red;
                    line[i][1] = (float) green;
                    line[i][2] = (float) blue;
                    line[i][4] = 0;
                    //cout << "colone : " << i << ", R : " << line[i][0] << ", G : " << line[i][1] << ", B : " << line[i][2] << endl;
                }

            }

            if (s) {
                initLine(frame, line);
                initCoord(frame, line, coord);

                s = !s;
            }

            detectMusic(frame, coord, &present[0], frows, music);

            drawLine(frame, coord, frows);
        }


        // Display the resulting frame
        resize(frame, resizedFrame, cv::Size(frame.cols/mul, frame.rows/mul));
        imshow( "Frame", resizedFrame );

        // Press  ESC on keyboard to exit
        char c=(char)waitKey(25);
        if(c==27)
            break;
        if(c==45){
            mul+=0.1;
        }
        if(c==43){
            mul-=0.1;
        }
        if(c==46){
            pause=!pause;
        }
        if(c==54){
            cap >> frame;
        }
        if(c==57){
            for(int i = 0; i < 5; i++)
                cap >> frame;
        }



    }
    // When everything done, release the video capture object
    cap.release();
    // Closes all the frames
    destroyAllWindows();

    return 0;
}