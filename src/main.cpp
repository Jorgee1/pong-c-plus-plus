#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <string>
#include <stdlib.h>
#include <ctime>
#include <vector>

#include "window.hpp"
#include "texture.hpp"
#include "action.hpp"
#include "entity.hpp"


class Options{
    public:
        std::vector<std::string> options;
        int index;
        bool wrap;
        Options(){
            index = 0;
            wrap = false;
        }

        void add(std::string option){
            options.push_back(option);
        }

        void next(){
            if(index < options.size() - 1){
                index++;
            }else if(wrap){
                index = 0;
            }
        }
        
        void prev(){
            if(index > 0){
                index--;
            }else if(wrap){
                index = options.size() - 1;
            }
        }

};

class ConfigMenu{
    public:
        int id;
        SDL_Renderer* renderer;
        TextureText* text_normal;
        TextureText* text_selected;
        
        Options config_menu;
        Options config_menu_op;
        Options config_menu_op2;
        Options config_menu_op3;
                
        enum config_menu_ids{
            CONFIG_OP1,
            CONFIG_OP2,
            CONFIG_OP3,
            CONFIG_RETURN,
        };

        enum config_menu_op1_ids{
            CONFIG_OP1_OP1,
            CONFIG_OP1_OP2,
            CONFIG_OP1_OP3,
        };
        
        enum config_menu_op2_ids{
            CONFIG_OP2_OP1,
            CONFIG_OP2_OP2,
            CONFIG_OP2_OP3,
        };
                
        enum config_menu_op3_ids{
            CONFIG_OP3_OP1,
            CONFIG_OP3_OP2,
            CONFIG_OP3_OP3,
        };
        
        
        ConfigMenu(){
            renderer      = NULL;
            text_normal   = NULL;
            text_selected = NULL;
            id            =   -1;
        }
        
        void init(int id, SDL_Renderer* renderer, TextureText* text_normal, TextureText* text_selected){
            this->renderer      =      renderer;
            this->text_normal   =   text_normal;
            this->text_selected = text_selected;
            this->id            =            id;
            
            this->config_menu.add("Res");
            this->config_menu.add("OP2");
            this->config_menu.add("OP3");
            this->config_menu.add("Return");
            this->config_menu.index = CONFIG_OP1;
            
            this->config_menu_op.add("640x480");
            this->config_menu_op.add("800x800");
            this->config_menu_op.add("1600x900");
            this->config_menu_op.index = CONFIG_OP1_OP1;

            this->config_menu_op2.add("arx");
            this->config_menu_op2.add("mix");
            this->config_menu_op2.add("asdf");
            this->config_menu_op2.index = CONFIG_OP2_OP1;

            this->config_menu_op3.add("x2");
            this->config_menu_op3.add("x4");
            this->config_menu_op3.add("x6");
            this->config_menu_op3.index = CONFIG_OP3_OP1;
        }
        
        void render(){
            for(int i=0; i<config_menu.options.size(); i++){
                if(config_menu.index == i){
                    text_selected->render(
                        10, 10 + (i)*text_selected->size,
                        config_menu.options[i],
                        text_selected->RIGHT
                    );
                }else{
                    text_normal->render(
                        10, 10 + (i)*text_normal->size,
                        config_menu.options[i],
                        text_normal->RIGHT
                    );
                }
            }
            
            text_normal->render(
                100, 10,
                config_menu_op.options[config_menu_op.index],
                text_normal->RIGHT
            );
            
            text_normal->render(
                100, 10 + text_normal->size,
                config_menu_op2.options[config_menu_op2.index],
                text_normal->RIGHT
            );
            
            text_normal->render(
                100, 10 + 2*text_normal->size,
                config_menu_op3.options[config_menu_op3.index],
                text_normal->RIGHT
            );
        }
};


bool check_collition(
        SDL_Rect A,
        SDL_Rect B
){
    /// PARA A
    int A_IZQ = A.x;
    int A_DER = A.x + A.w;
    int A_ARR = A.y;
    int A_ABJ = A.y + A.h;
    /// PARA B
    int B_IZQ = B.x;
    int B_DER = B.x + B.w;
    int B_ARR = B.y;
    int B_ABJ = B.y + B.h;
    /// Restriccones
    if(
        (A_ABJ >= B_ARR) &&
        (A_ARR <= B_ABJ) &&
        (A_DER >= B_IZQ) &&
        (A_IZQ <= B_DER) 
    ){
        return true;
    }
    return false;
}

int main( int argc, char* args[] ){
    srand (time(0));
    int SCREEN_WIDTH  = 640;
    int SCREEN_HEIGHT = 480;
    int TEXT_SIZE =  SCREEN_HEIGHT/12;
    
    int VEL       = SCREEN_HEIGHT/100;
    int BALL_SIZE = SCREEN_HEIGHT/50;

    int PLAYER_WIDHT  = SCREEN_HEIGHT/50;
    int PLAYER_HEIGHT = SCREEN_HEIGHT/6;
    int PLAYER_PADING = SCREEN_HEIGHT/10;
    
    int WIN_CONDITION = 3;
    int score[2] = {0,0};

    Uint32 start_view_timer = 600;

    bool exit  = false;
    bool pause = false;

    Uint32 ref_timer = 0;
 
    enum entity{
        BALL,
        PLAYER1,
        PLAYER2
    };
    
    enum views{
        VIEW_START,
        VIEW_GAME,
        VIEW_OPTIONS,
        VIEW_GAME_OVER,
        VIEW_CONFIG
    };
    
    
    enum game_over_menu_ids{
        GAME_OVER_YES,
        GAME_OVER_NO
    };
    Options game_over_menu;
    game_over_menu.add("YES");
    game_over_menu.add("NO");

    
    enum pause_menu_ids{
        PAUSE_CONTINUE,
        PAUSE_OPTIONS,
        PAUSE_EXIT,
    };
    Options pause_menu;
    pause_menu.add("Continue");
    pause_menu.add("Options");
    pause_menu.add("Exit");
    pause_menu.index = PAUSE_CONTINUE;
    
    int view_index = VIEW_START;
      
    std::string GAME_NAME = "PONG";
    std::string PATH(SDL_GetBasePath());

    std::string PATH_FONT = PATH + "asset/font/LiberationMono-Regular.ttf";
    std::string PATH_ICON = PATH + "asset/icon.bmp";

    SDL_Color COLOR_BLACK = {0x00, 0x00, 0x00, 0xFF};
    SDL_Color COLOR_RED   = {0xFF, 0x00, 0x00, 0xFF};
    SDL_Color COLOR_GREEN = {0x00, 0xFF, 0x00, 0xFF};
    SDL_Color COLOR_BLUE  = {0x00, 0x00, 0xFF, 0xFF};
    SDL_Color COLOR_WHITE = {0xFF, 0xFF, 0xFF, 0xFF};

    Entity ball;
    Entity player1;
    Entity player2;
    
    ball.init(
        SCREEN_WIDTH/2, SCREEN_HEIGHT/2,
        BALL_SIZE, BALL_SIZE, {VEL, VEL/2},
        COLOR_WHITE
    );

    ball.reset_direction();


    player1.init(
        PLAYER_PADING, SCREEN_HEIGHT/2 - PLAYER_HEIGHT,
        PLAYER_HEIGHT, PLAYER_WIDHT, {VEL, VEL},
        COLOR_WHITE
    );
    
    player2.init(
        SCREEN_WIDTH - PLAYER_WIDHT - PLAYER_PADING, SCREEN_HEIGHT/2 - PLAYER_HEIGHT,
        PLAYER_HEIGHT, PLAYER_WIDHT, {VEL, VEL},
        COLOR_WHITE
    );
    
    Window window(
        GAME_NAME.c_str(),
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        COLOR_BLACK
    );
    window.set_icon(PATH_ICON);

    TextureText text_white(
        window.get_render(),
        PATH_FONT,
        COLOR_WHITE,
        TEXT_SIZE
    );
    
    TextureText text_red(
        window.get_render(),
        PATH_FONT,
        COLOR_RED,
        TEXT_SIZE
    );
    
    Action* action = Action::get_instance();
    action->init(
        SDL_SCANCODE_Z,      // BUTTON_ACTION
        SDL_SCANCODE_X,      // BUTTON_CANCEL
        SDL_SCANCODE_RETURN, // BUTTON_START
        SDL_SCANCODE_UP,     // BUTTON_MOVE_UP
        SDL_SCANCODE_DOWN,   // BUTTON_MOVE_DOWN
        SDL_SCANCODE_LEFT,   // BUTTON_MOVE_LEFT
        SDL_SCANCODE_RIGHT   // BUTTON_MOVE_RIGHT
    );
    
    ConfigMenu config_menu;
    config_menu.init(
        VIEW_CONFIG,
        window.get_render(),
        &text_white,
        &text_red
    );
    
    ref_timer = SDL_GetTicks();

    while(exit == false){
        if(window.check_exit()){
            exit = true;
        }else{
            window.clear_screen();
                        
            // Logic
            switch(view_index){
                case VIEW_GAME:{
                    if(pause){
                        if(action->check_action(action->BUTTON_START)){
                            
                            switch(pause_menu.index){
                                case PAUSE_CONTINUE:{
                                    pause = false;
                                    break;
                                }
                                case PAUSE_OPTIONS:{
                                    view_index = config_menu.id;
                                    break;
                                }
                                case PAUSE_EXIT:{
                                    exit = true;
                                    break;
                                }
                            }
                                                  
                        }else if(action->check_action(action->BUTTON_MOVE_DOWN)){
                            pause_menu.next();
                        }else if(action->check_action(action->BUTTON_MOVE_UP)){
                            pause_menu.prev();
                        }
                    }else{
                        //PLAYER
                        if(action->get_state(action->BUTTON_MOVE_UP)){
                            player1.move_up();
                        }else if(action->get_state(action->BUTTON_MOVE_DOWN)){
                            player1.move_down();
                        }else{
                            player1.stop();
                        }

                        if(action->check_action(action->BUTTON_START)){
                            pause = true;
                        }

                        if((player1.y <= 0) && (player1.speed.y < 0)){
                            player1.stop_y();
                            player1.y = 0;
                        }else if(
                            (player1.y + player1.h >= SCREEN_HEIGHT) &&
                            (player1.speed.y > 0)   
                        ){
                            player1.stop_y();
                            player1.y = SCREEN_HEIGHT - player1.h;
                        }

                        // PC
                        if (ball.speed.x>0){
                            if(player2.y + (player2.h/2) + 20 < ball.y){
                                player2.move_down();
                            }else if(player2.y + (player2.h/2) - 20 > ball.y){
                                player2.move_up();
                            }else{
                                player2.stop();
                            }
                        }else{
                            player2.stop();
                        }

                        if(
                            (player2.y + player2.h >= SCREEN_HEIGHT) &&
                            (player2.speed.y > 0)
                        ){
                            player2.stop_y();
                            player2.y = SCREEN_HEIGHT - player2.h;
                        }else if((player2.y <= 0) && (player2.speed.y < 0)){
                            player2.stop_y();
                            player2.y = 0;
                        }
                        // ball movement
                        
                        if(ball.x + ball.w > SCREEN_WIDTH){
                            score[PLAYER1 - 1]++;
                            ball.x = SCREEN_WIDTH/2;
                            ball.y = rand() % SCREEN_HEIGHT;
                            ball.reset_direction();
                        }
                        if(ball.x < 0){
                            score[PLAYER2 - 1]++;
                            ball.x = SCREEN_WIDTH/2;
                            ball.y = rand() % SCREEN_HEIGHT;
                            ball.reset_direction();
                        }

                        // Bounce on screen
                        if(ball.y + ball.h > SCREEN_HEIGHT){
                            ball.move_up();
                        }
                        if(ball.y < 0){
                            ball.move_down();
                        }

                        // bounce on each elements

                        if(ball.speed.x < 0){
                            if(
                                check_collition(
                                    ball.get_rect(),
                                    player1.get_rect()
                                )
                            ){
                                ball.move_right();
                            }
                        }

                        if(ball.speed.x > 0){
                            if(
                                check_collition(
                                    ball.get_rect(),
                                    player2.get_rect()
                                )
                            ){
                                ball.move_left();
                            }
                        }

                        if(
                            (score[PLAYER1 - 1] >= WIN_CONDITION) ||
                            (score[PLAYER2 - 1] >= WIN_CONDITION)
                        ){
                            view_index = VIEW_GAME_OVER;
                            game_over_menu.index = GAME_OVER_YES;
                        }
                    }
                    break;
                }
                
                case VIEW_START:{
                    if(action->check_action(action->BUTTON_START)){
                        view_index = VIEW_GAME;
                    }
                    break;
                }
                
                case VIEW_GAME_OVER:{
                    if(action->check_action(action->BUTTON_START)){
                        if(game_over_menu.index == GAME_OVER_YES){
                            view_index = VIEW_START;
                            score[PLAYER1 - 1] = 0;
                            score[PLAYER2 - 1] = 0;
                            ball.x = SCREEN_WIDTH/2;
                            ball.y = rand() % SCREEN_HEIGHT;
                            ball.reset_direction();
                        }else{
                            exit = true;
                        }
                    }else if(action->check_action(action->BUTTON_MOVE_UP)){
                        game_over_menu.prev();
                    }else if(action->check_action(action->BUTTON_MOVE_DOWN)){
                        game_over_menu.next();
                    }
                    break;
                }
                case VIEW_CONFIG:{
                    if(action->check_action(action->BUTTON_START)){
                        if(config_menu.config_menu.index == ConfigMenu::CONFIG_RETURN){
                            view_index = VIEW_GAME;
                        }
                    }else if(action->check_action(action->BUTTON_MOVE_UP)){
                        config_menu.config_menu.prev();
                    }else if(action->check_action(action->BUTTON_MOVE_DOWN)){
                        config_menu.config_menu.next();
                    }else if(action->check_action(action->BUTTON_MOVE_LEFT)){
                        switch(config_menu.config_menu.index){
                            case ConfigMenu::CONFIG_OP1:{
                                config_menu.config_menu_op.prev();
                                break;
                            }
                            case ConfigMenu::CONFIG_OP2:{
                                config_menu.config_menu_op2.prev();
                                break;
                            }
                            case ConfigMenu::CONFIG_OP3:{
                                config_menu.config_menu_op3.prev();
                                break;
                            }
                        }

                    }if(action->check_action(action->BUTTON_MOVE_RIGHT)){
                        switch(config_menu.config_menu.index){
                            case ConfigMenu::CONFIG_OP1:{
                                config_menu.config_menu_op.next();
                                break;
                            }
                            case ConfigMenu::CONFIG_OP2:{
                                config_menu.config_menu_op2.next();
                                break;
                            }
                            case ConfigMenu::CONFIG_OP3:{
                                config_menu.config_menu_op3.next();
                                break;
                            }
                        }

                    }
                    break;
                }
            }


            // Update world
            if( !pause && view_index==VIEW_GAME){
                ball.update();
                player1.update();
                player2.update();
            }

            // Render            
            switch(view_index){
                case VIEW_START:{
                   text_white.render(
                        SCREEN_WIDTH/2, TEXT_SIZE,
                        GAME_NAME,
                        text_white.CENTER
                    );


                    if(SDL_GetTicks() - ref_timer < start_view_timer){
                        text_white.render(
                            SCREEN_WIDTH/2, SCREEN_HEIGHT - 2*TEXT_SIZE,
                            "PRESS START",
                            text_white.CENTER
                        );
                    }else if(SDL_GetTicks() - ref_timer > 2*start_view_timer){
                        ref_timer = SDL_GetTicks();
                    }
                    break;

                }
                case VIEW_GAME:{
                    // Draw entitys
                    window.draw_rectangle(ball.get_rect(), COLOR_WHITE);
                    window.draw_rectangle(player1.get_rect(), COLOR_WHITE);
                    window.draw_rectangle(player2.get_rect(), COLOR_WHITE);

                    // Draw UI
                    text_white.render(
                        SCREEN_WIDTH/2-  10, 0,
                        std::to_string(score[PLAYER1 - 1]),
                        text_white.LEFT
                    );

                    text_white.render(
                        SCREEN_WIDTH/2 + 10, 0,
                        std::to_string(score[PLAYER2 - 1])
                    );
                    
                    window.draw_line(
                        {SCREEN_WIDTH/2, 0},
                        {SCREEN_WIDTH/2, SCREEN_HEIGHT},
                        COLOR_WHITE
                    );
                    
                    if(pause){

                        text_white.render(
                            SCREEN_WIDTH/2,
                            SCREEN_HEIGHT/2 - TEXT_SIZE/2,
                            "PAUSE",
                            text_white.CENTER
                        );


                        for(int i=0; i<pause_menu.options.size(); i++){
                            if(pause_menu.index == i){
                                text_red.render(
                                    SCREEN_WIDTH/2,
                                    SCREEN_HEIGHT/2 + (2 + i) *TEXT_SIZE,
                                    pause_menu.options[i],
                                    text_red.CENTER
                                );
                            }else{
                                text_white.render(
                                    SCREEN_WIDTH/2,
                                    SCREEN_HEIGHT/2 + (2+i)*TEXT_SIZE,
                                    pause_menu.options[i],
                                    text_white.CENTER
                                );
                            }
                        }
                    }
                    break;
                }
                case VIEW_GAME_OVER:{
                    text_white.render(
                        SCREEN_WIDTH/2, TEXT_SIZE,
                        "GAME OVER",
                        text_white.CENTER
                    );

                    text_white.render(
                        SCREEN_WIDTH/2, 3*TEXT_SIZE,
                        "Continue?",
                        text_white.CENTER
                    );

                    for(int i=0; i<game_over_menu.options.size(); i++){
                        if(game_over_menu.index == i){
                            text_red.render(
                                SCREEN_WIDTH/2, (5 + i) *TEXT_SIZE,
                                game_over_menu.options[i],
                                text_red.CENTER
                            );
                        }else{
                            text_white.render(
                                SCREEN_WIDTH/2, (5 + i)*TEXT_SIZE,
                                game_over_menu.options[i],
                                text_white.CENTER
                            );
                        }
                    }


                    break;
                }
                case VIEW_CONFIG:{              
                    config_menu.render();
                    break;
                }
            }

            window.update_screen();
        }

    }
    return 0;
}
