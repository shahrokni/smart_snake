#pragma once

#include "stdlib.h"

enum PrinterMode
{
    CELL,
    ROW,
    COLUMN
};

enum Scene
{
    MENU,
    GAME
};

class RenderLoop
{
public:
    void render();

private:
    void check_prereq();
    void init_screen();
    void reset_screen();
    void print(const PrinterMode *printer_mode, const unsigned char *delay);
    unsigned char get_left_boundary_idx();
    unsigned char get_right_boundary_idx();
    unsigned char get_top_boundary_idx();
    unsigned char get_bottom_boundary_idx();
    void set_scene();
    void set_menu_screen();
    void set_game_screen();
    void place_str_screen(const unsigned char *y_idx, const unsigned char *x_idx, const char *str);
    size_t get_str_len(const char *str);
    static constexpr unsigned char height = 30;
    static constexpr unsigned char width = 30;
    char screen[height][width];
    static constexpr char temp_indicator_char = '.';
    static constexpr char space_char = ' ';
    static constexpr char block_char = 'x';
    static constexpr char snake_body_char = 'o';
    static constexpr const char *welcome_message_str = "~SMART SNAKE~";
    static constexpr const char *separator_line_str = "-------------";
    static constexpr const char *menu_item_one_str = "PLAY GAME PRESS 1";
    static constexpr const char *menu_item_two_str = "SMART AUTOPLAY PRESS 2";
    static constexpr const char *bobbles_str = "  * * * * * * * * * * * * ";

    Scene scene = MENU;
    bool exit = false;
};

class SnakeGame
{
public:
    void start();

private:
    RenderLoop render_loop;
};