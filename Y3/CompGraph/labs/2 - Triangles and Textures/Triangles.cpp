#include <ModelTriangle.h>
#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <glm/glm.hpp>
#include <fstream>
#include <vector>
#include <string.h>
#include <iostream>

using namespace std;
using namespace glm;

// #define WIDTH 320
// #define HEIGHT 240

// #define WIDTH 1696
// #define HEIGHT 1131

#define WIDTH 480
#define HEIGHT 395

void draw();
void update();
void handleEvent(SDL_Event event);
uint32_t packColour(Colour col);
float getStep(float begin, float end, float numStep);
vector<CanvasPoint> interpolate(CanvasPoint begin, CanvasPoint end, float numEl);
vector<float> interpolate(float begin, float end, float numEl);
void line(CanvasPoint begin, CanvasPoint end, Colour colour);
CanvasTriangle sortedTri(CanvasTriangle tri);
CanvasPoint getFlat(CanvasTriangle tri);
void triangle(CanvasTriangle triang, Colour col);
void rasterize(CanvasTriangle tri, Colour col);
// vector<uint32_t> loadTexture(CanvasPoint begin, CanvasPoint end, float stepCount);
vector<uint32_t> loadTexture(CanvasPoint begin, CanvasPoint end, float stepCount, vector<uint32_t> texture);

DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);

int main(int argc, char* argv[])
{
  SDL_Event event;
  while(true)
  {
    // We MUST poll for events - otherwise the window will freeze !
    if(window.pollForInputEvents(&event)) handleEvent(event);
    update();
    draw();
    // Need to render the frame at the end, or nothing actually gets shown on the screen !
    window.renderFrame();
  }
}

void draw()
{
  // window.clearPixels();

  // for(int y=0; y<window.height ;y++) {
  //   for(int x=0; x<window.width ;x++) {
  //     float red = rand() % 255;
  //     float green = 0.0;
  //     float blue = 0.0;
  //     uint32_t colour = (255<<24) + (int(red)<<16) + (int(green)<<8) + int(blue);
  //     window.setPixelColour(x, y, colour);
  //   }
  // }

  // CanvasPoint begin(30,45);
  // CanvasPoint end(window.width-1,window.height-1);
  // CanvasPoint end(27,200);
  // CanvasPoint begin(window.width-1,0);
  // CanvasPoint end(0,window.height-1);
  // CanvasPoint begin(11,20);
  // CanvasPoint end(0,20);
  // Colour col(255,255,255);
  // line(begin, end, col);
}

uint32_t packColour(Colour col) {
  return (255<<24) + (col.red<<16) + (col.green<<8) + col.blue;
}

Colour unPackColour(uint32_t col) {
  int red = (col>>16)&0xFF;
  int green = (col>>8)&0xFF;
  int blue = (col)&0xFF;
  return Colour(red, green, blue);
}

float getStep(float begin, float end, float numStep) {
  return (end - begin)/(numStep);
}

// float maxStepCount(CanvasPoint begin, CanvasPoint end) {

// }

vector<float> interpolate(float begin, float end, float numEl) {
  vector<float> out;
  if (numEl <= 0) return out;
  float step = getStep(begin, end, numEl);
  for (int i = 0; i < numEl; i++) {
      out.push_back(begin + (step*i));
  }
  return out;
}

vector<CanvasPoint> interpolate(CanvasPoint begin, CanvasPoint end, float numEl) {
  vector<CanvasPoint> out;
  if (numEl <= 0) return out;
  float stepX = getStep(begin.x, end.x, numEl);
  float stepY = getStep(begin.y, end.y, numEl);
  CanvasPoint step(0,0);
  for (float i = 0.0; i < numEl; i++) {
    step.x = begin.x + (stepX*i);
    step.y = begin.y + (stepY*i);
    out.push_back(step);
  }
  return out;
}

void line(CanvasPoint begin, CanvasPoint end, Colour colour) {
  float diffX = end.x-begin.x;
  float diffY = end.y-begin.y;
  // float steps = (std::abs(diffX) > std::abs(diffY)) ? diffX : diffY;
  float steps = std::max(abs(diffX), abs(diffY));
  // vector<CanvasPoint> linePoints = interpolate(begin, end, steps);
  if (steps <= 0) return;
  float stepX = getStep(begin.x, end.x, steps);
  float stepY = getStep(begin.y, end.y, steps);
  //Rounding leaves gap in centre of line
  float x, y;
  for (float i = 0.0; i < steps; i++) {
    x = begin.x + (i*stepX);
    y = begin.y + (i*stepY);
    window.setPixelColour(std::round(x), std::round(y), packColour(colour));
    // window.setPixelColour(std::round(linePoints[i].x), std::round(linePoints[i].y), packColour(colour));
  }
}

//Sorts vertices into descending vertical order e.g: {top, middle, bottom}
CanvasTriangle sortedTri(CanvasTriangle tri) {
  if (tri.vertices[0].y > tri.vertices[1].y) { //[0] is guaranteed smaller than [1]
    std::swap(tri.vertices[0], tri.vertices[1]);
  }
  if (tri.vertices[1].y > tri.vertices[2].y) { //[2] is guaranteed largest val
    std::swap(tri.vertices[1], tri.vertices[2]);
  }
  if (tri.vertices[0].y > tri.vertices[1].y) { //[0] is smallest, [2] is largest, [1] is mid
    std::swap(tri.vertices[0], tri.vertices[1]);
  }
  return tri;
}

CanvasPoint getFlat(CanvasTriangle tri) {
  //Could probably actually solve triangle for this
  float diffX = tri.vertices[2].x-tri.vertices[0].x;
  float diffY = tri.vertices[2].y-tri.vertices[0].y;
  float steps = std::max(abs(diffX), abs(diffY));
  if (steps <= 0) return tri.vertices[1];
  float stepX = getStep(tri.vertices[0].x, tri.vertices[2].x, steps);
  float stepY = getStep(tri.vertices[0].y, tri.vertices[2].y, steps);
  float i = (tri.vertices[1].y - tri.vertices[0].y)/stepY;
  return CanvasPoint(std::round(tri.vertices[0].x + (stepX*i)), tri.vertices[1].y);
}

void triangle(CanvasTriangle tri, Colour col) {
  line(tri.vertices[0], tri.vertices[1], col);
  line(tri.vertices[1], tri.vertices[2], col);
  line(tri.vertices[2], tri.vertices[0], col);
}

//Problem is here
void fillTri(CanvasTriangle tri, Colour col) {

  //Probably here
  float diffX = tri.vertices[0].x-tri.vertices[1].x;
  float diffY = tri.vertices[0].y-tri.vertices[1].y;
  float steps1 = std::max(abs(diffX), abs(diffY));
  diffX = tri.vertices[0].x-tri.vertices[2].x;
  diffY = tri.vertices[0].y-tri.vertices[2].y;
  float steps2 = std::max(abs(diffX), abs(diffY));
  float steps = std::max(steps1, steps2);

  vector<CanvasPoint> side1 = interpolate(tri.vertices[0], tri.vertices[1], steps);
  vector<CanvasPoint> side2 = interpolate(tri.vertices[0], tri.vertices[2], steps);
  cout << side1.size() << " " << side2.size() << endl;
  for (float i = 0.0; i < steps; i++) {
    line(side1[i], side2[i], col);
  }
}

void rasterize(CanvasTriangle tri, Colour col) {
  tri = sortedTri(tri);
  CanvasPoint flat = getFlat(tri);
  CanvasTriangle tri1 = tri;
  CanvasTriangle tri2 = tri;
  tri1.vertices[2] = flat;
  tri2.vertices[0] = flat;
  fillTri(tri1, col);
  fillTri(tri2, col);
}

// vector<uint32_t> loadTexture(CanvasPoint begin, CanvasPoint end, float stepCount) {
//   vector<uint32_t> tex;
//   vector<CanvasPoint> locs = interpolate(begin, end, stepCount);
//   for (float i = 0.0; i < stepCount; i++) {
//     tex.push_back(window.getPixelColour(std::round(locs[i].x), std::round(locs[i].y)));
//   }
//   return tex;
// }

///////
//Load PPM

string removeComments(string line) {
  return line.substr(0, line.find("#"));
}

//Gotta be a built in function for this
string nextNonComment(ifstream &file) {
  const char* t = " \t\n\r\f\v";
  string line;
  file >> line; //Read until whitespace
  line.erase(0, line.find_first_not_of(t)); //Remove leading whitespace
  while(line.at(0) == '#') {
    getline(file, line); //If start is comment, throw away rest of line
    file >> line;
    line.erase(0, line.find_first_not_of(t));
  }
  return removeComments(line);
}

vector<uint32_t> loadPPM(string fileName) {
  vector<uint32_t> out;
  ifstream file;
  file.open(fileName, ios::in | ios::binary | ios::ate);
  file.seekg(0, ios::beg);
  string line;
  line = nextNonComment(file); //Format
  line = nextNonComment(file); //Width
  int width = stoi(line);
  line = nextNonComment(file); //Height
  int height = stoi(line);
  line = nextNonComment(file); //Max Colour
  int length = (width*height)*3;
  char *c = new char[length]; //Read into buffer all at once instead
  file.read(&c[0], length);
  int j = 0;
  int y = 0;
  for (int i = 0; i < length; i+=3) {
    int blue = (unsigned char) c[i];
    int red = (unsigned char) c[i+1];
    int green = (unsigned char) c[i+2];
    Colour col(red, green, blue);
    out.push_back(packColour(col));
    window.setPixelColour(j%width, y, packColour(col));
    j++;
    if ((j%width) == 0) y++;
  }
  return out;
}

///////

///////
//Texture Tri

void line(CanvasPoint begin, CanvasPoint end, vector<uint32_t> texture, float step) {
  if (step <= 0) return;
  float stepX = getStep(begin.x, end.x, step);
  float stepY = getStep(begin.y, end.y, step);
  float x, y;
  for (float i = 0.0; i < step; i++) {
    x = begin.x + (i*stepX);
    y = begin.y + (i*stepY);
    window.setPixelColour(std::round(x), std::round(y), texture[i]);
    // window.setPixelColour(std::round(linePoints[i].x), std::round(linePoints[i].y), packColour(colour));
  }
}

CanvasTriangle getTextureTri(CanvasTriangle tri) {
  CanvasPoint v1(tri.vertices[0].texturePoint.x, tri.vertices[0].texturePoint.y);
  CanvasPoint v2(tri.vertices[1].texturePoint.x, tri.vertices[1].texturePoint.y);
  CanvasPoint v3(tri.vertices[2].texturePoint.x, tri.vertices[2].texturePoint.y);
  return CanvasTriangle(v1, v2, v3);
}

//Sorts vertices into descending vertical order e.g: {top, middle, bottom}
CanvasTriangle sortedTri(CanvasTriangle tri, CanvasTriangle &tex) {
  if (tri.vertices[0].y > tri.vertices[1].y) { //[0] is guaranteed smaller than [1]
    std::swap(tri.vertices[0], tri.vertices[1]);
    std::swap(tex.vertices[0], tex.vertices[1]);
  }
  if (tri.vertices[1].y > tri.vertices[2].y) { //[2] is guaranteed largest val
    std::swap(tri.vertices[1], tri.vertices[2]);
    std::swap(tex.vertices[1], tex.vertices[2]);
  }
  if (tri.vertices[0].y > tri.vertices[1].y) { //[0] is smallest, [2] is largest, [1] is mid
    std::swap(tri.vertices[0], tri.vertices[1]);
    std::swap(tex.vertices[0], tex.vertices[1]);
  }
  return tri;
}

// void fillTri(CanvasTriangle tri, CanvasTriangle tex, vector<uint32_t> texture) {

//   float diffX = tri.vertices[0].x-tri.vertices[1].x;
//   float diffY = tri.vertices[0].y-tri.vertices[1].y;
//   float steps1 = std::max(abs(diffX), abs(diffY));
//   diffX = tri.vertices[0].x-tri.vertices[2].x;
//   diffY = tri.vertices[0].y-tri.vertices[2].y;
//   float steps2 = std::max(abs(diffX), abs(diffY));
//   float steps = std::max(steps1, steps2);

//   // diffX = tex.vertices[0].x-tex.vertices[1].x;
//   // diffY = tex.vertices[0].y-tex.vertices[1].y;
//   // steps1 = std::max(abs(diffX), abs(diffY));
//   // diffX = tex.vertices[0].x-tex.vertices[2].x;
//   // diffY = tex.vertices[0].y-tex.vertices[2].y;
//   // steps2 = std::max(abs(diffX), abs(diffY));
//   // float stepsT = std::max(steps1, steps2);

//   vector<CanvasPoint> side1 = interpolate(tex.vertices[0], tex.vertices[1], steps);
//   vector<CanvasPoint> side2 = interpolate(tex.vertices[0], tex.vertices[2], steps);
//   vector<CanvasPoint> tri1 = interpolate(tri.vertices[0], tri.vertices[1], steps);
//   vector<CanvasPoint> tri2 = interpolate(tri.vertices[0], tri.vertices[2], steps);
//   vector<vector<uint32_t>>lineTex;
//   for (int i = 0; i < steps; i++) {
//     diffX = tri1[i].x-tri2[i].x;
//     diffY = tri1[i].y-tri2[i].y;
//     steps1 = std::max(abs(diffX), abs(diffY));
//     lineTex.push_back(loadTexture(side1[i], side2[i], steps1, texture));
//   }
//   for (int i = 0; i < steps; i++) {
//     diffX = tri1[i].x-tri2[i].x;
//     diffY = tri1[i].y-tri2[i].y;
//     steps1 = std::max(abs(diffX), abs(diffY));
//     line(tri1[i], tri2[i], lineTex[i], steps1);
//   }
// }

void fillTri(CanvasTriangle tri, CanvasTriangle tex, vector<uint32_t> texture) {

  float diffX = tri.vertices[0].x-tri.vertices[2].x;
  float diffY = tri.vertices[0].y-tri.vertices[2].y;
  float steps = std::max(abs(diffX), abs(diffY));

  vector<CanvasPoint> side2 = interpolate(tex.vertices[0], tex.vertices[1], steps);
  vector<CanvasPoint> side1 = interpolate(tex.vertices[0], tex.vertices[2], steps);
  vector<CanvasPoint> tri2 = interpolate(tri.vertices[0], tri.vertices[1], steps);
  vector<CanvasPoint> tri1 = interpolate(tri.vertices[0], tri.vertices[2], steps);
  vector<vector<uint32_t>>lineTex;
  for (int i = 0; i < steps; i++) {
    diffX = tri1[i].x-tri2[i].x;
    diffY = tri1[i].y-tri2[i].y;
    float steps1 = std::max(abs(diffX), abs(diffY));
    lineTex.push_back(loadTexture(side1[i], side2[i], steps1, texture));
  }
  for (int i = 0; i < steps; i++) {
    diffX = tri1[i].x-tri2[i].x;
    diffY = tri1[i].y-tri2[i].y;
    float steps1 = std::max(abs(diffX), abs(diffY));
    line(tri1[i], tri2[i], lineTex[i], steps1);
  }
}

vector<uint32_t> loadTexture(CanvasPoint begin, CanvasPoint end, float stepCount, vector<uint32_t> texture) {
  vector<uint32_t> tex;
  vector<CanvasPoint> locs = interpolate(begin, end, stepCount);
  for (int i = 0; i < stepCount; i++) {
    // tex.push_back(window.getPixelColour(std::round(locs[i].x), std::round(locs[i].y)));
    tex.push_back(texture[std::round(locs[i].x) + WIDTH*std::round(locs[i].y)]);
  }
  return tex;
}

void rasterize(CanvasTriangle tri, string fileName) {
  vector<uint32_t> texture;
  texture = loadPPM(fileName);
  window.clearPixels();
  tri = sortedTri(tri);
  CanvasTriangle tex = getTextureTri(tri);
  // CanvasPoint flat = getFlat(tri);
  // CanvasPoint flatTex = getFlat(tex);
  // CanvasTriangle tri1 = tri;
  // CanvasTriangle tri2 = tri;
  // CanvasTriangle tex1 = tex;
  // CanvasTriangle tex2 = tex;
  // tri1.vertices[2] = flat;
  // tri2.vertices[0] = flat;
  // tex1.vertices[2] = flatTex;
  // tex2.vertices[0] = flatTex;
  // fillTri(tri1, tex1, texture);
  // fillTri(tri2, tex2, texture);
  fillTri(tri, tex, texture);
}

///////

void update()
{
  // Function for performing animation (shifting artifacts or moving the camera)
}

void handleEvent(SDL_Event event)
{
  if(event.type == SDL_KEYDOWN) {
    switch(event.key.keysym.sym) {
      case SDLK_LEFT : 
        cout << "LEFT" << endl;
        break;
      case SDLK_RIGHT : 
        cout << "RIGHT" << endl;
        break;
      case SDLK_UP : 
        cout << "UP" << endl;
        break;
      case SDLK_DOWN : 
        cout << "DOWN" << endl;
        break;
      case SDLK_u : { //Draw stroked triangle
        CanvasPoint v1(rand()%(window.width-1), rand()%(window.height-1));
        CanvasPoint v2(rand()%(window.width-1), rand()%(window.height-1));
        CanvasPoint v3(rand()%(window.width-1), rand()%(window.height-1));
        CanvasTriangle tri(v1,v2,v3);
        Colour col(rand()%255, rand()%255, rand()%255);
        triangle(tri, col);
        break;
      }
      case SDLK_l : { //Draw line
        CanvasPoint v1(rand()%(window.width-1), rand()%(window.height-1));
        CanvasPoint v2(rand()%(window.width-1), rand()%(window.height-1));
        Colour col(rand()%255, rand()%255, rand()%255);
        line(v1, v2, col);
        break;
      }
      case SDLK_f : {
        CanvasPoint v1(rand()%(window.width-1), rand()%(window.height-1));
        CanvasPoint v2(rand()%(window.width-1), rand()%(window.height-1));
        CanvasPoint v3(rand()%(window.width-1), rand()%(window.height-1));
        CanvasTriangle tri(v1,v2,v3);
        Colour col(rand()%255, rand()%255, rand()%255);
        rasterize(tri, col);
        Colour white(255, 255, 255);
        triangle(tri, white);
        break;
      }
      case SDLK_r : 
        loadPPM("texture.ppm");
        break;
      case SDLK_t : {
        //DRAW DIAGRAM AND FIGURE OUT STEPS
        CanvasPoint v1(160, 10);
        v1.texturePoint = TexturePoint(195, 5);
        CanvasPoint v2(300, 230);
        v2.texturePoint = TexturePoint(395, 380);
        CanvasPoint v3(10, 150);
        v3.texturePoint = TexturePoint(65, 330);
        CanvasTriangle tri(v1,v2,v3);
        // CanvasPoint v4(195, 5);
        // CanvasPoint v5(395, 380);
        // CanvasPoint v6(65, 330);
        // CanvasTriangle tex(v4,v5,v6);
        rasterize(tri, "texture.ppm");
        Colour white(255, 255, 255);
        triangle(tri, white);
        break;
      }
      case SDLK_c : //Clear window
        window.clearPixels();
        break;
    }
    // if(event.key.keysym.sym == SDLK_LEFT) cout << "LEFT" << endl;
    // else if(event.key.keysym.sym == SDLK_RIGHT) cout << "RIGHT" << endl;
    // else if(event.key.keysym.sym == SDLK_UP) cout << "UP" << endl;
    // else if(event.key.keysym.sym == SDLK_DOWN) cout << "DOWN" << endl;
    // else if(event.key.keysym.sym == SDLK_u) {
    //   CanvasPoint v1(rand()%(window.width-1), rand()%(window.height-1));
    //   CanvasPoint v2(rand()%(window.width-1), rand()%(window.height-1));
    //   CanvasPoint v3(rand()%(window.width-1), rand()%(window.height-1));
    //   CanvasTriangle tri(v1,v2,v3);
    //   Colour col(rand()%255, rand()%255, rand()%255);
    //   triangle(tri, col);
    // } else if(event.key.keysym.sym == SDLK_c) {
    //   window.clearPixels();
    // }
  }
  else if(event.type == SDL_MOUSEBUTTONDOWN) cout << "MOUSE CLICKED" << endl;
}
