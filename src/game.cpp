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
    /* Snake */
    snake = new Snake;
    snake->start = new SnakePart;
    snake->start->postion = new Position;
    snake->start->postion->y = get_top_boundary_idx() + 1;
    snake->start->postion->x = get_left_boundary_idx() + 5;
    snake->start->cnt = 4;
    snake->start->next = nullptr;
    snake->start->prev = nullptr;
    snake->start->direction = RIGHT;
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

void RenderLoop::add_to_tail_snake(SnakePart *snake_part)
{
    if (snake_part->next == nullptr)
    {
        snake_part->cnt = snake_part->cnt + 1;
        return;
    }
    add_to_tail_snake(snake_part->next);
}

bool RenderLoop::got_score()
{
    return snake->start->postion->x == coin_position.x && snake->start->postion->y == coin_position.y;
}

bool RenderLoop::is_game_over()
{
    /* the following block checks bouncing against the wall */
    if (snake->start->direction == RIGHT && snake->start->postion->x == get_right_boundary_idx())
        return true;
    if (snake->start->direction == LEFT && snake->start->postion->x == get_left_boundary_idx())
        return true;
    if (snake->start->direction == DOWN && snake->start->postion->y == get_bottom_boundary_idx())
        return true;
    if (snake->start->direction == UP && snake->start->postion->y == get_top_boundary_idx())
        return true;

    /* the following block checks the self-destruction */
    unsigned char head_x = snake->start->postion->x;
    unsigned char head_y = snake->start->postion->y;
    SnakePart *snake_part_ref = snake->start->next;

    while (true)
    {
        if (snake_part_ref == nullptr)
            break;

        unsigned char p_x_end = 0;
        unsigned char p_y_end = 0;

        if (snake_part_ref->direction == UP)
        {
            p_x_end = snake_part_ref->postion->x;
            p_y_end = snake_part_ref->postion->y + snake_part_ref->cnt;
        }
        if (snake_part_ref->direction == DOWN)
        {
            p_x_end = snake_part_ref->postion->x;
            p_y_end = abs(snake_part_ref->postion->y - snake_part_ref->cnt);
        }
        if (snake_part_ref->direction == LEFT)
        {
            p_y_end = snake_part_ref->postion->y;
            p_x_end = snake_part_ref->postion->x + snake_part_ref->cnt;
        }
        if (snake_part_ref->direction == RIGHT)
        {
            p_y_end = snake_part_ref->postion->y;
            p_x_end = abs(snake_part_ref->postion->x - snake_part_ref->cnt);
        }

        if (std::min(p_x_end, snake_part_ref->postion->x) <= head_x && std::max(p_x_end, snake_part_ref->postion->x) >= head_x)
        {
            if (std::min(p_y_end, snake_part_ref->postion->y) <= head_y && std::max(p_y_end, snake_part_ref->postion->y) >= head_y)
            {
                return true;
            }
        }

        snake_part_ref = snake_part_ref->next;
    }

    return false;
}

void RenderLoop::game_over()
{
    reset_screen();

    unsigned char delay = 50;
    place_bobble_str_screen();
    print(&delay);

    reset_screen();
    print(&delay);

    unsigned char game_over_y_idx = height / 2;
    unsigned char game_over_x_idx = (width / 2) - get_str_len(game_over_str) / 2;
    place_str_screen(&game_over_y_idx, &game_over_x_idx, game_over_str);

    unsigned char developer_y_idx = game_over_y_idx + 1;
    unsigned char developer_x_idx = (width / 2) - get_str_len(developer_str) / 2;
    place_str_screen(&developer_y_idx, &developer_x_idx, developer_str);

    unsigned char i_love_c_y_idx = game_over_y_idx + 2;
    unsigned char i_love_c_x_idx = (width / 2) - get_str_len(i_love_c_str) / 2;
    place_str_screen(&i_love_c_y_idx, &i_love_c_x_idx, i_love_c_str);
    print(nullptr);
}

void RenderLoop::borrow_from_tail(SnakePart *snake_part)
{
    /* this is the tail */
    if (snake_part->next == nullptr)
    {
        snake_part->cnt -= 1;
        if (snake_part->cnt == 0)
        {
            snake_part->prev->next = nullptr;
            delete snake_part->postion;
            snake_part->postion = nullptr;
            delete snake_part;
        }
        return;
    }

    borrow_from_tail(snake_part->next);
}

void RenderLoop::change_snake_direction()
{
    if ((pressed_key == 'w' || pressed_key == 's') && (snake->start->direction == UP || snake->start->direction == DOWN))
        return;

    if ((pressed_key == 'd' || pressed_key == 'a') && (snake->start->direction == RIGHT || snake->start->direction == LEFT))
        return;

    if (pressed_key == ' ')
        return;

    SnakePart *snake_part = new SnakePart;
    snake_part->cnt = 1;
    snake_part->prev = nullptr;
    snake_part->next = snake->start;
    snake->start->prev = snake_part;
    borrow_from_tail(snake->start);
    /* replace the start */
    snake->start = snake_part;
    char x_corrector = 0;
    char y_corrector = 0;
    if (pressed_key == 's')
    {
        snake_part->direction = DOWN;
        x_corrector = 0;
        y_corrector = 1;
    }
    if (pressed_key == 'w')
    {
        snake_part->direction = UP;
        x_corrector = 0;
        y_corrector = -1;
    }
    if (pressed_key == 'a')
    {
        snake_part->direction = LEFT;
        x_corrector = -1;
        y_corrector = 0;
    }
    if (pressed_key == 'd')
    {
        snake_part->direction = RIGHT;
        x_corrector = 1;
        y_corrector = 0;
    }
    snake_part->postion = new Position;
    snake_part->postion->x = snake_part->next->postion->x + x_corrector;
    snake_part->postion->y = snake_part->next->postion->y + y_corrector;

    clear_pressed_key(&pressed_key);
}

void RenderLoop::set_game_screen()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    while (true)
    {
        if (is_game_over())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            game_over();
            break;
        }

        if (got_score())
        {
            add_to_tail_snake(snake->start);
            place_str_screen(&coin_position.y, &coin_position.x, &snake_body_char);
            std::cout << '\a';
            coin_position.x = 0;
            coin_position.y = 0;
            continue;
        }
        read_line(&pressed_key);
        change_snake_direction();
        reset_screen();
        move_snake(snake->start, false);
        place_snake_str_screen(snake);

        /* HERE, WE SHOULD SET THE COIN */
        if (coin_position.y != 0 && coin_position.x != 0)
            place_str_screen(&coin_position.y, &coin_position.x, coin_str);

        print(nullptr);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
}

size_t RenderLoop::get_str_len(const char *str)
{
    size_t len = 0;
    for (const char *p = str; *p != '\0'; p += 1)
        len += 1;
    return len;
}

void RenderLoop::move_snake(SnakePart *snake_part, bool rec_call)
{
    if (snake_part == nullptr)
        return;

    if (snake_part->cnt == 0)
    {

        snake_part->prev->next = nullptr;
        delete snake_part->postion;
        snake_part->postion = nullptr;
        delete snake_part;
        return;
    }

    char x_corrector = 0;
    char y_corrector = 0;
    if (snake_part->direction == RIGHT)
    {
        if (snake_part->prev == nullptr || snake_part->postion->x + 1 < snake_part->prev->postion->x)
        {
            x_corrector = 1;
            y_corrector = 0;
        }
    }
    else if (snake_part->direction == LEFT)
    {
        if (snake_part->prev == nullptr || snake_part->postion->x - 1 > snake_part->prev->postion->x)
        {
            x_corrector = -1;
            y_corrector = 0;
        }
    }
    else if (snake_part->direction == UP)
    {
        if (snake_part->prev == nullptr || snake_part->postion->y - 1 > snake_part->prev->postion->y)
        {
            x_corrector = 0;
            y_corrector = -1;
        }
    }
    else
    {
        if (snake_part->prev == nullptr || snake_part->postion->y + 1 < snake_part->prev->postion->y)
        {
            x_corrector = 0;
            y_corrector = 1;
        }
    }

    snake_part->postion->x += x_corrector;
    snake_part->postion->y += y_corrector;

    if (snake_part->next != nullptr && snake_part->next->cnt >= 1)
    {

        snake_part->cnt += 1;
        snake_part->next->cnt -= 1;
    }

    move_snake(snake_part->next, true);
}

void RenderLoop::place_snake_str_screen(Snake *snake)
{
    SnakePart *current_part = snake->start;
    while (current_part != nullptr)
    {
        screen[current_part->postion->y][current_part->postion->x] = snake_body_char;
        if (current_part->cnt > 1)
        {
            char y_printer_iterator, x_printer_iterator = 0;
            if (current_part->direction == DOWN)
            {
                y_printer_iterator = -1;
                x_printer_iterator = 0;
            }
            else if (current_part->direction == UP)
            {
                y_printer_iterator = 1;
                x_printer_iterator = 0;
            }
            else if (current_part->direction == LEFT)
            {
                y_printer_iterator = 0;
                x_printer_iterator = 1;
            }
            else
            {
                y_printer_iterator = 0;
                x_printer_iterator = -1;
            }

            unsigned char row_x = current_part->postion->x + x_printer_iterator;
            unsigned char row_y = current_part->postion->y + y_printer_iterator;
            for (size_t i = 0; i < current_part->cnt - 1; i += 1)
            {
                screen[row_y][row_x] = snake_body_char;
                row_x = row_x + x_printer_iterator;
                row_y = row_y + y_printer_iterator;
            }
        }
        current_part = current_part->next;
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

void RenderLoop::place_bobble_str_screen()
{
    reset_screen();
    // Render bobbles
    unsigned char bobble_top_idex = get_top_boundary_idx() + 1;
    unsigned char bobble_bottom_idx = get_bottom_boundary_idx();
    unsigned char bobble_left_idx = get_left_boundary_idx() + 1;
    for (unsigned char i = bobble_top_idex; i < bobble_bottom_idx; i += 1)
    {
        place_str_screen(&i, &bobble_left_idx, bobbles_str);
    }
}

void RenderLoop::set_menu_screen()
{
    unsigned char delay = 50;
    place_bobble_str_screen();
    print(&delay);

    reset_screen();

    unsigned char wm_y_idx = height / 2;
    unsigned char separate_line_y_idx = wm_y_idx + 1;
    unsigned char menu_item_one_y_idx = wm_y_idx + 2;

    unsigned char wm_x_idx = (width / 2) - (get_str_len(welcome_message_str) / 2);
    place_str_screen(&wm_y_idx, &wm_x_idx, welcome_message_str);

    unsigned char separate_line_x_idx = (width / 2) - (get_str_len(separator_line_str) / 2);
    place_str_screen(&separate_line_y_idx, &separate_line_x_idx, separator_line_str);

    unsigned char menu_item_one_x_idx = (width / 2) - (get_str_len(menu_item_one_str) / 2);
    place_str_screen(&menu_item_one_y_idx, &menu_item_one_x_idx, menu_item_one_str);

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

    // if (cols < width || rows < height)
    //  throw std::runtime_error("Exception: Small screen. Maximize the console.");
}

void RenderLoop::init_screen()
{
    /* screen pre-init */
    for (size_t i = 0; i < height; i += 1)
        for (size_t j = 0; j < width; j += 1)
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
}

void SnakeGame::start()
{
    system("clear");
    RenderLoop render_loop;
    render_loop.render();
}