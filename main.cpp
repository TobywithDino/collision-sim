#include <SDL2/SDL.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <vector>

#define SCREEN_WIDTH 1280 
#define SCREEN_HEIGHT 720
int SDL_RenderFillCircle(SDL_Renderer * renderer, int x, int y, int radius);
void checkCollision();
int lastTick = 0;

SDL_Renderer* renderer;
SDL_Window* window;

class Ball{
public:
    Ball(){
        radius = rand()%60 + 5;
        mass = radius * radius;
        speedx = rand()%13 - 6;
        speedy = rand()%13 - 6;
        x = rand()%(SCREEN_WIDTH-50) + 50;
        y = rand()%(SCREEN_HEIGHT-50) + 50;
        color = {(unsigned char)(rand()%226+30),(unsigned char)(rand()%226+30),(unsigned char)(rand()%226+30),255};
        lastColTime = 0;
        lastColBall = this;
    }
    void move(){
        x += speedx;
        y += speedy;
        if(x+radius > SCREEN_WIDTH){
            x = SCREEN_WIDTH-radius;
            speedx *= -1;
        }else if(x-radius < 0){
            x = radius;
            speedx *= -1;
        }
        if(y+radius > SCREEN_HEIGHT){
            y = SCREEN_HEIGHT-radius;
            speedy *= -1;
        }else if(y-radius < 0){
            y = radius;
            speedy *= -1;
        }
    }
    void render(){
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        if(SDL_RenderFillCircle(renderer, x, y, radius) == -1) std::cout << "not rendering\n";
    }
    void bounce(Ball* colBall){
        if(SDL_GetTicks64() - lastColTime > 200 || lastColBall != colBall){
            setLastColBall(colBall);
            setLastColTime(SDL_GetTicks64());
            colBall->setLastColBall(this);
            colBall->setLastColTime(SDL_GetTicks64());
            double sumMass = mass + colBall->getMass();
            double difMass = mass - colBall->getMass();
            double a_newSpeedx = (difMass*speedx + 2*colBall->getMass()*colBall->getSpeedx()) / sumMass;
            double a_newSpeedy = (difMass*speedy + 2*colBall->getMass()*colBall->getSpeedy()) / sumMass;
            double b_newSpeedx = (2*mass*speedx - difMass*colBall->getSpeedx()) / sumMass;
            double b_newSpeedy = (2*mass*speedy - difMass*colBall->getSpeedy()) / sumMass;
            setSpeedx(a_newSpeedx);
            setSpeedy(a_newSpeedy);
            colBall->setSpeedx(b_newSpeedx);
            colBall->setSpeedy(b_newSpeedy);
        }
    }
    double getRadius() {return radius;}
    double getMass() {return mass;}
    double getX() {return x;}
    double getY() {return y;}
    double getSpeedx() {return speedx;}
    double getSpeedy() {return speedy;}
    void setSpeedx(double newSpeedx) {speedx = newSpeedx;}
    void setSpeedy(double newSpeedy) {speedy = newSpeedy;}
    void setLastColTime(int last) {lastColTime = last;}
    void setLastColBall(Ball* colBall) {lastColBall = colBall;}
private:
    SDL_Color color;
    double radius;
    double mass;
    double speedx;
    double speedy;
    double x;
    double y;
    int lastColTime;
    Ball* lastColBall;
};
std::vector<Ball*> balls;

int main(int argc, char** argv){
    srand(time(NULL));
   
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("Error: SDL failed to initialize\nSDL Error: '%s'\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Collision-sim", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if(!window){
        printf("Error: Failed to open window\nSDL Error: '%s'\n", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer){
        printf("Error: Failed to create renderer\nSDL Error: '%s'\n", SDL_GetError());
        return 1;
    }
    for(int i=0;i<10;i++){
        Ball* ball = new Ball();
        balls.push_back(ball);
    }
    bool running = true;
    while(running){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    running = false;
                    break;

                default:
                    break;
            }
        }
        if(SDL_GetTicks64() - lastTick > 1000/60){
            lastTick = SDL_GetTicks64();
            SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
            SDL_RenderClear(renderer);
            for(Ball* ball : balls) ball->move();
            checkCollision();
            for(Ball* ball : balls) ball->render();
            SDL_RenderPresent(renderer);
        }
        
    }

    return 0;
}

void checkCollision(){
    for(Ball* b1 : balls){
        for(Ball* b2 : balls){
            if(b1 == b2) continue;
            if( (b1->getX() - b2->getX())*(b1->getX() - b2->getX())+(b1->getY() - b2->getY())*(b1->getY() - b2->getY()) <
                (b1->getRadius() + b2->getRadius())*(b1->getRadius() + b2->getRadius()))
            {
                b1->bounce(b2);
                b2->bounce(b1);
            }
        }
    }
}

int SDL_RenderFillCircle(SDL_Renderer * renderer, int x, int y, int radius)
{
    int offsetx, offsety, d;
    int status;

    offsetx = 0;
    offsety = radius;
    d = radius -1;
    status = 0;

    while (offsety >= offsetx) {

        status += SDL_RenderDrawLine(renderer, x - offsety, y + offsetx,
                                     x + offsety, y + offsetx);
        status += SDL_RenderDrawLine(renderer, x - offsetx, y + offsety,
                                     x + offsetx, y + offsety);
        status += SDL_RenderDrawLine(renderer, x - offsetx, y - offsety,
                                     x + offsetx, y - offsety);
        status += SDL_RenderDrawLine(renderer, x - offsety, y - offsetx,
                                     x + offsety, y - offsetx);

        if (status < 0) {
            status = -1;
            break;
        }

        if (d >= 2*offsetx) {
            d -= 2*offsetx + 1;
            offsetx +=1;
        }
        else if (d < 2 * (radius - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        }
        else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }

    return status;
}