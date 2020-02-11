#include <vector>
#include <glm/glm.hpp>
#include <iostream>

using namespace glm;
using namespace std;

void print(vector<float> const &input)
{
	for (int i = 0; i < input.size(); i++) {
		cout << input.at(i) << ' ';
	}
}

void print(vector<vec3> const &input)
{
	for (int i = 0; i < input.size(); i++) {
		for (int j = 0; j < 3; j++) cout << input[i][j] << ' ';
        cout << endl;
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

int main (int argc, char* argv[]) {
    vec3 from(1,4,9.2);
    vec3 to(4,1,9.8);
    // cout << to[0] << " " << to[1] << " " << to[2] << endl;
    print(interpolate(2.2,8.5,7));
    cout << endl;
    print(vecInterpolate(from, to, 4));
}