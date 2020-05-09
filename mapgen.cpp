#include <random>
#include <vector>
#include <iostream>
#include <ncurses.h>
using namespace std;

bool notinside(int x, int y, int a, int b, int c, int d) {
  if ((x >= a && x <= c) && (y >= b && y <= d)) return false;  
  return true;
}

void addroom () {
  bool overlaps = false;
  
  int startx, starty, endx, endy;
  int hallx, hally;
  
  do {
    overlaps = false;
    
    startx = rand() % (LINES-1);
    starty = rand() % (COLS -1);
    
    endx = startx + ((rand() % 8) + 5);
    endy = starty + ((rand() % 8) + 10);
  
    for (int x = startx-1; x < endx+1; x++)
      for (int y = starty-1; y < endy+1; y++){
        if ((mvinch(x, y) == ' ') || (x < 0) || (y < 0) || (x > LINES-1) || (y > COLS-1))  overlaps = true;
      }
  } while (overlaps == true);
  
  for (int x = startx; x < endx; x++)
    for (int y = starty; y < endy; y++) {
      mvprintw(x,y,"%c",' ');
    }
    
  hallx = endx - startx;
  hally = endy - starty;
  
  mvprintw(startx,starty, "%c", 'C');
  mvprintw(startx,endy-1, "%c", 'C');
  mvprintw(endx-1,starty, "%c", 'C');
  mvprintw(endx-1,endy-1, "%c", 'C');
  
  overlaps = false;
  
  // hallway
  if (((((endx-startx)/2)+startx) < (LINES/2)) && ((((endy-starty)/2) + starty) < (COLS/2))) {
    hally = (((endy-starty)/2) + starty);
    for (hallx = (((endx-startx)/2)+startx); hallx < (LINES/2); hallx++) {
      if (mvinch(hallx, hally) == ' ' && notinside(hallx, hally, startx, starty, endx, endy)) goto endofhall;
      else mvprintw(hallx, hally, "%c", ' ');
    }
    for (;hally < (COLS/2); hally++) {
      if (mvinch(hallx, hally) == ' ' && notinside(hallx, hally, startx, starty, endx, endy)) goto endofhall;
      else mvprintw(hallx, hally, "%c", ' ');
    }
  }
  if (((((endx-startx)/2)+startx) > (LINES/2)) && ((((endy-starty)/2) + starty) < (COLS/2))) {
    hally = (((endy-starty)/2) + starty);
    for (hallx = (((endx-startx)/2)+startx); hallx > (LINES/2); hallx--) {
      if (mvinch(hallx, hally) == ' ' && notinside(hallx, hally, startx, starty, endx, endy)) goto endofhall;
      else mvprintw(hallx, hally, "%c", ' ');
    }
    for (;hally < (COLS/2); hally++) {
      if (mvinch(hallx, hally) == ' ' && notinside(hallx, hally, startx, starty, endx, endy)) goto endofhall;
      else mvprintw(hallx, hally, "%c", ' ');
    }
  }
  if (((((endx-startx)/2)+startx) < (LINES/2)) && ((((endy-starty)/2) + starty) > (COLS/2))) {
    hally = (((endy-starty)/2) + starty);
    for (hallx = (((endx-startx)/2)+startx); hallx < (LINES/2); hallx++) {
      if (mvinch(hallx, hally) == ' ' && notinside(hallx, hally, startx, starty, endx, endy)) goto endofhall;
      else mvprintw(hallx, hally, "%c", ' ');
    }
    for (;hally > (COLS/2); hally--) {
      if (mvinch(hallx, hally) == ' ' && notinside(hallx, hally, startx, starty, endx, endy)) goto endofhall;
      else mvprintw(hallx, hally, "%c", ' ');
    }
  }
  if (((((endx-startx)/2)+startx) > (LINES/2)) && ((((endy-starty)/2) + starty) > (COLS/2))) {
    hally = (((endy-starty)/2) + starty);
    for (hallx = (((endx-startx)/2)+startx); hallx > (LINES/2); hallx--) {
      if (mvinch(hallx, hally) == ' ' && notinside(hallx, hally, startx, starty, endx, endy)) goto endofhall;
      else mvprintw(hallx, hally, "%c", ' ');
    }
    for (;hally > (COLS/2); hally--) {
      if (mvinch(hallx, hally) == ' ' && notinside(hallx, hally, startx, starty, endx, endy)) goto endofhall;
      else mvprintw(hallx, hally, "%c", ' ');
    }
  }
  
  endofhall:;
  
}

int main() {
  srand(time(NULL));
  initscr();
  noecho();
  halfdelay(1);
  clear();
  refresh();
  
  start_color();
  init_pair(3, COLOR_BLACK , COLOR_WHITE  );
  
  //int currentmap[LINES-1][COLS-1];
  for (int x=0; x<LINES-1; x++)
    for (int y=0; y<COLS-1; y++) {
      attron(COLOR_PAIR(3));
      mvprintw(x,y,"%c",'x');
      attroff(COLOR_PAIR(3));
    }
    
  int ch;
  
  do {
    switch (ch) {
      case 'b': addroom(); break;
    }
    
    mvprintw(LINES-1,COLS-1," ");
    refresh();
    
    ch = getch();
  } while (ch != 'q');
  
  endwin();
  return 0;
}