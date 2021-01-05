#include "entity.hpp"

Entity::Entity(){
    x = 0;
    y = 0;
    h = 0;
    w = 0;
    speed.x = 0;
    speed.y = 0;
    speed_limit.x = 0;
    speed_limit.y = 0;
    color = {0, 0, 0, 0xFF};
}

void Entity::init(
    int x, int y, int h, int w,
    SDL_Point speed, SDL_Color color
){
    this->x = x;
    this->y = y;
    this->h = h;
    this->w = w;
    this->speed_limit = speed;
    this->color = color;
}

void Entity::move_up(){
    speed.y = -speed_limit.y;
}

void Entity::move_down(){
    speed.y = +speed_limit.y;
}

void Entity::move_left(){
    speed.x = -speed_limit.x;
}

void Entity::move_right(){
    speed.x = +speed_limit.x;
}

void Entity::stop(){
    speed.x = 0;
    speed.y = 0;
}

void Entity::stop_x(){
    speed.x = 0;
}

void Entity::stop_y(){
    speed.y = 0;
}

void Entity::reset_direction(){
    int rand_value = rand() % 4;
    switch(rand_value){
        case 0:{
            move_up();
            move_right();
            break;
        }
        case 1:{
            move_down();
            move_right();
            break;
        }
        case 2:{
            move_up();
            move_left();
            break;
        }
        case 3:{
            move_down();
            move_left();
            break;
        }
    }
}

void Entity::update(){
    x += speed.x;
    y += speed.y;
}

SDL_Rect Entity::get_rect(){
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.h = h;
    rect.w = w;
    return rect;
}

SDL_Point Entity::get_position(){
    SDL_Point point;
    point.x = x;
    point.y = y;
    return point;
}
