#include <math.h>
#include "picosystem.hpp"
#include "hardware/flash.h"
#include "hardware/sync.h"

using namespace picosystem;

//saving stuff
#define ADDRESS 0x10020558
#define ERASE_SIZE 4096
#define WRITE_SIZE 4
uint8_t *readSave = (uint8_t *) (ADDRESS);
unsigned char saveBuffer[WRITE_SIZE];

// game stuff
int32_t hiscore = 999;
bool titlescreen = true;
float pi = 3.14159265;
int32_t theta = 90;
float ballX = 0;
float ballY = 0;
float arrowRadius = 15;
int32_t ballRadius = 2;
float power = 0;
float cpower = 0;
bool isAiming = true;
bool isSettingPower = false;
bool justHit = false;
int32_t justHitTimer = 0;
int32_t justHitTime = 2;
struct Edge {
    int32_t x;
    int32_t y;
    int32_t l;
    bool isVert;
};
float holeX = 0;
float holeY = 0;
int32_t holeRadius = 3;
std::vector<Edge> edges;
int32_t hole = 0;
int32_t strokes[] = {0,0,0,0,0,0,0,0,0};
bool hasSetUpHole = false;

void writeScores() {
   uint32_t ints = save_and_disable_interrupts();
   flash_range_erase(ADDRESS - XIP_BASE, ERASE_SIZE);
   flash_range_program(ADDRESS - XIP_BASE, saveBuffer, WRITE_SIZE);
   restore_interrupts(ints);
}

void saveMyData() {
    saveBuffer[0] = (hiscore >> 24) & 0xFF;
    saveBuffer[1] = (hiscore >> 16) & 0xFF;
    saveBuffer[2] = (hiscore >> 8) & 0xFF;
    saveBuffer[3] = hiscore & 0xFF;
    writeScores();
}

void readBackMyData() {
    hiscore = (readSave[0] << 24 | readSave[1] << 16 | readSave[2] << 8 | readSave[3]);
}

void playHitSound() {
    voice_t v = voice(20,0,0,0,0,0,0,0,0);
    play(v, 480, 20, 100);
}

void init() {
    readBackMyData();
    if(hiscore < 0){
        hiscore = 999;     
        saveMyData();
    }
}

void setUpHole() {
    switch(hole) {
        case 0:
            ballX = 30;
            ballY = 60;
            holeX = 90;
            holeY = 60;
            strokes[hole] = 0;
            edges.push_back({10,40,100,false});
            edges.push_back({10,80,100,false});
            edges.push_back({10,40,40,true});
            edges.push_back({110,40,41,true});
            break;
        case 1:
            ballX = 95;
            ballY = 80;
            holeX = 25;
            holeY = 45;
            strokes[hole] = 0;
            edges.push_back({40,60,70,false});
            edges.push_back({10,100,100,false});
            edges.push_back({10,30,30,false});
            
            edges.push_back({110,60,41,true});
            edges.push_back({10,30,70,true});
            edges.push_back({40,30,30,true});
            break;
        case 2:
            ballX = 100;
            ballY = 60;
            holeX = 20;
            holeY = 60;
            strokes[hole] = 0;
            edges.push_back({10,40,30,false});
            edges.push_back({50,40,60,false});

            edges.push_back({10,80,60,false});
            edges.push_back({80,80,30,false});

            edges.push_back({10,40,40,true});
            edges.push_back({110,40,41,true});

            edges.push_back({40,40,26,true});
            edges.push_back({50,40,26,true});
            edges.push_back({40,65,10,false});

            edges.push_back({70,55,26,true});
            edges.push_back({80,55,26,true});
            edges.push_back({70,55,10,false});
            break;
        case 3:
            ballX = 30;
            ballY = 30;
            holeX = 90;
            holeY = 90;
            strokes[hole] = 0;
            edges.push_back({10,20,40,false});
            edges.push_back({10,60,60,false});
            edges.push_back({10,20,40,true});
            edges.push_back({50,20,20,true});

            edges.push_back({50,40,60,false});
            edges.push_back({70,100,40,false});
            edges.push_back({70,60,40,true});
            edges.push_back({110,40,61,true});
            break;
        case 4:
            ballX = 30;
            ballY = 100;
            holeX = 100;
            holeY = 60;
            strokes[hole] = 0;
            edges.push_back({10,110,40,false});
            edges.push_back({10,70,20,false});
            edges.push_back({10,70,41,true});
            edges.push_back({50,90,21,true});

            edges.push_back({50,90,20,false});
            edges.push_back({30,50,20,false});
            edges.push_back({30,50,21,true});
            edges.push_back({70,70,21,true});

            edges.push_back({70,70,40,false});
            edges.push_back({50,30,60,false});
            edges.push_back({50,30,21,true});
            edges.push_back({110,30,41,true});
            break;
        case 5:
            ballX = 20;
            ballY = 40;
            holeX = 40;
            holeY = 100;
            strokes[hole] = 0;
            edges.push_back({10,50,60,false});
            edges.push_back({10,30,20,true});

            edges.push_back({10,30,100,false});
            edges.push_back({30,110,80,false});
            edges.push_back({70,50,41,true});
            edges.push_back({110,30,81,true});

            edges.push_back({30,90,20,true});
            edges.push_back({30,90,40,false});
            break;
        case 6:
            ballX = 20;
            ballY = 100;
            holeX = 90;
            holeY = 90;
            strokes[hole] = 0;
            edges.push_back({10,110,40,false});
            edges.push_back({10,20,100,false});
            edges.push_back({50,35,20,false});
            edges.push_back({70,110,40,false});
            edges.push_back({10,20,90,true});
            edges.push_back({50,35,76,true});
            edges.push_back({70,35,75,true});
            edges.push_back({110,20,91,true});
            break;
        case 7:
            ballX = 30;
            ballY = 90;
            holeX = 90;
            holeY = 40;
            strokes[hole] = 0;
            edges.push_back({10,110,40,false});
            edges.push_back({10,70,60,false});
            edges.push_back({50,80,20,false});
            edges.push_back({10,70,40,true});
            edges.push_back({50,80,31,true});

            edges.push_back({70,110,40,false});
            edges.push_back({80,70,30,false});
            edges.push_back({70,80,30,true});
            edges.push_back({110,70,41,true});

            edges.push_back({80,60,30,false});
            edges.push_back({70,20,40,false});
            edges.push_back({70,20,51,true});
            edges.push_back({110,20,41,true});
            edges.push_back({80,60,10,true});
            break;
        case 8:
            ballX = 100;
            ballY = 100;
            holeX = 60;
            holeY = 80;
            strokes[hole] = 0;
            edges.push_back({10,110,60,false});
            edges.push_back({10,20,100,false});
            edges.push_back({30,40,60,false});
            edges.push_back({90,110,20,false});
            edges.push_back({30,90,20,false});
            edges.push_back({50,70,20,false});
            edges.push_back({10,20,90,true});
            edges.push_back({30,40,51,true});
            edges.push_back({90,40,70,true});
            edges.push_back({110,20,91,true});
            edges.push_back({70,70,41,true});
            edges.push_back({50,70,21,true});
            break;
    }
}

void checkHole() {
    if (abs(ballX - holeX) < 3 && (abs(ballY - holeY) < 3) && cpower < 60) {
        cpower = 0;
        power = 0;
        ballX = 0;
        ballY = 0;
        hole += 1;
        hasSetUpHole = false;
        edges.clear();
        edges.shrink_to_fit();

        if (hole == 9) { //end game
            titlescreen = true;
            hole = 0;
            int32_t total = 0;
            for (int32_t i = 0; i < 9; i++) {
                total += strokes[i];
                strokes[i] = 0;
            }
            if (hiscore > total) {
                hiscore = total;       
                saveMyData();
                readBackMyData();
            }
        }

        playHitSound();
    }
}

void checkEdges() {
    for (auto &edge : edges) {
        if (edge.isVert) {
            if (theta > 90 && theta < 270) { // left
                if ((abs(ballX - edge.x) < 3) && (ballY > edge.y) && (ballY < (edge.y + edge.l))) {
                    theta = (sin((pi * (float)theta / 180.0)) * 180.0)/pi;
                    playHitSound();
                    justHit = true;
                }
            } else { // right
                if ((abs(ballX - edge.x) < 3) && (ballY > edge.y) && (ballY < (edge.y + edge.l))) {
                    theta = 180 - (sin((pi * (float)theta / 180.0)) * 180.0)/pi;
                    playHitSound();
                    justHit = true;
                }
            }
        } else {
            if (theta > 0 && theta < 180) { // top
                if ((abs(ballY - edge.y) < 3) && (ballX > edge.x) && (ballX < (edge.x + edge.l))) {
                    theta = 270 + (sin((pi * (float)(theta-270) / 180.0)) * 180.0)/pi;
                    playHitSound();
                    justHit = true;
                }
            } else { // bottom
                if ((abs(ballY - edge.y) < 3) && (ballX > edge.x) && (ballX < (edge.x + edge.l))) {
                    theta = 90 + (sin((pi * (float)(theta-90) / 180.0)) * 180.0)/pi;
                    playHitSound();
                    justHit = true;
                }
            }
        }
    }
}

void update(uint32_t tick) {
     if (titlescreen) {
        if (pressed(A)) {
            titlescreen = false;
        }
        return;
    }

    if (!hasSetUpHole) {
        setUpHole();
        hasSetUpHole = true;
    }

    if (isAiming) {
        if (button(LEFT)) {
            if (theta++ > 360) theta = 0;
        }
        if (button(RIGHT)) {
            if (theta-- < 0) theta = 360;
        }
        if (pressed(A)) {
            isAiming = false;
            isSettingPower = true;
        }
    } else if (isSettingPower) {
        if (power < 5) {
            power++;
        } else if (power < 60) {
            power+=2;
        } else if (power < 120) {
            power+=4;
        } else {
            power+=6;
        }

        if (power > 240) power = 0;
        frect(0,112,power/2,8);
        if (pressed(A)) {
            cpower = power;
            isSettingPower = false;
            strokes[hole] += 1;
            playHitSound();
        }
    }else if (!isAiming && !isSettingPower) {
        float speed = cpower > 20 ? 2 : 1.1;
        cpower -= 1;
        // if (justHit && cpower > 20) {
        //     // speed = 4;
        // }
        if(justHitTimer++ >= justHitTime){
            justHit = false;
            justHitTimer = 0;
        }
        float bx = ballX + speed * cos((pi * (float)theta / (float)180.0));
        float by = ballY - speed * sin((pi * (float)theta / (float)180.0));
        ballX = bx;
        ballY = by;

        if (cpower <= 0) {
            isAiming = true;
            power = 0;
            if (ballY < 60) {
                theta = 270;
            } else {
                theta = 90;
            }
        }
    }

    if(!justHit) checkEdges();
    checkHole();

    // // top wall
    // if (ballY <= 0) {
    //     theta = 270 + (sin((pi * (float)(theta-270) / 180.0)) * 180.0)/pi;
    // }

    // // bottom wall
    // if (ballY >= 120) {
    //     theta = 90 + (sin((pi * (float)(theta-90) / 180.0)) * 180.0)/pi;
    // }

    // // left wall
    // if (ballX <= 0) {
    //     theta = (sin((pi * (float)theta / 180.0)) * 180.0)/pi;
    // }

    // // right wall
    // if (ballX >= 120) {
    //     theta = 180 - (sin((pi * (float)theta / 180.0)) * 180.0)/pi;
    // }
}

void drawEdges() {
    for (auto &edge : edges) {
        if (edge.isVert) vline(edge.x,edge.y,edge.l);
        if (!edge.isVert) hline(edge.x,edge.y,edge.l);
    }
}

void draw(uint32_t tick) {
    pen(0,0,0);
    clear();

    if (titlescreen) {
        pen(15,15,15);
        text("Wyatt's Mini Golf", 20, 30);

        if (hiscore != 999) {
            text("Lowest Score: ", 20, 60);
            text(str(hiscore), 20, 70);
        }

        if (time() % 1000 < 500) {
            text("Press A", 42, 90);   
        }
        return;
    }

    pen(12,12,12);
    fcircle(holeX,holeY,holeRadius);

    pen(15,15,15);
    fcircle(ballX,ballY,ballRadius);

    if (isAiming) {
        int32_t px = ballX + arrowRadius * cos((pi * (float)theta / (float)180.0));
        int32_t py = ballY - arrowRadius * sin((pi * (float)theta / (float)180.0));
        line(ballX,ballY,px,py);
    }

    drawEdges();

    //ui
    for (int32_t i = 0; i < 9; i++) {
        text(strokes[i] == 0 && i != hole ? "-" : str(strokes[i]),(i*13) + 4,0);
    }
    int32_t total = 0;
    for (int32_t i = 0; i < 9; i++) {
        total += strokes[i];
    }
    text("score:",0,8);
    text(str(total),35,8);
}