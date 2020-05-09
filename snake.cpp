#include <vector>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <list>

using namespace std;

class apple {
  public:
  int x, y;
  bool alive = true;
  
  apple() {
    x = rand() % (LINES-1);
    y = rand() % (COLS -1);
  }
  
  void render() {
    mvprintw(x,y, "%c", 'A');
  }
};

class snake {
  public:
  int x, y;
  int eaten = 0;
  int direction = 'd';
  
  snake() {
    x = 30;
    y = 30;
  }
  
  void change(int newdirection) {
    direction = newdirection;
  }
  
  void move() {
    switch (direction) {
      case 'w': x--; break;
      case 'a': y--; break;
      case 's': x++; break;
      case 'd': y++; break;
    }
    
    if (x < 1) x = LINES-1;
    if (y < 1) y = COLS-1;
    if (x > LINES-1) x = 1;
    if (y > COLS-1) y = 1;
  }
  
  void eat(list<apple>& localapples) {
    for (auto& item : localapples) {
      if (x == item.x && y == item.y && item.alive) {
        item.alive = false;
        eaten++;
      }
    }
  }

  void render() {
    mvprintw(x, y, "%c", 's');
  }  
  
};

int main() {
  initscr();
  noecho();
  halfdelay(1);
  
  srand(time(NULL));

  int cyclecount = 0;
  
  list<apple> apples;
    
  snake p1;
  
  char ch;
  
  do {
    cyclecount++;
    
    clear();
    
    switch (ch) {
      case 'w': p1.change('w'); break;
      case 'a': p1.change('a'); break;
      case 's': p1.change('s'); break;
      case 'd': p1.change('d'); break;
    }
    
    if (cyclecount % 10 == 0)
      apples.emplace_back();
    
    
    if (apples.size() > 0)
      for (auto& item : apples) {
        p1.eat(apples);
      }
      
    for (auto it = apples.begin(); it != apples.end();)
      if (it->alive) {
        ++it;
      } else {
        it = apples.erase(it);
      }  
      
    if (apples.size() > 0)
      for (auto& item : apples)
        item.render();
    
    

    p1.move();
    p1.render();
    
    mvprintw(0,0, "cycles: %d", cyclecount);
    mvprintw(1,0, "size: %d", p1.eaten);
    
    refresh();
    
    ch = getch();
  } while (ch != 'q');
    
    
  endwin();
  return 0;
}