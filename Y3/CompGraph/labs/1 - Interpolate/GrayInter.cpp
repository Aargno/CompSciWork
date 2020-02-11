#include <ModelTriangle.h>
#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <glm/glm.hpp>
#include <fstream>
#include <vector>
#include <iostream>

using namespace std;
using namespace glm;

#define WIDTH 320
#define HEIGHT 240

float getStep(float begin, float end, int numStep);
vector<float> interpolate(float begin, float end, int numEl);

void draw();
void update();
void handleEvent(SDL_Event event);

DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);

int main (int argc, char* argv[]) {
    SDL_Event event;
    while(true) {
        // We MUST poll for events - otherwise the window will freeze !
        if(window.pollForInputEvents(&event)) handleEvent(event);
        update();
        draw();
        // Need to render the frame at the end, or nothing actually gets shown on the screen !
        window.renderFrame();
    }
}

float getStep(float begin, float end, int numStep) {
    return (end - begin)/(numStep-1);
}

vector<float> interpolate(float begin, float end, int numEl) {
    vector<float> out;
    float step = getStep(begin, end, numEl);
    for (int i = 0; i < numEl; i++) {
        out.push_back(begin + (step*i));
    }
    return out;
}

void draw()
{
    window.clearPixels();
    vector<float> range = interpolate(255, 0, window.width);
    for (int y=0; y<window.height; y++) {
        for (int x=0; x<window.width; x++) {
            float red = range[x];
            float green = red;
            float blue = red;
            uint32_t colour = (255<<24) + (int(red)<<16) + (int(green)<<8) + int(blue);
            window.setPixelColour(x, y, colour);
        }
    }
//   window.clearPixels();
//   for(int y=0; y<window.height ;y++) {
//     for(int x=0; x<window.width ;x++) {
//       float red = rand() % 255;
//       float green = 0.0;
//       float blue = 0.0;
//       uint32_t colour = (255<<24) + (int(red)<<16) + (int(green)<<8) + int(blue);
//       window.setPixelColour(x, y, colour);
//     }
//   }
}

void update()
{
  // Function for performing animation (shifting artifacts or moving the camera)
}

void handleEvent(SDL_Event event)
{
  if(event.type == SDL_KEYDOWN) {
    if(event.key.keysym.sym == SDLK_LEFT) cout << "LEFT" << endl;
    else if(event.key.keysym.sym == SDLK_RIGHT) cout << "RIGHT" << endl;
    else if(event.key.keysym.sym == SDLK_UP) cout << "UP" << endl;
    else if(event.key.keysym.sym == SDLK_DOWN) cout << "DOWN" << endl;
  }
  else if(event.type == SDL_MOUSEBUTTONDOWN) cout << "MOUSE CLICKED" << endl;
}