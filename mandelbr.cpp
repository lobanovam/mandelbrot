#include "mandelbr.hpp"

#define AVX 1
#define DRAW 0 

__m256 max_dist = _mm256_set1_ps (MAX_DISTANCE);

int main() {
    sf::RenderWindow window(sf::VideoMode(W_HEIGHT, W_WIDTH), "My window");
    window.setFramerateLimit(30);
    float cent_x = 0.0, cent_y = 0.0;

    sf::Clock clock; // starts the clock

    sf::Font font;
    font.loadFromFile("SEASRN__.ttf");
    sf::Text cycle_text = *GenerateTextSprite (font, "Cycle time: ", 50, 50);
    cycle_text.setCharacterSize (20);
    cycle_text.setFillColor (sf::Color::Red);

    while (window.isOpen()) {

        clock.restart();


        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            cent_x -= 0.1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            cent_x += 0.1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            cent_y += 0.1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            cent_y -= 0.1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            x_brdr -= 0.2;
            y_brdr -= 0.2;
            dx = 2 * x_brdr / (float)W_WIDTH;
            dy = 2 * y_brdr / (float)W_HEIGHT;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            x_brdr += 0.2;
            y_brdr += 0.2;
            dx = 2 * x_brdr / (float)W_WIDTH;
            dy = 2 * y_brdr / (float)W_HEIGHT;
        }
        
        
        sf::Event event;
        while (window.pollEvent(event)) {  
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
       

        if (AVX)
            AVXDrawMandlbr(window, cent_x, cent_y);
        else
            DrawMandlbr(window, cent_x, cent_y);
        
        sf::Time elapsed_time = clock.getElapsedTime();

        char text_buffer[100];
        sprintf (text_buffer, "FPS: %f\n", 1/elapsed_time.asSeconds());
        printf (text_buffer);

        cycle_text.setString (text_buffer);
        window.draw (cycle_text);

        window.display();

        window.clear(sf::Color::Black);
    }

    return 0;
    
}

void DrawMandlbr(sf::RenderWindow &window, float center_x, float center_y) {
    sf::RectangleShape Pixel = sf::RectangleShape(sf::Vector2f(1, 1));
    Pixel.setFillColor(sf::Color::Black);
    
    float y_min = -y_brdr - center_y, y_max = y_brdr - center_y;
    float x_min = -x_brdr - center_x, x_max = x_brdr - center_x;
    for (float y0 = y_min; y0 <= y_max; y0 += dy) {
    
        float y0_pos = (y_max - y0) * 1/dy;         
        
        for (float x0 = x_min; x0 <= x_max; x0 += dx) {
            float x0_pos = (x0 - x_min) * 1/dx;

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

            Pixel.setPosition(x0_pos, y0_pos);
            Pixel.setFillColor(sf::Color::Black);
            if (cur_iter < MAX_ITER)
                Pixel.setFillColor(sf::Color{(BYTE)cur_iter * 5, (BYTE) cur_iter * 10, (BYTE) 255 - cur_iter});

            if (DRAW)
                window.draw(Pixel);
        }
    }
}

void AVXDrawMandlbr(sf::RenderWindow &window, float center_x, float center_y) {
    __m256 x_shifts = _mm256_set_ps (7*dx, 6*dx, 5*dx, 4*dx, 3*dx, 2*dx, dx, 0);
    __m256 y_brdr_arr = _mm256_set1_ps (y_brdr);
    __m256 x_brdr_arr = _mm256_set1_ps (x_brdr);

    __m256 y_scale = _mm256_set1_ps (1/dy);
    __m256 x_scale = _mm256_set1_ps (1/dx);

    sf::RectangleShape Pixel = sf::RectangleShape(sf::Vector2f(1, 1));
    float y_min = -y_brdr - center_y, y_max = y_brdr - center_y;
    float x_min = -x_brdr - center_x, x_max = x_brdr - center_x;

    __m256 y_const_shift = _mm256_set1_ps (y_max);
    __m256 x_const_shift = _mm256_set1_ps (-x_min);

    for (float y0 = y_min; y0 <= y_max; y0 += dy) {
        __m256 y0_arr = _mm256_set1_ps (y0);                  // (y0, ... , y0)
        __m256 y_pos = _mm256_mul_ps (_mm256_sub_ps (y_const_shift, y0_arr), y_scale);  //(y_brdr - center_y - y0)*1/dy                 
        
        for (float x0 = x_min; x0 <= x_max; x0 += 8*dx) {
            __m256 x0_arr = _mm256_add_ps (_mm256_set1_ps (x0), x_shifts);                 // (x0, x0 + dx, ... , x0 + 7dx)
            __m256 x_pos = _mm256_mul_ps (_mm256_add_ps (x_const_shift, x0_arr), x_scale); //(x_brdr + center_x + x0)*1/dx      

            __m256i cur_iters =  _mm256_set1_epi32(0);
            __m256 x = x0_arr;
            __m256 y = y0_arr;

             for (int iterator = 0; iterator < MAX_ITER; iterator++) {
            
                __m256 x2 = _mm256_mul_ps(x, x);
                __m256 y2 = _mm256_mul_ps(y, y);
                __m256 xy = _mm256_mul_ps(x, y);

                __m256 dist = _mm256_add_ps(x2, y2);
                __m256 mask = _mm256_cmp_ps(dist, max_dist, _CMP_LT_OQ); // (FFFFFFFF (= -1) if true, 0 if false)

                int res = _mm256_movemask_ps(mask);
                if (!res)  break;                                        // all distances are out of range
           
                cur_iters = _mm256_sub_epi32 (cur_iters, _mm256_castps_si256 (mask));   //cur_iter + 1 or cur_iter + 0

                x = _mm256_add_ps(_mm256_sub_ps(x2, y2), x0_arr);
                y = _mm256_add_ps(_mm256_add_ps(xy, xy), y0_arr);
            
             }
            
            float* x_cords = (float*) &x_pos;
            float* y_cords = (float*) &y_pos;
            int* iters = (int*) &cur_iters;
            for (int i = 0; i < 8; i++) {
                Pixel.setPosition(x_cords[i], y_cords[i]);
                Pixel.setFillColor(sf::Color::Black);
                int n = iters[i];
                if (n < MAX_ITER) {
                    Pixel.setFillColor(sf::Color((BYTE)n * 30, (BYTE) n * 5, (BYTE) 255 - n));
                }
                if (DRAW)
                    window.draw(Pixel);
            }   
        }
    }
}

sf::Text *GenerateTextSprite (sf::Font &font, char* content, float x_coord, float y_coord) {
    sf::Text *text = new sf::Text;          

    text->setFont(font);
    text->setFillColor(sf::Color::Red);
    text->setString(content);
    text->setCharacterSize(48);
    text->setPosition(x_coord, y_coord);

    return text;
}
