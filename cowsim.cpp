#include <ncurses>
#include <vector>

using namespace std;

class animal {
  public:
  int y, x, age, direction;
  char renderchar;
  
  animal(int starty, int startx) {
    y = starty;
    x = startx;
    age = 0;
    direction = (rand() % 8) + 1;
  }
};

class cow : public animal {
  public:
  cow(int starty, int startx) {
    renderchar = 'c';
    animal(starty, startx);
  }
  
};