#include <iostream>
#include <fstream>

using namespace std;

int main() {
  ifstream entry("test.txt");
    
  if (entry.good()) {
    string sLine1, sLine2;
    getline(entry, sLine1); // sLine1 holds first line of test.txt
    getline(entry, sLine2); // sLine1 holds second line of test.txt
    cout << sLine2 << endl;
  }
  entry.close();

  return 0;
}