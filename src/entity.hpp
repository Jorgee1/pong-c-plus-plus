#include <SDL.h>

#ifndef ENTITY_H
#define ENTITY_H

class Entity{
    public:
        int x;
        int y;
        int h;
        int w;

        SDL_Point speed_limit;
        SDL_Point speed;
        SDL_Color color;
        
        Entity();
        
        void init(
            int, int, int, int,
            SDL_Point, SDL_Color
        );
        
        void move_up();
        void move_down();
        void move_left();
        void move_right();
        
        void stop();
        void stop_x();
        void stop_y();
        
        void reset_direction();
        
        void update();
        
        SDL_Rect get_rect();
        SDL_Point get_position();
};

#endif







