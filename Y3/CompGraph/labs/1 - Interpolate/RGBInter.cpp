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

vector<vec3> vecInterpolate(vec3 from, vec3 to, int numEl) {
    vec3 step;
    for (int i = 0; i < 3; i++) step[i] = getStep(from[i], to[i], numEl);
    vector<vec3> out;
    vec3 store;
    for (int i = 0; i < numEl; i++) {
        for (int j = 0; j < 3; j++) store[j] = from[j] + (i*step[j]);
        out.push_back(store);
    }
    return out;
}

void draw()
{
    window.clearPixels();
    vec3 red(255,0,0);
    vec3 green(0,255,0);
    vec3 blue(0,0,255);
    vec3 yellow(255,255,0);
    vector<vec3> left = vecInterpolate(red, yellow, window.height);
    vector<vec3> right = vecInterpolate(blue, green, window.height);
    vector<vec3> row;
    for (int y=0; y<window.height; y++) {
        row = vecInterpolate(left[y], right[y], window.width);
        for (int x=0; x<window.width; x++) {
            float red = row[x][0];
            float green = row[x][1];
            float blue = row[x][2];
            uint32_t colour = (255<<24) + (int(red)<<16) + (int(green)<<8) + int(blue);
            window.setPixelColour(x, y, colour);
        }
    }
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