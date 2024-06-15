#include "raylib.h"
#include "raymath.h"
#include <algorithm>
#include <iostream>

class Cell
{
    private:
        Color color;
        Color colors_of_dead_cell[7] = {(Color){245, 2, 124, 255}, (Color){131, 1, 145, 255}, (Color){24, 173, 184, 255}, (Color){14, 110, 117, 255}, (Color){8, 62, 66, 255}, (Color){4, 31, 33, 255}, (Color){0, 0, 0, 255}};
        int current_decay_state = 6, decay_timer = 0;
        int color_decay_time[7] = {4, 4, 16, 16, 16, 16, 16};
        bool dead;
        bool states_record[2] = { false, false };
    
    public:
        Cell()
        {
            color = BLACK;
            dead = true;
            states_record[1] = true;
        }

        Cell(bool Dead)
        {
            dead = Dead;
            states_record[1] = dead;
            dead ? color = BLACK : color = WHITE;
        }

        Color get_color() { return color; }

        bool is_dead() { return dead; }

        void set_state(bool isDead)
        { 
            dead = isDead;
            std::swap(states_record[0], states_record[1]);
            states_record[1] = dead;
            if (!states_record[0] && states_record[1])
            {
                current_decay_state = 0;
            }
        }

        void update_color()
        {
            if (dead)
            {
                if (current_decay_state != 6)
                {
                    decay_timer++;
                    if (decay_timer > color_decay_time[current_decay_state])
                    {
                        current_decay_state++;
                        decay_timer = 0;
                    }
                    color = colors_of_dead_cell[current_decay_state];
                }
            }
            else
            {
                color = (Color){255, 255, 255, 255};
                current_decay_state = 0;
            }
        }
};

class Automata
{
    public:
        virtual ~Automata() = default;
        virtual void update_field(Cell (&field)[150][200]) = 0;
};

class GameOfLife : public Automata
{
    public:
        void update_field(Cell (&field)[150][200]) override
        {
            Cell new_field[150][200];

            for (int i = 0; i < 150; i++)
            {
                for (int j = 0; j < 200; j++)
                {
                    int counter = 0;
                    for (int k = -1; k < 2; k++)
                        for (int p = -1; p < 2; p++)
                            if (i + k >= 0 && i + k < 150 && j + p >= 0 && j + p < 200 && !(k == 0 && p == 0) && !field[i + k][j + p].is_dead()) counter++;

                    if (!field[i][j].is_dead() && (counter == 2 || counter == 3)) new_field[i][j].set_state(false);
                    else if (field[i][j].is_dead() && counter == 3) new_field[i][j].set_state(false);
                    else new_field[i][j].set_state(true);
                }
            }

            for (int i = 0; i < 150; i++)
                for (int j = 0; j < 200; j++)
                {
                    field[i][j].set_state(new_field[i][j].is_dead());
                    field[i][j].update_color();
                }
        }
};


bool ColorEquals(Color c1, Color c2)
{
    return c1.a == c2.a && c1.b == c2.b && c1.g == c2.g && c1.r == c2.r;
}


int main()
{
    int pen_size = 1, cursor_pos_x = 0, cursor_pos_y = 0;
    bool paused = true;
    Cell Field[150][200];
    Automata* current_automata = new GameOfLife();
    Vector2 mousePosition = Vector2Zero();

    for (int i = 0; i < 150; i++)
        for (int j = 0; j < 200; j++) Field[i][j] = Cell(true);

    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Cellular automaton");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        pen_size += GetMouseWheelMove();
        if (pen_size < 1) pen_size = 1;
        if (pen_size > 50) pen_size = 50;

        if (!paused) current_automata->update_field(Field);
        mousePosition = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            int posX = (int)mousePosition.x / 4;
            int posY = (int)mousePosition.y / 4;

            for (int i = posY; i < std::min(posY + pen_size, 150); i++)
                for (int j = posX; j < std::min(posX + pen_size, 200); j++) Field[i][j] = Cell(false);
        }

        if (IsKeyPressed(KEY_ENTER))
        {
            paused = !paused;
        }

        BeginDrawing();

            ClearBackground(BLACK);

            for (int i = 0; i < 150; i++)
                for (int j = 0; j < 200; j++)
                    if (!ColorEquals(Field[i][j].get_color(), BLACK))
                    {
                        DrawRectangle(j * 4, i * 4, 3, 3, Field[i][j].get_color());
                    }

            DrawRectangleLines((int)mousePosition.x, (int)mousePosition.y, pen_size * 4, pen_size * 4, (Color){130, 130, 130, 255});

        EndDrawing();
    }

    delete current_automata;

    return 0;
}