#include <ncurses.h>
#include <vector>
#include <stdio.h> 
#include <stdlib.h>
#include <time.h> 

using namespace std;

class animal;
class cow;
class grass;

class player {
  public:
  int y, x;
  
  player() {
    y = 1; x = 25;
  }
  
  void move(int direction) {
    switch (direction) {
      case 'w': y--; break;
      case 'a': x--; break;
      case 's': y++; break;
      case 'd': x++; break;
    }
  }
  
  void render() {
    attron(COLOR_PAIR(1));
    mvprintw(y, x, "@");
    attroff(COLOR_PAIR(1));
  }
};

class animal {
  public:
  int y, x, age, direction, eaten;
  char renderchar;
  bool alive = true;
  bool male;
  
  animal(int starty, int startx) {
    y = starty;
    x = startx;
    age = 0;
    direction = (rand() % 8) + 1;
    male = (rand() % 2);
    eaten = 0;
  }

  void kill() {
    alive = false;
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
    age++;
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

  void eat(vector<grass>& localgrass) {
    for (auto& it : localgrass) {
      if ((x == it.x) && (y == it.y)) {
	eaten++;
	it.kill();
      }
    }
  }
  
  void render() {
    if (age > 25) renderchar = 'O';
    if (male) {
      attron(COLOR_PAIR(4));
      mvprintw(y, x, "%c", renderchar);
      attroff(COLOR_PAIR(4));
    } else {
      attron(COLOR_PAIR(3));
      mvprintw(y, x, "%c", renderchar);
      attroff(COLOR_PAIR(3));
    }
  }  
};

class grass {
  public:
  int x, y, age;
  bool alive = true;
  char renderchar = '.';
  
  grass(int starty, int startx) {
    y = starty;
    x = startx;
    age = 0;
  }
  
  void cycleupdate() {
    age++;
  }
  
  bool birth() {
    if (rand() % 70 == 1) return true;
    else return false;
  }
  
  void checkbounds() {
    if (x < 0) alive = false;
    if (y < 31) alive = false;
    if (x >= LINES) alive = false;
    if (y >= COLS) alive = false;
  } // end of - void checkbounds()
  
  void render() {
    if (age > 25) renderchar = ',';
    if (age > 50) renderchar = ';';
    attron(COLOR_PAIR(2));
    mvprintw(y, x, "%c", renderchar);
    attroff(COLOR_PAIR(2));
  }
};

void debug(int localcyclecount, vector<cow>& localcowvec) {
  for (int x=0; x< LINES; x++)
    mvprintw(x, 30, "|");
  
  mvprintw(0,0,"Cycles: %d", localcyclecount);
  mvprintw(1,0,"Cows:   %d", localcowvec.size());
  
  mvprintw(0, 29, "");
}

void grassspread(vector<grass>& localvec) {
  size_t old_size = localvec.size();
  for (int i = 0; i < old_size; i++) {
    if(localvec[i].birth()) {
      
      // generate possible new grass location
      int newy = localvec[i].y + ((rand() % 9) - 4);
      int newx = localvec[i].x + ((rand() % 9) - 4);
      
      // check for overlaps
      bool overlap = false;
      for (auto& it : localvec) {
        if (it.x == newx && it.y == newy)
          overlap = true;
      }
      
      if ((newy < 0) || (newx <= 30) || (newy >= LINES) || (newx >= COLS)) overlap = true;
      
      // emplace if no overlap
      if (!overlap) localvec.emplace_back(newy, newx);
    }
  }
} // end of - void birthfunc()

int main() {
  initscr();
  noecho();
  //nodelay(stdscr, true);
  halfdelay(1);
  start_color();
  init_pair(1, COLOR_RED,     COLOR_BLACK); // 1 - PLAYER - RED     / BLACK
  init_pair(2, COLOR_GREEN,   COLOR_BLACK); // 2 - GRASS  - GREEN   / BLACK
  init_pair(3, COLOR_MAGENTA, COLOR_BLACK); // 3 - F COW  - MAGENTA / BLACK
  init_pair(4, COLOR_CYAN,    COLOR_BLACK); // 4 - M COW  - CYAN    / BLACK
  
  srand(time(NULL));
  
  int ch;
  int cyclecount = 0;
  
  int startingcows = 100;
  int startinggrass = 300;
  
  vector<cow> cowvec;
  vector<grass> grassvec;
  
  player p1 = player();
  
  for (int x = 0; x < startingcows; x++) cowvec.emplace_back((rand() % LINES-1), ((rand() % (COLS - 30)) + 30));
  for (int x = 0; x < startinggrass; x++) grassvec.emplace_back((rand() % LINES-1), ((rand() % (COLS - 30)) + 30));
   
  do {
    cyclecount++;
    clear();
  
    // update all grass
    for (auto& it : grassvec) it.cycleupdate();
    grassspread(grassvec);
  
    //move all cows
    for (auto& it : cowvec) it.move();
    
    //render loops
    for (auto& it : grassvec) it.render();
    for (auto& it : cowvec) it.render();
    
    switch (ch) {
      case 'w': p1.move('w'); break;
      case 'a': p1.move('a'); break;
      case 's': p1.move('s'); break;
      case 'd': p1.move('d'); break;
    }
    
    p1.render();
    
    debug(cyclecount, cowvec);
    refresh(); ch = getch();
  } while(ch != 'q');
  
  endwin();
  return 0;
}
