#include "mandelbr.hpp"

const int W_HEIGHT = 1000;
const int W_WIDTH = 1000;
const int MAX_ITER = 255;
const float MAX_DISTANCE = 100.0; // squared  10.0
typedef unsigned char BYTE;

const float dx = 0.004;   // 4/1000
const float dy = 0.004;   // 4/1000

int main() {
    sf::RenderWindow window(sf::VideoMode(W_HEIGHT, W_WIDTH), "My window");
    window.setFramerateLimit(30);

    while (window.isOpen())
    {
        //printf("is open\n");
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        //window.clear(sf::Color::Black);

        DrawMandlbr(window);

        window.display();
    }

    return 0;
    
}


void DrawMandlbr(sf::RenderWindow &window) {
    sf::RectangleShape Pixel = CreatePixel();

    for (float y0 = -2; y0 <= 2; y0 += dy) {
        float y0_pos = (2.0 - y0) * 250;         // 250 = 1/dy
       // printf("y0 is %f\n", y0);
        
        for (float x0 = -2.0; x0 <= 2.0; x0 += dx) {
            float x0_pos = (2.0 + x0) * 250;
            int cur_iter = 0;

            float x = x0, y = y0;

            for ( ; cur_iter < MAX_ITER; cur_iter++) {
                float x2 = x * x, y2 = y * y, xy = x * y;
                float distance = x2 + y2;
                if (distance > MAX_DISTANCE)
                    break;

                x = x2 - y2 + x0;
                y = xy + xy + y0;
            }
            
            //printf("x0_pos is %f, y0_pos is %f\n", x0_pos, y0_pos);   
            Pixel.setPosition(x0_pos, y0_pos);
            Pixel.setFillColor(sf::Color{(BYTE) cur_iter, 0, (BYTE) 255 - cur_iter});
            window.draw(Pixel);
        }
    }

}

sf::RectangleShape CreatePixel() {
    sf::RectangleShape pixel(sf::Vector2f(150, 20));
    pixel.setSize(sf::Vector2f(1, 1));
    pixel.setPosition(0, 0);
    pixel.setFillColor(sf::Color::Green);
    //printf("pixel set\n");
    return pixel;
}
