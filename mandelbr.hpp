#ifndef MANDELBR_H
#define MANDELBR_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <stdio.h>
#include <math.h>
#include <immintrin.h>

void DrawMandlbr(sf::Image &image, float center_x, float center_y);
void AVXDrawMandlbr(sf::Image &image, float center_x, float center_y);
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

__m256 max_dist = _mm256_set1_ps (MAX_DISTANCE);
__m256i i_x_shifts = _mm256_set_epi32 (7, 6, 5, 4, 3, 2, 1, 0);
__m256 f_x_shifts = _mm256_set_ps (7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f, 0.f);

#endif