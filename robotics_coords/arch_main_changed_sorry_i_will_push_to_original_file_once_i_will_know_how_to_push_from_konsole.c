#include <stdio.h>
#include <math.h>
#include <stdbool.h>
//#include <kipr/wombat.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164
#define WINDOW_HEIGHT 500
#define WINDOW_WIDTH 500

//warning: when running on arch, use escape sequence

// -ok

// btw the thing I added is in comments just in case

struct bottom_left {
    float xn;
    float yn;
};
struct bottom_left convert_coords(float x, float y) {
    struct bottom_left hp;
    hp.xn = -x + WINDOW_WIDTH;
    hp.yn = -y + WINDOW_HEIGHT;
    return hp;
}


void create_border(SDL_Renderer* r, int xpos, int ypos, int w, int h) {
    SDL_Rect border;
    border.x = xpos;
    border.y = ypos;
    border.w = w;
    border.h = h;
    //return border;
    SDL_SetRenderDrawColor(r, 255,255,255,255);
    SDL_RenderDrawRect(r, &border);
    SDL_RenderFillRect(r, &border);
    SDL_SetRenderDrawColor(r, 0,0,0,255);
}

void generate_point(SDL_Renderer* r, int32_t x_c, int32_t y_c, int32_t radius) { //dont make radius const
    SDL_SetRenderDrawColor(r, 255,0,0,(0xff)-1);
    //important information: SDL does not provide automatic way to draw a circle or any curves
    //use the midpoint algorithm:
    const int32_t d = radius * 2;
    int32_t x = radius - 1;
    int32_t y = 0;
    int32_t dx = 1;
    int32_t dy = 1;
    int32_t error = (dx - (radius << 1)); //radius is 32bit -> represents in binary and shift bit left

    while(x >= y) {
        SDL_RenderDrawPoint(r, x_c + x, y_c + y);
        SDL_RenderDrawPoint(r, x_c + y, y_c + x);
        SDL_RenderDrawPoint(r, x_c - y, y_c + x);
        SDL_RenderDrawPoint(r, x_c - x, y_c + y);
        SDL_RenderDrawPoint(r, x_c - x, y_c - y);
        SDL_RenderDrawPoint(r, x_c - y, y_c - x);
        SDL_RenderDrawPoint(r, x_c + y, y_c - x);
        SDL_RenderDrawPoint(r, x_c + x, y_c - y);
        if(error <= 0) {
            y++;
            error += dy;
            dy += 2;
        }
        if(error > 0) {
            x--;
            dx += 2;
            error += (dx - d);
        }
    }
    //i know it looks ugly but truly this algorithm works in about 500ms
    //probably will be even faster with predfined 40 digits of pi and some trig
}

void generate_arrow(SDL_Renderer* r, double x1, double y1, double x2, double y2) {
    float y_t = convert_coords(x2,y2).yn;
    int c = 20; //arbitrarily set magnitude of a line
    double phi = acos(x2 / sqrt((pow(x2,2.0) + pow(y2,2.0))) );
    float left = PI + (phi/2.0);
    float right = PI + (3.0*phi/2.0);
    
    // as you already have it just writing
    //



    printf("phi is %f \n", phi);

    SDL_SetRenderDrawColor(r, 0,255,0,255);
    SDL_RenderDrawLine(r,x1,convert_coords(x1,y1).yn,x2,y_t);
    SDL_SetRenderDrawColor(r, 255,0,0,255);
    SDL_RenderDrawLine(r,x2, y_t, (x2 + (c*cos(left))), (y_t - (c*sin(left))));
    SDL_RenderDrawLine(r,x2,y_t, x2 + (c*cos(right)), y_t - (c*sin(right)));


}

void clear_scheme(SDL_Renderer* render) {
    SDL_SetRenderDrawColor(render, 0, 0,0, 255);
    SDL_RenderClear(render);
    SDL_RenderPresent(render);
}
// void nthpos(SDL_Renderer* render, SDL_Rect* rect) {
//     SDL_SetRenderDrawColor(render, 255, 0, 0, 255);
//     SDL_RenderFillRect(render, rect);
//     SDL_RenderPresent(render);
// }

// void move(SDL_Rect* rect, float v) {
//     rect->x += v;
// }





void move(float* buffer, float* x, float* y, float v) {
    *x += v;
    *y += v;
    buffer[0] = *x;
    buffer[1] = *y;


}


bool is_collided(float* x, float* y, float* x2, float* y2, float height, float width) {
    //x2+y2=r2
    //x2, y2 are coords of another object of collision, idk how to make it look better
    //radius works according to size of a robot
    float power_2 = 2.0;
    float max_coord = sqrt(pow(height/power_2, 2)+pow(width/power_2, 2));

    if(abs(pow(x2-x, 2)+pow(y2-y, 2))<=pow(max_coord, 2)){
        return true;
    }else{
        return false;
    }
}



int main(int argc, char* argv[]) {
    uint32_t flags = SDL_RENDERER_ACCELERATED;
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("eror init sdl");
        exit(1);
    }
    printf("hello world\n");
    SDL_Window* win = SDL_CreateWindow("Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0); //turns out top left corner is origin (0,0)
    SDL_Renderer* render = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    //SDL_Surface* screen;
    //SDL_FreeSurface(screen);

    SDL_SetRenderDrawColor(render,0,0,0,255);
    SDL_Rect rect;
    rect.x = 400; rect.y=400; rect.h = 30; rect.w=40;
    //SDL_RenderClear(render);
    //SDL_RenderPresent(render);
    //SDL_Rect init = create_border(render, 0, 0, 10, 800); //x,y,w,h
    //create_border(render, 790, 0, 10, 800);
    //create_border(render, 0, 790, 800, 10);
    //create_border(render, 0,0, 800, 10);
    
    // generate_point(render, 5, 5, 4); // not even a full circle just an approximation
    //generate_arrow(render, 0.0,400.0,0.0,400.0); //x1,y1,x2,y2
    float velocity = 10.0;
    // SDL_Rect nr = {.x=100,.y=100,.h=30,.w=30};
    // generate_arrow(render,0.0,0.0,770.0,10.0);
    // clear_scheme(render);
    float xi = 0.0;
    float yi = 0.0;

    // --use other variables--
    float xi2 = 0.0;
    float yi2 = 0.0;
    double radius1 = 101.0;
    float xi3 = 100.0;
    float yi3 = 100.0;

    float width = 200.0;
    float height = 400.0;


    // --use other variables--


    float max_r = 400.0;
    //generate_arrow(render, xi, 0.0, max_r, 0.0);


    bool exit = false;
    while(!exit) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            switch (event.type)
            {
            case SDL_QUIT:
                exit = true;
                break;
            
            default:
                break;
            }
        }
	create_border(render, 0, 0, 10, 800);
	//SDL_Rect init = create_border(render,0,0,10,800);
	generate_arrow(render, 0, 0, 250, 200);
    float transform[2] = {0};
    move(transform, &xi, &yi, velocity);


// - Atai was here -
//    if(is_collided(&xi2, &yi2, &xi3, &yi3, height, width)){
        // I have no idea how to print bools on C
//        printf("collided");
//    }else{
//        printf("not collided");
//    }






    generate_arrow(render, 0.0, 0.0, max_r + transform[0], transform[1]);
    printf("xi is %f and yi is %f", xi, yi);
    SDL_Delay(100);
	//SDL_SetRenderDrawColor(render, 255, 0, 0, 255);
    	//SDL_RenderFillRect(render, &init);
	SDL_RenderPresent(render);
	SDL_Delay(1000/60);
        // nr.x += velocity;
        // clear_scheme(render);
        // nthpos(render, &nr);
    }
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(win);
    return 0;
}
