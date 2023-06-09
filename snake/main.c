#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "../lib/game.h"
#include "../lib/io.h"
#include "../lib/term.h"
#include "../lib/util.h"

// TODO: snake tail
// TODO: snake tail collision
// TODO: moving through apple with vi keys should consume the apple

#define WIDTH 32
#define HEIGHT 16
#define APPLE_CHAR 'A'

const int MAX_APPLE = 5;
enum move {
  UP = 0,
  DOWN = 1,
  LEFT = 2,
  RIGHT = 3,
  START = 4,
  END = 5,
  TOP = 6,
  BOTTOM = 7,
};

char KEYBINDS[9][20] = {"w,k - up",
                        "s,j - down",
                        "a,h - left",
                        "d,l - right",
                        "0 - start of row",
                        "$ - end of row",
                        "g - start of column",
                        "G - end of column",
                        "q - quit"};

int SCORE = 0;
int ALIVE = 1;
int SPEED = 1;
int APPLE = 0;
int POSX = 1;
int POSY = 1;

void generate_apple(char **field) {
  if (APPLE >= MAX_APPLE)
    return;
  int x;
  int y;
  do {
    x = rand() % WIDTH - 2;
    y = rand() % HEIGHT - 3;
  } while (x < 1 || y < 1);
  field[x][y] = APPLE_CHAR;
  APPLE++;
}

void fill_playfield(char **field) {
  game_create_border(field, '|', '-', '+', WIDTH, HEIGHT);
  field[1][1] = '@';
}

void render_playfield(char **field) {
  term_clear();
  printf("Keybinds:\n");
  for (size_t i = 0; i < ARRAY_SIZE(KEYBINDS); i++) {
    printf("\t%s\n", KEYBINDS[i]);
  }
  printf("SCORE: %d\n", SCORE);
  game_render(field, WIDTH, HEIGHT);
}

void move(char **field, int direction) {
  field[POSX][POSY] = ' ';

  switch (direction) {
  case START:
    POSX = 1;
    break;
  case END:
    POSX = WIDTH - 2;
    break;
  case TOP:
    POSY = 1;
    break;
  case BOTTOM:
    POSY = HEIGHT - 2;
    break;
  case UP:
    POSY -= SPEED;
    if (POSY < 1)
      ALIVE = 0;
    break;
  case DOWN:
    POSY += SPEED;
    if (POSY >= HEIGHT - 2)
      ALIVE = 0;
    break;
  case LEFT:
    POSX -= SPEED;
    if (POSX < 1)
      ALIVE = 0;
    break;
  case RIGHT:
    POSX += SPEED;
    if (POSX >= WIDTH - 2)
      ALIVE = 0;
    break;
  }

  if (field[POSX][POSY] == APPLE_CHAR) {
    APPLE--;
    SCORE++;
    generate_apple(field);
  }

  field[POSX][POSY] = '@';
}

int handle_input(char **field, char *in) {
  switch (*in) {
  case 'w':
  case 'k':
    move(field, UP);
    break;
  case 's':
  case 'j':
    move(field, DOWN);
    break;
  case 'a':
  case 'h':
    move(field, LEFT);
    break;
  case 'd':
  case 'l':
    move(field, RIGHT);
    break;
  case '0':
    move(field, START);
    break;
  case '$':
    move(field, END);
    break;
  case 'g':
    move(field, TOP);
    break;
  case 'G':
    move(field, BOTTOM);
    break;
  case 'q':
    return -1;
  }
  return 0;
}

int main(void) {
  term_enable_raw_mode();
  term_disable_buffering(stdout);
  srand(time(NULL));

  char **field = game_alloc(WIDTH, HEIGHT);
  fill_playfield(field);
  generate_apple(field);
  render_playfield(field);

  char c;

  while (c = io_read_char(), c != -1) {
    int r = handle_input(field, &c);
    if (!ALIVE) {
      term_clear();
      puts("game over, press any key");
      fflush(stdout);
      SCORE = 0;
      ALIVE = 1;
      SPEED = 1;
      APPLE = 0;
      POSX = 1;
      POSY = 1;
      fill_playfield(field);
      generate_apple(field);
      continue;
    }

    if (r != 0) {
      term_clear();
      puts("exit...");
      break;
    }

    c = 0;
    render_playfield(field);
  }

  game_destroy(field, WIDTH);
  return EXIT_SUCCESS;
}
