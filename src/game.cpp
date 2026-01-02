#include "game.h"
#include "stdlib.h"
#include <iostream>
#include <sys/ioctl.h>
#include <stdexcept>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <termios.h>

RenderLoop::RenderLoop()
{
    snake = new Snake;
    snake->head = new Position;
    snake->head->y = get_top_boundary_idx() + 1;
    snake->head->x = get_left_boundary_idx() + 4;
    snake->cnt = 3;
    snake->child = nullptr;
    snake->parent = nullptr;
    snake->direction = RIGHT;
    snake->joint = false;
}

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
    reset_screen();

    coin_position.y = (height / 2) + get_top_boundary_idx();
    coin_position.x = (width / 2) + get_left_boundary_idx();
    place_str_screen(&coin_position.y, &coin_position.x, coin_str);
    place_snake_str_screen(snake);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    while (true)
    {
        move_snake(snake, false);
        reset_screen();
        coin_position.y = (height / 2) + get_top_boundary_idx();
        coin_position.x = (width / 2) + get_left_boundary_idx();
        place_str_screen(&coin_position.y, &coin_position.x, coin_str);
        place_snake_str_screen(snake);
        print(nullptr);
        std::cout << '\a';
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void RenderLoop::set_autoplay_screen()
{
    return;
}

size_t RenderLoop::get_str_len(const char *str)
{
    size_t len = 0;
    for (const char *p = str; *p != '\0'; p += 1)
        len += 1;
    return len;
}

void RenderLoop::move_snake(Snake *snake, bool rec_call)
{
    if (snake == nullptr)
        return;

    if (snake->direction == RIGHT)
    {
        snake->head->x = snake->head->x + 1;
    }
    move_snake(snake->child, true);
}

void RenderLoop::place_snake_str_screen(Snake *snake)
{
    Snake *current_snake = snake;
    while (current_snake != nullptr)
    {
        screen[current_snake->head->y][current_snake->head->x] = snake_body_char;

        char y_printer_iterator, x_printer_iterator = 0;
        if (current_snake->direction == DOWN)
        {
            y_printer_iterator = -1;
            x_printer_iterator = 0;
        }
        else if (current_snake->direction == UP)
        {
            y_printer_iterator = 1;
            x_printer_iterator = 0;
        }
        else if (current_snake->direction == LEFT)
        {
            y_printer_iterator = 0;
            x_printer_iterator = 1;
        }
        else
        {
            y_printer_iterator = 0;
            x_printer_iterator = -1;
        }

        unsigned char row_x = current_snake->head->x + x_printer_iterator;
        unsigned char row_y = current_snake->head->y + y_printer_iterator;
        for (size_t i = 0; i < current_snake->cnt - 1; i += 1)
        {
            screen[row_y][row_x] = snake_body_char;
            row_x = row_x + x_printer_iterator;
            row_y = row_y + y_printer_iterator;
        }
        current_snake = current_snake->child;
    }
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
    unsigned char delay = 50;

    reset_screen();

    // Render bobbles
    unsigned char bobble_top_idex = get_top_boundary_idx() + 1;
    unsigned char bobble_bottom_idx = get_bottom_boundary_idx();
    unsigned char bobble_left_idx = get_left_boundary_idx() + 1;
    for (unsigned char i = bobble_top_idex; i < bobble_bottom_idx; i += 1)
    {
        place_str_screen(&i, &bobble_left_idx, bobbles_str);
    }
    print(&delay);

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

    print(&delay);

    while (pressed_key != '1' && pressed_key != '2')
        read_line(&pressed_key);

    scene = pressed_key == '1' ? GAME : AUTOPLAY;
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

void RenderLoop::print(const unsigned char *delay)
{

    std::cout << "\x1b[H";

    for (size_t i = 0; i < width; i += 1)
    {
        if (delay != nullptr)
            std::this_thread::sleep_for(std::chrono::milliseconds(*delay));
        for (size_t j = 0; j < height; j += 1)
        {
            std::cout << screen[i][j];
        }
        std::cout << "\n";
    }
}

void RenderLoop::read_line(unsigned char *pressed_key)
{
    ssize_t n;
    read(STDIN_FILENO, pressed_key, 1);
}

void RenderLoop::fix_console()
{
    termios original{};
    tcgetattr(STDIN_FILENO, &original);
    termios raw = original;

    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

void RenderLoop::clear_pressed_key(unsigned char *pressed_key)
{
    *pressed_key = ' ';
}

void RenderLoop::render()
{
    // Hide cursor
    std::cout << "\x1b[?25l";
    check_prereq();
    fix_console();
    init_screen();

    /* move to the menu scene */
    set_menu_screen();

    if (scene == GAME)
    {
        clear_pressed_key(&pressed_key);
        set_game_screen();
    }
    else
    {
        clear_pressed_key(&pressed_key);
        set_autoplay_screen();
    }
}

void SnakeGame::start()
{
    system("clear");
    RenderLoop render_loop;
    render_loop.render();
}