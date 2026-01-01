#include "snake.h"
#include "stdlib.h"
#include <iostream>
#include <sys/ioctl.h>
#include <stdexcept>
#include <unistd.h>
#include <thread>
#include <chrono>

unsigned char RenderLoop::get_left_boundary_idx()
{
    return 1;
}

unsigned char RenderLoop::get_right_boundary_idx()
{
    return width - 1;
}

unsigned char RenderLoop::get_top_boundary_idx()
{
    return 1;
}

unsigned char RenderLoop::get_bottom_boundary_idx()
{
    return height - 1;
}

void RenderLoop::set_game_screen()
{
}

size_t RenderLoop::get_str_len(const char *str)
{
    size_t len = 0;
    for (const char *p = str; *p != '\0'; p += 1)
        len += 1;
    return len;
}

void RenderLoop::place_str_screen(const unsigned char *y_idx, const unsigned char *x_idx, const char *str)
{
    const char *p_str = str;
    char x_cr = *x_idx;
    while (true)
    {
        if (*p_str == '\0')
            break;
        screen[*y_idx][x_cr] = *p_str;
        p_str += 1;
        x_cr += 1;
    }
}

void RenderLoop::set_menu_screen()
{
    PrinterMode pm = CELL;
    unsigned char delay = 2;

    reset_screen();

    // Render bobbles
    unsigned char bobble_top_idex = get_top_boundary_idx() + 1;
    unsigned char bobble_bottom_idx = get_bottom_boundary_idx();
    unsigned char bobble_left_idx = get_left_boundary_idx() + 1;
    for (unsigned char i = bobble_top_idex; i < bobble_bottom_idx; i += 1)
    {
        place_str_screen(&i, &bobble_left_idx, bobbles_str);
    }
    print(&pm, &delay);

    reset_screen();

    unsigned char wm_y_idx = height / 2;
    unsigned char separate_line_y_idx = wm_y_idx + 1;
    unsigned char menu_item_one_y_idx = wm_y_idx + 2;
    unsigned char menu_item_two_y_idx = wm_y_idx + 3;

    unsigned char wm_x_idx = (width / 2) - (get_str_len(welcome_message_str) / 2);
    place_str_screen(&wm_y_idx, &wm_x_idx, welcome_message_str);

    unsigned char separate_line_x_idx = (width / 2) - (get_str_len(separator_line_str) / 2);
    place_str_screen(&separate_line_y_idx, &separate_line_x_idx, separator_line_str);

    unsigned char menu_item_one_x_idx = (width / 2) - (get_str_len(menu_item_one_str) / 2);
    place_str_screen(&menu_item_one_y_idx, &menu_item_one_x_idx, menu_item_one_str);

    unsigned char menu_item_two_x_idx = (width / 2) - (get_str_len(menu_item_two_str) / 2);
    place_str_screen(&menu_item_two_y_idx, &menu_item_two_x_idx, menu_item_two_str);

    print(&pm, &delay);
}

void RenderLoop::reset_screen()
{
    init_screen();
}

void RenderLoop::check_prereq()
{
    winsize w{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1)
        throw std::runtime_error("Exception: ioctl threw an exception");

    unsigned short int rows = w.ws_row;
    unsigned short int cols = w.ws_col;

    if (cols < width || rows < height)
        throw std::runtime_error("Exception: Small screen. Maximize the console.");
}

void RenderLoop::init_screen()
{
    /* screen pre-init */
    for (size_t i = 0; i < width; i += 1)
        for (size_t j = 0; j < height; j += 1)
            screen[i][j] = space_char;

    /* screen boundary - top and bottom*/
    unsigned char rows[2] = {
        get_top_boundary_idx(),
        get_bottom_boundary_idx()};
    for (size_t i = 1; i < width - 1; i += 1)
        for (size_t j = 0; j < 2; j += 1)
            screen[rows[j]][i] = i % 2 == 0 ? block_char : space_char;

    /* screen boundary - left and right */
    unsigned char columns[2] = {get_left_boundary_idx(), get_right_boundary_idx()};
    for (size_t i = 2; i < height - 1; i += 1)
        for (size_t j = 0; j < 2; j += 1)
            screen[i][columns[j]] = block_char;
}

void RenderLoop::print(const PrinterMode *printer_mode, const unsigned char *delay)
{

    if (printer_mode == nullptr)
        throw std::runtime_error("Exception: Printer mode is null.");

    std::cout << "\x1b[H";
    if (*printer_mode == CELL)
    {
        for (size_t i = 0; i < width; i += 1)
        {
            for (size_t j = 0; j < height; j += 1)
            {
                if (delay != nullptr)
                    std::this_thread::sleep_for(std::chrono::milliseconds(*delay));
                std::cout << screen[i][j] << '\a';
                std::cout << std::flush;
            }
            std::cout << "\n"
                      << std::flush;
        }
    }
    else if (*printer_mode == ROW)
    {
    }
}

void RenderLoop::render()
{
    check_prereq();
    init_screen();

    // Hide cursor
    std::cout << "\x1b[?25l";

    if (scene == MENU)
        set_menu_screen();
    if (scene == GAME)
        set_game_screen();

    // Hide cursor
    // std::cout << "\x1b[?25l";
    // while (!exit)
    // {
    //     // Move cursor to home (row 1 col 1), don't clear
    //     std::cout << "\x1b[H";

    //     if (scene == MENU)
    //         set_menu_screen();
    //     if (scene == GAME)
    //         set_game_screen();

    //     print();
    //     std::cout << std::flush; // important
    // }
    // Show cursor back (optional)
    // std::cout << "\x1b[?25h";
}

void SnakeGame::start()
{
    system("clear");
    RenderLoop render_loop;
    render_loop.render();
}