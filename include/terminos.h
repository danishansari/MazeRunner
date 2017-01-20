#include <termios.h>
#include <stdio.h>

static struct termios old, new1;

/* Initialize new1 terminal i/o settings */
void initTermios(int echo) 
{
  tcgetattr(0, &old); /* grab old terminal i/o settings */
  new1 = old; /* make new1 settings same as old settings */
  new1.c_lflag &= ~ICANON; /* disable buffered i/o */
  new1.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
  tcsetattr(0, TCSANOW, &new1); /* use these new1 terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void) 
{
  tcsetattr(0, TCSANOW, &old);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo) 
{
  char ch;
  initTermios(echo);
  ch = getchar();
  fflush(stdin);
  resetTermios();
  return ch;
}

/* Read 1 character without echo */
char getch(void) 
{
  return getch_(0);
}

/* Read 1 character with echo */
char getche(void) 
{
  return getch_(1);
}

/* Let's test it out */
//int main(void) {
//  char c;
//  printf("(getche example) please type a letter: ");
//  c = getche();
//  printf("\nYou typed: %c\n", c);
//  printf("(getch example) please type a letter...");
//  c = getch();
//  printf("\nYou typed: %c\n", c);
//  return 0;
//} 
