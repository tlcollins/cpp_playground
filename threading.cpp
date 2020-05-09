#include <ncurses.h>
#include <thread>

using namespace std;

class cow {
  public:
  
  int cowgroup = rand() % 3;
  int x = rand() % (COLS-1);
  int y = rand() % (LINES-1);
  int direction = rand() % 9;
  
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
  
  void render(int colorint = 0) {
    attron(COLOR_PAIR(colorint));
    mvprintw(x, y, "%c", 'c');
    attroff(COLOR_PAIR(colorint));
  } // end of - void render()  
    
  
  void operator()(int grouping) { 
    if (grouping == cowgroup) {
      
      move();
      render(grouping);
      
    }
  }
  
};

int main() {
  
  initscr();
  
  start_color();
  init_pair(0, COLOR_WHITE, COLOR_BLACK);
  init_pair(1, COLOR_GREEN, COLOR_BLACK);  
  
  
  thread th1(cow(), 0);
  do {
    
    
  } while (true);
  
  endwin();
  
  
  return 0;
  
}
  