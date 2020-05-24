#include <iostream>
#include <curses.h>
#include <vector>
#include <list>
#include <unistd.h>
#include <math.h>
#include <cmath>
#include <fstream>
#include <chrono>
using namespace std; //test from lenovo #5 test 15

class ty_grass;
class ty_cow;
class ty_carcass;
class ty_vulture;

list<ty_grass> grassobjects;
list<ty_cow> cowobjects;
list<ty_carcass> carcassobjects;
list<ty_vulture> vultureobjects;

int cyclecount = 0;
int grasskillcount = 0;
int grasswipe = 0;
int cowkillcount = 0;
int cowwipe = 0;
int MAXGRASS = 2500;
int MAXCOWS = 2500;
int MAXVULTURES = 200;
int TARGETFPS = 20;

auto now = chrono::system_clock::now();
auto last_ms = chrono::time_point_cast<chrono::milliseconds>(now);

class ty_grass { //################################################## CLASS GRASS
  public:
  int age;
  int x, y;
  bool alive = true;
  int maxage = 150;
  int worth = (rand() % 5) + 5;
  int growthreach = 6;
  int maxdensity = 6;
  
  ty_grass(int startx, int starty, int startage = 0) {
    x = startx;
    y = starty;
    if (startage == 0) age = 0;
    else age = rand() % startage;
  }
   
  void cycleupdate() {
    age++;

    if (x < 0) alive = false;
    if (y < 30) alive = false;
    if (x >= LINES) alive = false;
    if (y >= COLS) alive = false;
    
    if (age > maxage)
      if (rand() % 50 == 1)
        alive = false;

    if (grassobjects.size() < MAXGRASS) {
      if (rand() % 75 == 1) {
        int localgrass = 0;
        for (auto it = grassobjects.begin(); it != grassobjects.end(); it++)
          if (sqrt( pow((it->x - x), 2) + pow((it->y - y), 2) ) < growthreach)
            localgrass++;
        if (localgrass < maxdensity)
          for (int i=0; i<3; i++)
            grassobjects.emplace_back(x + (rand() % (growthreach * 2) - growthreach), y + (rand() % (growthreach * 2) - growthreach));
      }
    }
  }
  
  int getworth() {
    int fvalue = 0;
    if (age < maxage/3) fvalue = worth;
    if (age > maxage/3) fvalue = worth * 2;
    if (age > (maxage/3)*2) fvalue = worth * 3;
    return fvalue;
  }
  
  void renderupdate() { //################################################## GRASS RENDER
    attron(COLOR_PAIR(1));
    mvprintw(x, y, ".");
    if (age > maxage/3) mvprintw(x, y, ",");
    if (age > ((maxage/3) * 2)) mvprintw(x, y, ";");
    attroff(COLOR_PAIR(1));
  }
};

class ty_cow { //################################################## CLASS COW
  public:
  bool alive = true;
  bool target = false;
  bool ismale;
  int age = 0;
  int eaten = 0;
  int fullness = 50;
  int x, y;
  int targetx, targety;
  int children = 0;
  int direction = 2;
  int generation;
  int maxage = (rand() % 70) + 70;
  int vision = (rand() % 5) + 4;
  int adultage = 20;
  int movecooldown = 0;
  int fullnessthresh = 30;
  
  ty_cow(int startx, int starty, int gennn = 64) {
    x = startx;
    y = starty;
    generation = gennn + 1;
    ismale = rand() % 2;
  }
  
  void cycleupdate() {
    age++;
    fullness--;
        
    if (generation == 91) generation = 65;
    
    if (movecooldown <= 0) {
      if (target) { //################################################## COW MOVEMENT
        if (targetx < x) x--;
        if (targetx > x) x++;
        if (targety < y) y--;
        if (targety > y) y++;
        if (targetx == x && targety == y) {
          target = false;
        }
      } else {
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
          
        direction += ((rand() % 3) - 1);
        if (direction == 0)
          direction = 8;
        if (direction == 9)
          direction = 1;
        
        if (fullness <= fullnessthresh) { //################################################## COW TARGET SELECTION OF GRASS
          
          for (auto it = grassobjects.begin(); it != grassobjects.end(); it++) {
            if (sqrt(pow((it->x - x), 2) + pow((it->y - y), 2) ) < vision && target == false && (rand()%4 == 1)) {
              targetx = it->x;
              targety = it->y;
              target = true;
            }
          }
        } else {
          if (rand() % 4 == 1) {
            for (auto it = grassobjects.begin(); it != grassobjects.end(); it++) {
              if (sqrt(pow((it->x - x), 2) + pow((it->y - y), 2) ) < vision && target == false && (rand()%4 == 1)) {
                targetx = it->x;
                targety = it->y;
                target = true;
              }
            }
          }
        }
      }
      
      if (x < 0) {x++; direction = 5;}
      if (y < 30) {y++; direction = 3;}
      if (x >= LINES) {x--; direction = 1;}
      if (y >= COLS) {y--; direction = 7;}
    } else {
      movecooldown--;
    }
      
    //################################################## birthing logic  
    if (age > adultage && eaten > 5) {
      eaten = 0;
      if (children <= 300 && cowobjects.size() < MAXCOWS) {
        cowobjects.emplace_back(x + (rand() % 4 - 2), y + (rand() % 4 - 2), generation);
        children++;
      }
    }
      
    //################################################## eat logic  
    for (auto it = grassobjects.begin(); it != grassobjects.end(); it++)
      if (x == it->x && y == it->y) {
        it->alive = false;
        eaten++;
        fullness += it->getworth();
        if (age > adultage) movecooldown = 2;
        else movecooldown = 1;
        target = false;
      }
      
      
    //################################################## death logic
    if (age > maxage) {
      int chancetodie = age % maxage;
      if (rand() % maxage <= chancetodie) {
        alive = false;
      }
    }
    
    if (fullness <= 0) alive = false;
  }

  void renderupdate() { //################################################## COW RENDER
    char cowcow;
    if (age > adultage) cowcow = 'C';
    else cowcow = 'c';
    if (ismale) {
      attron(COLOR_PAIR(3));
      mvprintw(x, y, "%c", cowcow);
      attroff(COLOR_PAIR(3));
    } else {
      attron(COLOR_PAIR(4));
      mvprintw(x, y, "%c", cowcow);
      attroff(COLOR_PAIR(4));
    }
  }
};

class ty_carcass {
  public:
  int x, y;
  int lifespan = 25;
  bool alive = true;
  int spawngrassradius = 3;
  int spawngrasscount = (rand() % 10);
  ty_carcass(int startx, int starty) {
    x = startx;
    y = starty;
  }
  
  void cycleupdate() {
    lifespan--;
    if (lifespan <= 0) {
      alive = false;
      for (int i=0; i < spawngrasscount; i++)
        grassobjects.emplace_back(x + ((rand() % (spawngrassradius * 2)) - spawngrassradius), y + ((rand() % (spawngrassradius * 2)) - spawngrassradius));
    }
  }
  
  void renderupdate() { //################################################## GRASS RENDER
    attron(COLOR_PAIR(2));
    mvprintw(x, y, "X");
    attroff(COLOR_PAIR(2));
  }
};

class ty_vulture { //################################################## CLASS VULTURE
  public:
  bool alive = true;
  bool target = false;
  int age = 0;
  int x, y;
  int targetx, targety;
  int direction = 2;
  int fullness = 200;
  int vision = 20;
  int movecooldown;
  int maxage = (rand() % 100) + 200;
  int children = 0;
  int eaten = 0;
  int fullnessthresh = 200;
  
  ty_vulture(int startx, int starty) {
    x = startx;
    y = starty;
  }
  
  void cycleupdate() {
    age++;
    fullness--;
    
    if (movecooldown <= 0) {
      if (target) { //################################################## VULTURE MOVEMENT
        if (targetx < x) x--;
        if (targetx > x) x++;
        if (targety < y) y--;
        if (targety > y) y++;
        if (targetx == x && targety == y) {
          target = false;
        }
      } else {    
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
         
        if (rand() % 5 == 1) direction += ((rand() % 3) - 1);
      
        if (direction == 0) direction = 8;
        if (direction == 9) direction = 1;
        
        if (fullness <= fullnessthresh) { //################################################## VULTURE TARGET SELECTION
          
          for (auto it = carcassobjects.begin(); it != carcassobjects.end(); it++) {
            if (sqrt(pow((it->x - x), 2) + pow((it->y - y), 2) ) < vision && target == false && (rand()%2 == 1)) {
              targetx = it->x;
              targety = it->y;
              target = true;
            }
          }
        }      
      }
      
      for (auto it = carcassobjects.begin(); it != carcassobjects.end(); it++)
        if (x == it->x && y == it->y) {
          it->alive = false;
          fullness += 150;
          target = false;
          movecooldown = 5;
          eaten++;
          direction = rand() % 9;
        }
      
        if (x == targetx && y == targety) target = false;    
          
        if (x < 0) {x++; direction = 5;}
        if (y < 30) {y++; direction = 3;}
        if (x >= LINES) {x--; direction = 1;}
        if (y >= COLS) {y--; direction = 7;}
      } else {
        movecooldown--;
      }
    
    if (fullness <= 0) alive = false;
    
    if (eaten >= 3 && vultureobjects.size() < MAXVULTURES) {
      eaten = 0;
      fullness = 200;
      children++;
      vultureobjects.emplace_back(x +((rand() % 3) -1), y +((rand() % 3) -1));
    }
    
    if (age > maxage) {
      int chancetodie = age % maxage;
      if (rand() % maxage <= chancetodie) {
        alive = false;
      }
    }
    
    if (rand() % 10 == 1) grassobjects.emplace_back(x + ((rand() % 3) - 1), y + ((rand() % 3) - 1));
  }

  void renderupdate() { //################################################## COW RENDER
    attron(COLOR_PAIR(8));
    mvprintw(x, y, "%c", 'V');
    attroff(COLOR_PAIR(8));
  }
};

int getmalecount() {
  int malecount = 0;
  for (auto it = cowobjects.begin(); it != cowobjects.end(); it++)
    if (it->ismale) malecount++;
  return malecount;
}

int getfemalecount() {
  int femalecount = 0;
  for (auto it = cowobjects.begin(); it != cowobjects.end(); it++)
    if (!it->ismale) femalecount++;
  return femalecount;
}

void debug(int cyclefps) { //################################################## DEBUG FUNCTION
  mvprintw(0, 0, "%d cycles  - %dfps", cyclecount, cyclefps);
  mvprintw(1, 0, "%d grass", grassobjects.size());
  mvprintw(2, 0, "%d cow", cowobjects.size());
  mvprintw(3, 0, "%d carcass", carcassobjects.size());
  mvprintw(4, 0, "%d vultures", vultureobjects.size());
  
  mvprintw(6, 0, "%d g-wipe | %d c-wipe", grasswipe, cowwipe);
  
  int i = 0; int scowdebug = 13;
  for (auto it = cowobjects.begin(); it != cowobjects.end(); it++) {
    i++; if (i >= 50) break;
    mvprintw(scowdebug+i, 0, "c%d", i);
    mvprintw(scowdebug+i, 5, "f%d", it->fullness);
    if (it->fullness <= it->fullnessthresh) {
      attron(COLOR_PAIR(3));
      mvprintw(scowdebug+i, 5, "f%d", it->fullness);
      attroff(COLOR_PAIR(3));
    }
    if (it->fullness <= 9) {
      attron(COLOR_PAIR(2));
      mvprintw(scowdebug+i, 5, "f%d", it->fullness);
      attroff(COLOR_PAIR(2));
    }
    if (it->age >= it->maxage) {
      attron(COLOR_PAIR(2));
      mvprintw(scowdebug+i, 10, "a%d", it->age);
      attroff(COLOR_PAIR(2));
    } else mvprintw(scowdebug+i, 10, "a%d", it->age);
    mvprintw(scowdebug+i, 16, "v%d", it->vision);
    mvprintw(scowdebug+i, 20, "g%c", it->generation + 0);
  }
  
  for (int i=0; i<LINES; i++)
    mvprintw(i,29,"|");
}

void outputcsv() {
  ofstream fout;
  ifstream fin;
  fin.open("cycles.csv");
  fout.open("cycles.csv", ios::app);
  fout << grassobjects.size() << "," << cowobjects.size() << "," << vultureobjects.size() << "\n";
  fin.close();
  fout.close();
}

void checkcowwipe() {
  if (cowobjects.size() == 0) {
    for (int i=0; i<5; i++)
      cowobjects.emplace_back(rand()%LINES, (rand()%(COLS-30))+30);
    cowwipe++;
  }
}
void checkgrasswipe() {
  if (grassobjects.size() < 100) {
    for (int i=0; i<1000; i++)
      grassobjects.emplace_back(rand()%LINES, (rand()%(COLS-30))+30, 100);
    grasswipe++;
  }
}

int main() { //################################################## MAIN LOOP
  srand(time(NULL));
  initscr();
  start_color();
  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  init_pair(2, COLOR_RED, COLOR_BLACK);
  init_pair(3, COLOR_BLUE, COLOR_BLACK);
  init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(5, COLOR_BLACK, COLOR_RED);
  init_pair(6, COLOR_BLACK, COLOR_BLUE);
  init_pair(7, COLOR_BLACK, COLOR_MAGENTA);
  init_pair(8, COLOR_YELLOW, COLOR_BLACK);
  clear();
  
  //creation conditions
  for (int i=0; i<200; i++) grassobjects.emplace_back(rand()%LINES, (rand()%(COLS-30))+30, 100);
  for (int i=0; i<50 ; i++) cowobjects.emplace_back(rand()%LINES, (rand()%(COLS-30))+30);
  for (int i=0; i<15 ;  i++) vultureobjects.emplace_back(rand()%LINES, (rand()%(COLS-30))+30);
  
  auto now = chrono::system_clock::now();
  auto loop_start = chrono::time_point_cast<chrono::milliseconds>(now);
  now = chrono::system_clock::now();
  auto loop_end = chrono::time_point_cast<chrono::milliseconds>(now);
  auto loop_diff = loop_end - loop_start;  
  
  while (true) {
    cyclecount++;
    
    //get loop start time
    auto now = chrono::system_clock::now();
    auto loop_start = chrono::time_point_cast<chrono::milliseconds>(now);
    
    //logic loops
    for (auto it = grassobjects.begin(); it != grassobjects.end(); it++) it->cycleupdate();
    for (auto it = cowobjects.begin(); it != cowobjects.end(); it++) it->cycleupdate();
    for (auto it = carcassobjects.begin(); it != carcassobjects.end(); it++) it->cycleupdate();
    for (auto it = vultureobjects.begin(); it != vultureobjects.end(); it++) it->cycleupdate();
    
    //delete duplicate grass
    for (auto it = grassobjects.begin(); it != grassobjects.end(); it++)
      for (auto iy = it; iy != grassobjects.end();) {
        iy++;
        if (it->x == iy->x && it->y == iy->y)
          it = grassobjects.erase(it);
      }
      
    //kill conditionals
    for (auto it = grassobjects.begin(); it != grassobjects.end();)
      if (it->alive) {
        ++it;
      } else {
        it = grassobjects.erase(it);
        grasskillcount++;
      }
    for (auto it = cowobjects.begin(); it != cowobjects.end();) {
      if (it->alive) {
        ++it;
      } else {
        carcassobjects.emplace_back(it->x, it->y);
        it = cowobjects.erase(it);
        cowkillcount++;
      }
    }
    for (auto it = carcassobjects.begin(); it != carcassobjects.end();) {
      if (it->alive) {
        ++it;
      } else {
        it = carcassobjects.erase(it);
      }
    }
    for (auto it = vultureobjects.begin(); it != vultureobjects.end();) {
      if (it->alive) {
        ++it;
      } else {
        carcassobjects.emplace_back(it->x, it->y);
        it = vultureobjects.erase(it);
      }
    }

    //wipe conditionals
    checkgrasswipe();
    checkcowwipe();

    //draw loops
    clear();
    
    
    for (auto& it : grassobjects) it.renderupdate();
    for (auto& it : cowobjects) it.renderupdate();
    for (auto& it : carcassobjects) it.renderupdate();
    for (auto& it : vultureobjects) it.renderupdate();
    
    //for (auto it = cowobjects.begin(); it != cowobjects.end(); it++) it->renderupdate();
    //for (auto it = carcassobjects.begin(); it != carcassobjects.end(); it++) it->renderupdate();
    //for (auto it = vultureobjects.begin(); it != vultureobjects.end(); it++) it->renderupdate();

    now = chrono::system_clock::now();
    auto loop_end = chrono::time_point_cast<chrono::milliseconds>(now);
    auto loop_diff = loop_end - loop_start;
    if (((1000 / TARGETFPS) - (int)loop_diff.count()) >= 0) usleep(((1000 / TARGETFPS) - (int)loop_diff.count()) * 1000);
    
    now = chrono::system_clock::now();
    auto loop_end2 = chrono::time_point_cast<chrono::milliseconds>(now);    
    auto loop_diff2 = loop_end2 - loop_start;
    int actualfps = 1000 / (int)loop_diff2.count();

    debug(actualfps);
    outputcsv();
    
    mvprintw(LINES-1,COLS-1,"");
    refresh();
  }
  
  endwin();
  return 0;
}
