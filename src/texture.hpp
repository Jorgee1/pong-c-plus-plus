#include <SDL.h>
#include <SDL_ttf.h>

#include <string>

#ifndef TEXTURE_H
#define TEXTURE_H

class Texture{
    public:
        SDL_Texture* texture;
        SDL_Renderer* renderer;
        
        Texture();

        Texture(SDL_Renderer *);
        ~Texture();
        
        SDL_Rect get_size();

        void load_bmp(std::string, int scale = 1);
        void rotate(int);

        void free();
        void render(int, int);

    private:
        int scale;
        int degree;
};

class TextureBlock: public Texture{
    public:
        TextureBlock();
        TextureBlock(
            SDL_Renderer*,
            SDL_Color,
            int, int
        );

        void init(SDL_Renderer*, SDL_Color, SDL_Rect);
        void create_texture(SDL_Color, SDL_Rect);
};

class TextureText{
    public:
        SDL_Renderer* renderer;
        SDL_Color bbox_color;
        bool bbox_flag;
        int size;
        enum h_pos{
            CENTER,
            LEFT,
            RIGHT
        };

        TextureText();
        TextureText(
            SDL_Renderer*,
            std::string,
            SDL_Color,
            int
        );

        void init(
            SDL_Renderer*,
            std::string,
            SDL_Color,
            int
        );

        void fill_array_null();
        void render(
            int, int,
            std::string,
            int horizontal_p = RIGHT
        );

        SDL_Rect get_text_size(std::string);
        Texture* char_to_texture(char);

    private:
        Texture texture_chars[128];
};

#endif
