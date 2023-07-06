#include <vitasdk.h>

#include "core.h"
#include "playfield.h"
#include "input.h"
#include "scoring.h"
#include "../lib/timer.h"
#include "../lib/shuffle7.h"
#include "../graphics/core.h"
#include "../graphics/playfield.h"

void animate_line_kill(uint8_t Y)
{
   3+4;
   return;
}


void animate_game_over()
{
   10+9;
   return;
}

/*  TO-DO: move rotation system to separate files
    Wallkick values for the Super Rotation System
    https://tetris.fandom.com/wiki/SRS#Basic_Rotation 

    NOTE: the sign of the Y value is opposite to what the Tetris wiki uses,
    i.e. ΔY > 0 is downward in this implementation, whereas it means upward in the wiki
    so the Y-values of the wallkicks below are negated relative to what the wiki shows.
*/
static const int8_t WALLKICKS_JLTSZ[4][4][2] = { {{-1, 0}, {-1,-1}, {0, 2}, {-1, 2}},
                                                 {{ 1, 0}, { 1, 1}, {0,-2}, { 1,-2}},
                                                 {{ 1, 0}, { 1,-1}, {0, 2}, { 1, 2}},
                                                 {{-1, 0}, {-1, 1}, {0,-2}, {-1,-2}} };

static const int8_t WALLKICKS_I[4][4][2] = { {{-2, 0}, { 1, 0}, {-2, 1}, { 1,-2}},
                                             {{-1, 0}, { 2, 0}, {-1,-2}, { 2, 1}},
                                             {{ 2, 0}, {-1, 0}, { 2,-1}, {-1, 2}},
                                             {{ 1, 0}, {-2, 0}, { 1, 2}, {-2,-1}} };

static tetromino_t tetromino; // TO-DO: rename this to active_tetromino
static uint8_t X, Y;
static int8_t Y_hard_drop = -1;
static tetromino_type_t held_tetromino = TETROMINO_TYPE_NULL;
static bool tetromino_swapped = false;
static engine_state_t engine_state = ENGINE_STATE_UNINITIALIZED;

static uint32_t gravity_delay = ENGINE_GRAVITY_INITIAL_DELAY_MICROSECONDS;
static vita_timestamp_t drop_lock_timer = {0};
static vita_timestamp_t gravity_timer;

/*
    TO-DO: Update timer code to not assign by reference
*/


static void engine_check_drop_lock();
static void engine_update_gravity();


static inline void engine_input_callback_right() {engine_move_active_tetromino(1,0);}
static inline void engine_input_callback_left() {engine_move_active_tetromino(-1,0);}
static inline void engine_input_callback_start() {engine_state = ENGINE_STATE_LOSE;}


void engine_init()
{ //{{{
    input_init();

    input_set_callback_up(engine_hard_drop_tetromino);
    input_set_callback_right(engine_input_callback_right);
    input_set_callback_down(engine_soft_drop_tetromino);
    input_set_callback_left(engine_input_callback_left);

    input_set_callback_triangle(engine_swap_held_tetromino_with_active);
    input_set_callback_circle(engine_swap_held_tetromino_with_active);
    input_set_callback_cross(engine_rotate_active_tetromino_clockwise);
    input_set_callback_square(engine_rotate_active_tetromino_counterclockwise);

    input_set_callback_start(engine_input_callback_start);

    bag_of_7_init(0); // TO-DO: Get time for random seed
    engine_spawn_tetromino(engine_pop_queued_tetromino());
    timer_set_current_time(&gravity_timer);
    engine_state = ENGINE_STATE_RUNNING;
/*}}}*/ }


void engine_game_loop(void)
{ //{{{
    uint32_t elapsed_us=0;
    int32_t remaining_us=0;
    uint32_t loop_timer;
    vita_timestamp_t start_time, end_time;

    while(engine_state == ENGINE_STATE_RUNNING) {
        // elapsed_us=0;
        // remaining_us=0;
        // timer_set_current_time(&start_time);

        input_read_and_run_callbacks();
        engine_update_gravity();
        engine_check_drop_lock();
        graphics_draw_game();

        // timer_set_current_time(&end_time);
        // elapsed_us = timer_get_elapsed_microseconds(&start_time, &end_time);
        // remaining_us = ENGINE_MICROSECONDS_PER_FRAME - elapsed_us;
        // if (remaining_us > 0) usleep(remaining_us);

    }

    switch(engine_state) {
        default:
        case ENGINE_STATE_LOSE:
            graphics_animate_game_over();
            return;
        case ENGINE_STATE_WIN:
            break;
    }

/*}}}*/ }


static void engine_check_drop_lock()
{ //{{{
    if (!timer_is_null(&drop_lock_timer)) {
        vita_timestamp_t now_time;
        timer_set_current_time(&now_time);
        uint32_t elapsed_us = timer_get_elapsed_microseconds(&drop_lock_timer, &now_time);
        if (elapsed_us >= ENGINE_DROP_LOCK_DELAY_MICROSECONDS) {
            timer_unset(&drop_lock_timer);
            if (!playfield_validate_tetromino_placement(&tetromino, X, Y+1)) {
                engine_place_tetromino_at_xy(X,Y);   // Lock the piece if it cannot proceed downward
                graphics_draw_game();
            }
        }
    }
/*}}}*/ }


static void engine_update_gravity()
{ //{{{
    vita_timestamp_t now_time; 
    timer_set_current_time(&now_time);

    uint32_t elapsed_us = timer_get_elapsed_microseconds(&gravity_timer, &now_time);
    if (elapsed_us >= gravity_delay) {
        if (!engine_move_active_tetromino(0,1) && timer_is_null(&drop_lock_timer)) {
            drop_lock_timer = now_time;
        }
        gravity_timer = now_time;
    }
/*}}}*/ }


tetromino_type_t engine_pop_queued_tetromino()
{ //{{{
    const tetromino_type_t type = (tetromino_type_t)(bag_of_7_pop_sample()+1);
    graphics_draw_queue_preview();
    return type;
/*}}}*/}


void engine_spawn_tetromino(tetromino_type_t type)
{ //{{{
    tetromino = (tetromino_t){ type, 0 };
    X = PLAYFIELD_SPAWN_X;
    Y = PLAYFIELD_SPAWN_Y;
    if (!playfield_validate_tetromino_placement(&tetromino, X, Y)) {
        --Y;
        if (!playfield_validate_tetromino_placement(&tetromino, X, Y)) {
            engine_state = ENGINE_STATE_LOSE;  // Game is over if there's no room for a new piece.
        }
    }
/*}}}*/}


void engine_end()
{ //{{{
    return;
/*}}}*/ }


const tetromino_t* engine_get_active_tetromino()
{ return &tetromino; }


const tetromino_type_t engine_get_held_tetromino()
{ return held_tetromino; }


/*const point_t engine_get_active_xy()
{ return (const point_t){X, Y}; }*/


const int8_t engine_update_hard_drop_y()
{ //{{{
    Y_hard_drop = -1;
    int8_t _Y = Y;
    bool previous_valid = false;
    while(_Y < PLAYFIELD_HEIGHT1+2) {
        bool current_valid = playfield_validate_tetromino_placement(&tetromino, X, _Y);
        if (previous_valid && (!current_valid)) {
            if(_Y > Y) {
                Y_hard_drop = _Y-1;
                break;
            }
        }
        previous_valid = current_valid;
        ++_Y;
    }
    return Y_hard_drop;
/*}}}*/ }


bool engine_move_active_tetromino(int8_t dx, uint8_t dy)
{ //{{{
    uint8_t _X=X+dx, _Y=Y+dy;
    if (playfield_validate_tetromino_placement(&tetromino, _X, _Y)) {
        X=_X;
        Y=_Y;
        return true;
    }
    return false;
/*}}}*/ }


void engine_hard_drop_tetromino()
{ //{{{
    if (Y_hard_drop > -1) {
        uint8_t drop_height = Y_hard_drop - Y;
        scoring_add_hard_drop(drop_height);
        engine_place_tetromino_at_xy(X, Y_hard_drop);
    }
/*}}}*/ }


void engine_soft_drop_tetromino()
{ //{{{
    if (!engine_move_active_tetromino(0,1) && timer_is_null(&drop_lock_timer)) {
        timer_set_current_time(&drop_lock_timer);
    }
    timer_set_current_time(&gravity_timer); // Reset gravity timer to prevent double-down
    scoring_add_soft_drop();
/*}}}*/ }


void engine_swap_held_tetromino_with_active(void)
{ //{{{
    if (!tetromino_swapped) {
        tetromino_swapped = true;
        tetromino_type_t current = tetromino.type;
        if (held_tetromino == TETROMINO_TYPE_NULL) {
            held_tetromino = engine_pop_queued_tetromino();
            graphics_draw_queue_preview();
        }
        tetromino.type = held_tetromino;
        tetromino.rotation = 0;
        held_tetromino = current;
        graphics_draw_held_tetromino();
        engine_spawn_tetromino(tetromino.type);
    }
/*}}}*/ }


void engine_place_tetromino_at_xy(uint8_t x, uint8_t y)
{ //{{{
    playfield_place_tetromino(&tetromino, x, y);
    graphics_playfield_convert_grid_to_block_vertices();
    uint8_t lines = playfield_clear_lines(graphics_animate_line_kill);
    uint8_t new_level = scoring_add_line_clears(lines);
    if (new_level) {
        gravity_delay = (ENGINE_GRAVITY_INITIAL_DELAY_MICROSECONDS 
                       - ((uint32_t)ENGINE_GRAVITY_INITIAL_DELAY_MICROSECONDS * new_level
                                                                       / SCORING_MAX_LEVEL));
        if (new_level >= SCORING_MAX_LEVEL) engine_state = ENGINE_STATE_WIN;
    }
    engine_spawn_tetromino(engine_pop_queued_tetromino());
    tetromino_swapped = false;  // Reset swappability 
/*}}}*/}


void engine_rotate_active_tetromino_clockwise()  // Rotation with wallkicks
{ //{{{
    tetromino_rotate_clockwise(&tetromino);

    if (playfield_validate_tetromino_placement(&tetromino, X, Y)) goto valid_exit;

    uint8_t x, y;
    const int8_t (*wallkicks)[2];

    switch(tetromino.type) {
        case TETROMINO_TYPE_O:  // O-piece cannot rotate, nothing to be done
            goto valid_exit;
        case TETROMINO_TYPE_I:
            wallkicks = WALLKICKS_I[tetromino.rotation]; break;
        default:
            wallkicks = WALLKICKS_JLTSZ[tetromino.rotation]; break;
    }

    for (uint8_t i = 0; i < 4; ++i) {
        x = X + wallkicks[i][0];
        y = Y + wallkicks[i][1];
        if (playfield_validate_tetromino_placement(&tetromino, x, y)) {
            X = x;
            Y = y;
            goto valid_exit;
        }
    }
    goto invalid_exit;

    valid_exit:
        timer_unset(&drop_lock_timer);  // valid rotations restart drop-lock timer
        return;

    invalid_exit:
        tetromino_rotate_counterclockwise(&tetromino); // undo rotation if no kicks are valid
        return;
/*}}}*/ }


void engine_rotate_active_tetromino_counterclockwise()  // Rotation with wallkicks
{ //{{{
    tetromino_rotate_counterclockwise(&tetromino);

    if (playfield_validate_tetromino_placement(&tetromino, X, Y)) goto valid_exit;

    uint8_t x, y;
    const int8_t (*wallkicks)[2];

    switch(tetromino.type) {
        case TETROMINO_TYPE_O:  // O-piece cannot rotate, nothing to be done
            goto valid_exit;
        case TETROMINO_TYPE_I:
            wallkicks = WALLKICKS_I[tetromino.rotation]; break;
        default:
            wallkicks = WALLKICKS_JLTSZ[tetromino.rotation]; break;
    }

    for (uint8_t i = 0; i < 4; ++i) {
        x = X - wallkicks[i][0];
        y = Y - wallkicks[i][1];
        if (playfield_validate_tetromino_placement(&tetromino, x, y)) {
            X = x;
            Y = y;
            goto valid_exit;
        }
    }
    goto invalid_exit;

valid_exit:
    timer_unset(&drop_lock_timer);  // valid rotations restart drop-lock timer
    return;

invalid_exit:
    tetromino_rotate_clockwise(&tetromino); // undo rotation if no kicks are valid
    return;
/*}}}*/ }