#include <ncurses.h>
#include <vector>
#include <stdio.h> 
#include <stdlib.h>
#include <time.h> 
#include <algorithm>
#include <unistd.h>
#include <cmath>
#include <string>
#include <random>

using namespace std;

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

class grass {
  public:
  int x, y, age, worth;
  bool alive = true;
  char renderchar = '.';
  int growthreach = 3;
  int maxdensity = 5;
  int maxage = 150;
  
  grass(int starty, int startx) {
    y = starty;
    x = startx;
    age = 0;
    worth = (rand() % 4) + 5;
  }
  
  void cycleupdate() {
    age++;
    if (age > maxage)
      if (rand() % 2 == 1)
        alive = false;
  }
  
  void spread(vector<grass>& grassvec, vector<grass>& newgrass, default_random_engine& generator) {
    uniform_int_distribution<int> distribution(1,25);
    int dice_roll = distribution(generator);
    
    //if (rand() % 24 == 1) {
    if (dice_roll == 1) {    
    
      int newy = y + ((rand() % (growthreach * 2)) - (growthreach - 1));
      int newx = x + ((rand() % (growthreach * 2)) - (growthreach - 1));
      
      bool overlap = false;
      for (auto& it : grassvec) {
        if ((it.x == newx && it.y == newy) || (newy < 0) || (newx <= 30) || (newy >= LINES) || (newx >= COLS)) {
          overlap = true; break;
        }
      }
      if (!overlap){
        for (auto& it : newgrass) {
          if (it.x == newx && it.y == newy) {
            overlap = true; break;
          }
        }
      }
      
      if (!overlap) {
        int localgrass = 0;
        for (auto& it : grassvec)
          if (sqrt( pow((it.x - newx), 2) + pow((it.y - newy), 2) ) <= growthreach) {
            localgrass++;
            if (localgrass >= maxdensity) break;
          }
        if (localgrass < maxdensity) {
          newgrass.emplace_back(newy, newx);
        }//*/
      }
    }
  }
  
  void kill() {
    alive = false;
  }
    
  void render() {
    if (age > (maxage / 3)) renderchar = ',';
    if (age > ((maxage / 3) * 2)) renderchar = ';';
    attron(COLOR_PAIR(2));
    mvprintw(y, x, "%c", renderchar);
    attroff(COLOR_PAIR(2));
  }
};

class animal {
  public:
  int y, x, age, direction, eaten, fullness, targety, targetx, vision, children, movecooldown, maxage, adultage;
  char renderchar;
  bool alive = true;
  bool male, target;
  
  animal(int starty, int startx) {
    y = starty;
    x = startx;
    age = 0;
    direction = (rand() % 8) + 1;
    male = (rand() % 2);
    eaten = 0;
    fullness = 50;
    children = 0;
    movecooldown = 0;
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
    vision = ((rand() % 5) + 2);
    target = false;
    maxage = (rand() % 70) + 70;
    adultage = 25;
  }
  
  void move(vector<grass>& localgrass) {
    age++; fullness--;
    
    if (fullness <= 0) alive = false;
    if (age > maxage) {
      int chancetodie = age % maxage;
      if (rand() % maxage <= chancetodie) {
        alive = false;
      }
    }
    
    if (movecooldown <= 0) {
      if (target) {
        if (targetx < x) x--;
        if (targetx > x) x++;
        if (targety < y) y--;
        if (targety > y) y++;
        if (targetx == x && targety == y) {
          target = false;
        }
      } else {
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
        
        if (x < 31)      { x++; direction = 3; }
        if (x > COLS-1)  { x--; direction = 7; }
        if (y < 0)       { y++; direction = 5; }
        if (y > LINES-1) { y--; direction = 1; }
        
        direction += ((rand() % 3) - 1);
        if (direction == 9) direction = 1;
        if (direction == 0) direction = 8;
        
        if (fullness < 20) {
          for (auto& it : localgrass) {
            if ( (sqrt(pow((it.x - x), 2) + pow((it.y - y), 2) ) < vision) && (target == false) && (rand() % 4 == 1) ) {
              target = true;
              targety = it.y; targetx = it.x;
            }
          }
        }
      }
    } else {
      movecooldown--;
    }
  }
  
  bool birth(cow& thisone, vector<cow>& cowvec) {
    if (!male) {
      if (eaten > 10 && age > adultage) {
        int localcows = 0;
        for (auto& it : cowvec) {
          if ( (sqrt(pow((thisone.x - it.x), 2) + pow((thisone.y - it.y), 2) ) < vision) ) {
            localcows++;
          }
        }
        eaten = 0;
        if (localcows < 3) {
          return true;
        }
      }
    }
    return false;
  }

  void eat(vector<grass>& grassvec) {
    if (fullness < 50) {
      for (auto& it : grassvec) {
        if ((x == it.x) && (y == it.y)) {
          eaten++;
          fullness += it.worth;
          it.kill();
          target = false;
          movecooldown = 2;
          direction = (rand() % 8) + 1;
        }
      }
    }
  }
  
  void render() {
    if (age > adultage) renderchar = 'O';
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

template <typename T>
void removeitems(vector<T>& v) {
  v.erase(remove_if(v.begin(), v.end(), [](auto&& o) {return !o.alive;}), v.end());
}

void debug(int localcyclecount, vector<cow>& localcowvec, vector<grass>& localgrassvec) {
  for (int x=0; x< LINES; x++)
    mvprintw(x, 30, "|");
  
  mvprintw(0,0,"Cycles: %d", localcyclecount);
  mvprintw(1,0,"Grass:  %d", localgrassvec.size());
  mvprintw(2,0,"Cows:   %d", localcowvec.size());
  
  for (int x = 0; x < localcowvec.size(); x++) {
    mvprintw(x+5,0,"c%-2d a%-4d f%-3d e%-3d %s", x, localcowvec[x].age, localcowvec[x].fullness, localcowvec[x].eaten, localcowvec[x].target ? "true" : "false");
  }
  
  mvprintw(0, 29, "");
}

int main() {
  initscr();
  noecho();
  nodelay(stdscr, true);
  
  start_color();
  init_pair(1, COLOR_RED,     COLOR_BLACK); // 1 - PLAYER - RED     / BLACK
  init_pair(2, COLOR_GREEN,   COLOR_BLACK); // 2 - GRASS  - GREEN   / BLACK
  init_pair(3, COLOR_MAGENTA, COLOR_BLACK); // 3 - F COW  - MAGENTA / BLACK
  init_pair(4, COLOR_CYAN,    COLOR_BLACK); // 4 - M COW  - CYAN    / BLACK
  
  srand(time(NULL));
  default_random_engine generator;
  
  int ch;
  int cyclecount = 0;
  
  int startingcows = 100;
  int startinggrass = 500;
  
  vector<cow> cowvec;
  vector<cow> newcows;
  vector<grass> grassvec;
  vector<grass> newgrass;
  //grassvec.reserve(15000);
  
  player p1 = player();
  
  for (int x = 0; x < startinggrass; x++) grassvec.emplace_back((rand() % LINES-1), ((rand() % (COLS - 30)) + 30));
   
  do {
    cyclecount++;
    clear();
    
    if (cyclecount == 250) {
      //halfdelay(1);
      for (int x = 0; x < startingcows; x++) cowvec.emplace_back((rand() % LINES-1), ((rand() % (COLS - 30)) + 30));
    }
  
    // update all grass
    for (auto& it : grassvec) it.cycleupdate();
    for (auto& it : grassvec) it.spread(grassvec, newgrass, generator);
    grassvec.insert(grassvec.end(), newgrass.begin(), newgrass.end());
    mvprintw(4,0,"NewGrass:%d", newgrass.size());
    newgrass.clear();
    //grassspread(grassvec);
  
    //move all cows
    for (auto& it : cowvec) it.move(grassvec);
    for (auto& it : cowvec) it.eat(grassvec);
    
    //cow birth 
    for (auto& it : cowvec)
      if (it.birth(it, cowvec)) {
        int howmany = (rand() % 3) + 2;
        for (int x = 0; x < howmany; x++)
          newcows.emplace_back(it.y, it.x);
      }
    cowvec.insert(cowvec.end(), newcows.begin(), newcows.end());
    mvprintw(3,0,"NewCows: %d", newcows.size());
    newcows.clear();
    
    //remove !alive items from respective vectors
    removeitems(grassvec);
    removeitems(cowvec);
    
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
    
    debug(cyclecount, cowvec, grassvec);
    refresh(); ch = getch();
  } while(ch != 'q');
  
  endwin();
  return 0;
}
