#include "mandelbr.hpp"

#define AVX 0
#define DRAW 1

__m256 max_dist = _mm256_set1_ps (MAX_DISTANCE);

int main() {
    sf::RenderWindow window(sf::VideoMode(W_HEIGHT, W_WIDTH), "Mandelbrot");
    float cent_x = 0.0, cent_y = 0.0;

    sf::Clock clock;                               
    sf::Font font;
    font.loadFromFile("caviar-dreams.ttf");
    sf::Text fps_text = *SetText (font, 0, 0);
    sf::Text scroll_text = *SetText(font, (float) W_WIDTH - 250.f, 0);
    float scrollScale = 1;

    sf::Image image;
    image.create(W_WIDTH, W_HEIGHT, sf::Color::Black);

    sf::Texture texture;
    texture.loadFromImage(image);

    sf::Sprite sprite;
    sprite.setTexture(texture);

    while (window.isOpen()) {

        clock.restart();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            cent_x += 0.1 * scrollScale;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            cent_x -= 0.1 * scrollScale;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            cent_y -= 0.1 * scrollScale;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            cent_y += 0.1 * scrollScale;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            x_brdr *= 0.9;
            y_brdr *= 0.9;
            dx = 2 * x_brdr / (float)W_WIDTH;
            dy = 2 * y_brdr / (float)W_HEIGHT;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            x_brdr *= 1.2;
            y_brdr *= 1.2;
            dx = 2 * x_brdr / (float)W_WIDTH;
            dy = 2 * y_brdr / (float)W_HEIGHT;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            scrollScale *= 1.25;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            scrollScale *= 0.8;
        
        sf::Event event;
        while (window.pollEvent(event)) {  
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        if (AVX)
            AVXDrawMandlbr(image, cent_x, cent_y);
        else
            DrawMandlbr(image, cent_x, cent_y);
        
        sf::Time elapsed_time = clock.getElapsedTime();

        char textFPS[20];
        sprintf (textFPS, "FPS: %.2f\n", 1/elapsed_time.asSeconds());

        char textSCROLL[20];
        sprintf (textSCROLL, "Scroll Scale: %.3f\n", scrollScale);

        window.clear(sf::Color::Black);

        texture.update(image);
        fps_text.setString (textFPS);
        scroll_text.setString(textSCROLL);

        window.draw (sprite);
        window.draw (fps_text);
        window.draw(scroll_text);

        window.display();
   
    }

    return 0;
}

void DrawMandlbr(sf::Image &image, float center_x, float center_y) {
    
    float y_min = -y_brdr - center_y, y_max = y_brdr - center_y;
    float x_min = -x_brdr - center_x, x_max = x_brdr - center_x;
    for (int y0_pix = 0; y0_pix <= W_HEIGHT; y0_pix++) {
        float y0 = y_max - (float)y0_pix * dy; 
        
        for (int x0_pix = 0; x0_pix <= W_WIDTH; x0_pix++) {
            float x0 = (float) x0_pix * dx + x_min;
    
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

            if (DRAW) {
                sf::Color color; 
                if (cur_iter < MAX_ITER) {
                    color = sf::Color((BYTE)cur_iter * 30, (BYTE) cur_iter * 5, (BYTE) 255 - cur_iter);
                } else {
                    color = sf::Color::Black;
                }
                image.setPixel(x0_pix, y0_pix, color);
            } 
        }
    }
}

void AVXDrawMandlbr(sf::Image &image, float center_x, float center_y) {
    __m256i i_x_shifts = _mm256_set_epi32 (7, 6, 5, 4, 3, 2, 1, 0);
    __m256 f_x_shifts = _mm256_set_ps (7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f, 0.f);

    __m256 dy_arr = _mm256_set1_ps (dy);
    __m256 dx_arr = _mm256_set1_ps (dx);

    float y_max = y_brdr - center_y;
    float x_min = -x_brdr - center_x;
    __m256 y_max_arr = _mm256_set1_ps (y_max);
    __m256 x_min_arr = _mm256_set1_ps (x_min);


    for (int y0_pix = 0; y0_pix <= W_HEIGHT; y0_pix++) {
        __m256i y_pos = _mm256_set1_epi32 (y0_pix);                   //(y0, ... , y0)
        __m256 f_y_pos = _mm256_set1_ps ((float) y0_pix);      // (float) (y0, ... , y0)

        __m256 y0_arr = _mm256_sub_ps (y_max_arr,  _mm256_mul_ps(f_y_pos, dy_arr));  // float y0 = y_max - (float)y0_pix * dy;                
        
        for (int x0_pix = 0; x0_pix + 8 <= W_WIDTH; x0_pix += 8) {
            __m256i x_pos = _mm256_add_epi32 (_mm256_set1_epi32 (x0_pix), i_x_shifts);   // (x0, x0 + 1, ... , x0 + 7)
            __m256 f_x_pos = _mm256_add_ps (_mm256_set1_ps ((float)x0_pix), f_x_shifts);  // (float) (x0, x0 + 1, ... , x0 + 7)
            __m256 x0_arr = _mm256_add_ps (_mm256_mul_ps(f_x_pos, dx_arr), x_min_arr); //  float x0 = (float) x0_pix * dx + x_min     

            __m256i cur_iters =  _mm256_set1_epi32(0);
            __m256 x = x0_arr;
            __m256 y = y0_arr;

             for (int iterator = 0; iterator < MAX_ITER; iterator++) {
            
                __m256 x2 = _mm256_mul_ps(x, x);
                __m256 y2 = _mm256_mul_ps(y, y);
                __m256 xy = _mm256_mul_ps(x, y);

                __m256 dist = _mm256_add_ps(x2, y2);
                
                __m256 mask = _mm256_cmp_ps(dist, max_dist, _CMP_LT_OQ);      // FFFFFFFF (= -1) if true, 0 if false

                int res = _mm256_movemask_ps(mask);
                if (!res)  break;                                             // all distances are out of range
           
                cur_iters = _mm256_sub_epi32 (cur_iters, _mm256_castps_si256 (mask));   //cur_iter + 1 or cur_iter + 0

                x = _mm256_add_ps(_mm256_sub_ps(x2, y2), x0_arr);
                y = _mm256_add_ps(_mm256_add_ps(xy, xy), y0_arr);
            
             }

            u_int32_t* x_cords = (u_int32_t*) &x_pos;
            u_int32_t  y_cord  = * (u_int32_t*) &y_pos;
            int* iters   = (int*) &cur_iters;
            if (DRAW) {
                for (int i = 0; i < 8; i++) {
                    sf::Color color; 
                    int n = iters[i];
                    //printf("n is %d\n", n);
                    //getchar();
                    if (n < MAX_ITER) {
                        color = sf::Color((BYTE)n * 30, (BYTE) n * 5, (BYTE) 255 - n);
                    } else {
                        color = sf::Color::Black;
                    }
                    //printf("x_cords: %d, y_cord: %d\n", x_cords[i], y_cord);
                    //getchar();
                    
                    image.setPixel(x_cords[i], y_cord, color);
                } 
            }
        }
    }
}

sf::Text *SetText (sf::Font &font, float x_coord, float y_coord) {
    sf::Text *text = new sf::Text;          

    text->setFont(font);
    text->setFillColor(sf::Color::Yellow);
    text->setCharacterSize(30);
    text->setPosition(x_coord, y_coord);

    return text;
}
