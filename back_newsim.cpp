#include <iostream>
#include <curses.h>
#include <vector>
#include <unistd.h>
#include <math.h>
#include <cmath>
#include <fstream>
#include <chrono>

using namespace std; //test from lenovo #5 test 15

class ty_object {
  public:
  int x, y;
  int age = 0;
  char renderchar;
  bool alive = true;
  
  void render(int colorint = 0) {
    attron(COLOR_PAIR(colorint));
    mvprintw(x, y, "%c", renderchar);
    attroff(COLOR_PAIR(colorint));
  } // end of - void render()
}; // end of - class ty_object

class ty_grass : public ty_object {
  public:
  ty_grass (int startx, int starty) {
    x = startx;
    y = starty;
    renderchar = '.';
  } // end of - constructor()
  
  void cycleupdate() {
    age++;
    if (age > 50)
      if (rand() % 100 == 1)
        alive = false;
  } // end of - void cycleupdate()
  
  void checkbounds() {
    if (x < 0) alive = false;
    if (y < 30) alive = false;
    if (x >= LINES) alive = false;
    if (y >= COLS) alive = false;
  } // end of - void checkbounds()

  bool birth() {
    if (rand() % 100 == 1) return true;
    else return false;
  } // end of - bool birth()
}; // end of - class ty_grass

class ty_cow : public ty_object {
  public:
  int direction;
  
  ty_cow(int startx, int starty) {
    x = startx;
    y = starty;
    renderchar = 'c';
    direction = (rand() % 8) + 1;
  } // end of - constructor()
  
  void cycleupdate() {
    age++;
    move();
  } // end of - void cycleupdate()
  
  void move() {
    direction += ((rand() % 3) - 1);
    if (direction == 0) direction = 8;
    if (direction == 9) direction = 1;        
    switch(direction) {
      case 1: x--; break;
      case 2: x--; y++; break;
      case 3: y++; break;
      case 4: y++; x++; break;
      case 5: x++; break;
      case 6: x++; y--; break;
      case 7: y--; break;
      case 8: y--; x--; break;
    }
    if (x < 0) {x++; direction = 5;}
    if (y < 30) {y++; direction = 3;}
    if (x >= LINES) {x--; direction = 1;}
    if (y >= COLS) {y--; direction = 7;}
  } // end of - void move()
}; // end of - class ty_cow

void removenotalive(vector<auto>& localvec) {
  int i = 0;
  do {
    if (localvec[i].alive == false) {
      localvec.erase(localvec.begin() + i);
    } else {
      i++;
    }
  } while (i != localvec.size());
} // end of - void removenotalive()

void birthfunc(vector<auto>& localvec) {
  size_t old_size = localvec.size();
  for (int i = 0; i < old_size; i++) {
    if(localvec[i].birth()) {
      
      // generate possible new grass location
      int newx = localvec[i].x + ((rand() % 7) - 3);
      int newy = localvec[i].y + ((rand() % 7) - 3);
      
      // check for overlaps
      bool overlap = false;
      for (auto& it : localvec) {
        if (it.x == newx && it.y == newy)
          overlap = true;
          it = localvec.end();
      }
      
      // emplace if no overlap
      if (!overlap) localvec.emplace_back(newx, newy);
    }
  }
} // end of - void birthfunc()

void checkoverlap(vector<auto>& localvec) {
  for (int a = 0; a < localvec.size(); a++)
    for (int b = a + 1; b < localvec.size(); b++)
      if (localvec[a].x == localvec[b].x && localvec[a].y == localvec[b].y)
        localvec[a].alive = false;
} // end of - void birthfunc()

void debug(int a, int b, int c, vector<auto>& debugvec) {
  mvprintw(0,0,"%d", a);
  mvprintw(1,0,"%d grass", b);
  mvprintw(2,0,"%d cows", c);
  mvprintw(3,0,"%d lines %d cols", LINES, COLS - 30);
  
  
  for (int i=0; i<debugvec.size(); i++) mvprintw(i+10, 0, "c:%-2d a:%-3d", i, debugvec[i].age);
  
  for (int i=0; i<LINES; i++) mvprintw(i,29,"|");
}

int main(int argc, char** argv) {
  int cyclecount = 0;
  
  int startercows = 50;
  int startergrass = 500;
  
  //if (argv[1]) startercows = atoi(argv[1]); // works
  //if (argv[2]) startergrass = atoi(argv[2]); // doesn't work
  
  vector<ty_cow> cowvec;
  vector<ty_grass> grassvec;
  
  srand(time(NULL));
  
  //curses initialization
  initscr();
  
  start_color();
  init_pair(0, COLOR_WHITE, COLOR_BLACK);
  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  init_pair(2, COLOR_RED, COLOR_BLACK);
  init_pair(3, COLOR_BLUE, COLOR_BLACK);
  init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(5, COLOR_BLACK, COLOR_RED);
  init_pair(6, COLOR_BLACK, COLOR_BLUE);
  init_pair(7, COLOR_BLACK, COLOR_MAGENTA);
  init_pair(8, COLOR_YELLOW, COLOR_BLACK);
  
  clear();
  refresh();
 
  for (int i=0; i < startergrass; i++) grassvec.emplace_back((rand() % (LINES)), (rand() % (COLS - 30) + 30));
  for (int i=0; i < startercows; i++) cowvec.emplace_back((rand() % (LINES)), (rand() % (COLS - 30) + 30));
  
  while (true) { // main sim loop
    clear();
    cyclecount++;
    
    //logic loops
    for (auto& it : grassvec) it.cycleupdate();
    for (auto& it : cowvec) it.cycleupdate();
    
    //birth loops    
    birthfunc(grassvec);
    
    //check grass overlap
    for (auto& it : grassvec) it.checkbounds();
    
    //kill loops
    removenotalive(grassvec);
    
    //render loops
    for (auto& it : grassvec) it.render(1);
    for (auto& it : cowvec) it.render(3);
       
    debug(cyclecount, grassvec.size(), cowvec.size(), grassvec);
    usleep(50000);
    mvprintw(LINES-1,COLS-1,"");
    refresh();
  } // end of - main loop

  endwin();
  return 0;
} // end of - int main()