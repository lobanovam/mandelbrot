#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <stdio.h>
#include <math.h>
#include <immintrin.h>

void DrawMandlbr(sf::RenderWindow &window, float center_x, float center_y);
void AVXDrawMandlbr(sf::RenderWindow &window, float center_x, float center_y);
sf::Text *SetText (sf::Font &font, float x_coord, float y_coord);

const int W_HEIGHT = 1000;
const int W_WIDTH = 1000;
const int MAX_ITER = 50;
const float MAX_DISTANCE = 100.0; // squared  10.0
typedef unsigned char BYTE;

float x_brdr = 2.0;
float y_brdr = 2.0;
float dx = 2 * x_brdr / (float)W_WIDTH;
float dy = 2 * x_brdr / (float)W_HEIGHT;