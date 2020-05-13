#include <ncurses.h>

using namespace std;




int main() {
  initscr();
  
  
  WINDOW* debug      = newwin(40, 30, 0,0);
  WINDOW* playscreen = newwin(40,80,0,30);
  
  int ch;
  
  do {
    
    
    box(playscreen,0,0);
    box(debug,0,0);
    mvwprintw(playscreen,1,1,"Hello World!");
    
    refresh();
    wrefresh(playscreen);
    wrefresh(debug);
    ch = getch();
    
  } while (ch != 'q');
  
  endwin();
  return 0;
}