#include <ncurses.h>
#include <vector>
#include <stdio.h> 
#include <stdlib.h>
#include <time.h> 

using namespace std;

class animal {
  public:
  int y, x, age, direction;
  char renderchar;
  bool alive = true;
  
  animal(int starty, int startx) {
    y = starty;
    x = startx;
    age = 0;
    direction = (rand() % 8) + 1;
  }
  
  void render() {
    mvprintw(y, x, "%c", renderchar);
  }  
};

class cow : public animal {
  public:
  cow(int starty, int startx) : animal(starty, startx) {
    renderchar = 'c';
  }
  
  void move() {
    switch (direction) {          
      case 1: y--;      break;    // 8 1 2
      case 2: y--; x++; break;    // 7 C 3
      case 3:      x++; break;    // 6 5 4
      case 4: y++; x++; break;
      case 5: y++;      break;
      case 6: y++; x--; break;
      case 7:      x--; break;
      case 8: y--; x--; break;
    }
    
    if (x < 31) {
      x++; direction = 3;
    }
    if (x > COLS-1) {
      x--; direction = 7;
    }
    if (y < 0) {
      y++; direction = 5;
    }
    if (y > LINES-1) {
      y--; direction = 1;
    }
    
    direction += ((rand() % 3) - 1);
    if (direction == 9) direction = 1;
    if (direction == 0) direction = 8;
  }
};

void debug(int localcyclecount, vector<cow>& localcowvec) {
  for (int x=0; x< LINES; x++)
    mvprintw(x, 30, "|");
  
  mvprintw(0,0,"Cycles: %d", localcyclecount);
  mvprintw(1,0,"Cows:  %5d", localcowvec.size());
  
  mvprintw(0, 29, "");
}

int main() {
  initscr();
  noecho();
  halfdelay(1);
  srand(time(NULL));
  
  int ch;
  int cyclecount = 0;
  
  int startingcows = 1000;
  
  vector<cow> cowvec;
  
  for (int x = 0; x < startingcows; x++)
    cowvec.emplace_back((rand() % LINES-1), ((rand() % (COLS - 30)) + 30));
  
  do {
    cyclecount++;
    clear();
  
    for (auto& it : cowvec) {
      it.move();
      it.render();
    }
    
    debug(cyclecount, cowvec);
    refresh(); ch = getch();
  } while(ch != 'q');
  
  endwin();
  return 0;
}
