#pragma once

#include "stdlib.h"

enum Scene
{
    GAME,
    AUTOPLAY
};

struct Position
{
    unsigned char x;
    unsigned char y;
};

enum Direction
{
    LEFT,
    RIGHT,
    UP,
    DOWN
};

struct Snake
{
    unsigned char cnt;
    bool joint;
    Snake *child;
    Snake *parent;
    Position *head;
    Direction direction;
};

class RenderLoop
{
public:
    RenderLoop();
    void render();

private:
    void check_prereq();
    void fix_console();
    void init_screen();
    void reset_screen();
    void print(const unsigned char *delay);
    unsigned char get_left_boundary_idx();
    unsigned char get_right_boundary_idx();
    unsigned char get_top_boundary_idx();
    unsigned char get_bottom_boundary_idx();
    void set_scene();
    void set_menu_screen();
    void set_game_screen();
    void set_autoplay_screen();
    void place_snake_str_screen(Snake *snake);
    void place_str_screen(const unsigned char *y_idx, const unsigned char *x_idx, const char *str);
    size_t get_str_len(const char *str);
    void read_line(unsigned char *pressed_key);
    void clear_pressed_key(unsigned char *pressed_key);
    void remove_snake_tail(Snake *snake);
    void add_to_tail_snake(Snake *snake);
    void change_direction_snake(Snake *snake);
    void move_snake(Snake *snake, bool rec_call);
    bool is_game_over();
    void game_over();
    void place_bobble_str_screen();
    bool got_score();
    unsigned char pressed_key;
    static constexpr unsigned char height = 30;
    static constexpr unsigned char width = 30;
    char screen[height][width];
    Position coin_position = {15, 2};
    Snake *snake;
    static constexpr const char temp_indicator_char = '.';
    static constexpr const char space_char = ' ';
    static constexpr const char block_char = 'x';
    static constexpr const char snake_body_char = 'o';
    static constexpr const char *coin_str = "$";
    static constexpr const char *welcome_message_str = "~SMART SNAKE~";
    static constexpr const char *separator_line_str = "-------------";
    static constexpr const char *menu_item_one_str = "PLAY GAME PRESS 1";
    static constexpr const char *menu_item_two_str = "SMART AUTOPLAY PRESS 2";
    static constexpr const char *bobbles_str = "  * * * * * * * * * * * * ";
    static constexpr const char *game_over_str = "GAME OVER!";
    static constexpr const char *developer_str = "DEVELOPED BY JSMAN :)";
    static constexpr const char *i_love_c_str = "I LOVE C/C++";
    Scene scene;
    bool exit = false;
};

class SnakeGame
{
public:
    void start();

private:
    RenderLoop render_loop;
};