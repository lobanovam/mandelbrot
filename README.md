# mandelbrot
# Mandelbrot set

## Introduction

This projects is part of I.R.Dedinskiy programming course (1st year MIPT DREC). \
The goal is to draw a mandelbrot set and optimize it using SIMD instructions.

For visualization we will use SFML library.

## About Mandelbrot set
The Mandelbrot set is the set of complex numbers {\displaystyle c}c for which the function {\displaystyle f_{c}(z)=z^{2}+c}{\displaystyle f_{c}(z)=z^{2}+c} does not diverge to infinity when iterated from {\displaystyle z=0}z=0, i.e., for which the sequence {\displaystyle f_{c}(0)}{\displaystyle f_{c}(0)}, {\displaystyle f_{c}(f_{c}(0))}{\displaystyle f_{c}(f_{c}(0))}, etc., remains bounded in absolute value.





For more detailed information please visit the link below: \
https://en.wikipedia.org/wiki/Mandelbrot_set

## Basic implementation

Let's just use the given Alpha Blending formula for each couple of pixels independently.

~~~C++

~~~

To reduce the impact of SFML library, we will calculate the resulting value of each couple of pixels for 1000 times.

## Optimisation ideas

Our calculations for each pixel are absolutely the same and completely independent. So why don't we calculate the resultaing colors for several pixels simultaneously?
Here the SIMD instructions come to the rescue. If you are not familiar with SIMD, check the following link: \
https://en.wikipedia.org/wiki/Single_instruction,_multiple_data \
For complete list of SIMD instructions check \
https://www.laruence.com/sse/# 

Let's load 8 pixels in a 256-bit vector and calculate all 8 resulting colors in 1 iteration.
~~~C++
~~~
Now using some AVX2 commands let's calculate the resulting colors for all 8 pixels.

~~~C++
_mm_storeu_si128((__m128i *)(backPixelArr + (x + x_offs) + (y + y_offs) * backWidth), result);
~~~

## Results

FPS (frames per second) for each tested configuration. 

**reminder**: 1000 iterations for each calculation


| flags    | NO AVX, FPS | AVX, FPS |
|----------|-------------|----------|
| no flags | 4.6         | 3.0      |
| -O1      | 12.7        | 74.7     |
| -O2      | 14.8        | 78.8     |
| -O3      | 14.7        | 79.4     |
| -Ofast   | 14.7        | 79.4     |

Speed growth factor k = $\frac{FPS_{AVX}}{FPS_{NO-AVX}}$


| AVX\NOAVX | no flags  | -O1  | -O2  | -O3  | -Ofast |
|-----------|-----------|------|------|------|--------|
| no flags  | 0.65      | 0.24 | 0.20 | 0.20 | 0.20   |
| -O1       | 16.24     | 5.88 | 5.05 | 5.08 | 5.08   |
| -O2       | 17.13     | 6.2  | 5.32 | 5.36 | 5.36   |
| -O3       | 17.26     | 6.25 | 5.37 | 5.40 | 5.40   |
| -Ofast    | 17.26     | 6.25 | 5.37 | 5.40 | 5.40   |


As we can see, the best speed growth factor with the same flags was achieved with "-O1" flag (5.88).

The best FPS was achieved with AVX and "-O3"  /  "-Ofast" flags. Speed growth factor here is 5.40.
