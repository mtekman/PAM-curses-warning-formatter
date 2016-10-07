
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include <signal.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <curses.h>

void Quit(){
  endwin();
  exit(0);
}

void printmessage(char *message, int len, int time, int colpair);
void userget();
void blink_warn();
void formatter(float );
void top_messages();
void runFormatter(float );

WINDOW * mainwin;
int oldcur;

int rows,cols,centerX,centerY;
int x,y;


# define pause(amount){\
    usleep(amount * 1000);\
    refresh();\
  }

//#define debug

int swearline = 2, swearcol;
bool isPrinting = false;

void dummyfunc(int dd){
  if (!isPrinting){
    int tempy = y, tempx = x;
    y = swearline; x = swearcol;
    printmessage("Fuck you!", 9, 10, 1);
    y = tempy; x = tempx;
  }
};

int main(int argc, char **argv){

  // Trap ctrl -C
  ESCDELAY = 25;
  signal(SIGINT, dummyfunc);
  signal(SIGTSTP, dummyfunc);
  signal(SIGSTOP, dummyfunc);
  signal(SIGSTOP, dummyfunc);
  signal(SIGABRT, dummyfunc);
  signal(SIGTTIN, dummyfunc);
  signal(SIGTTOU, dummyfunc);
  
  //  runFormatter(atof(argv[1]));
  runFormatter(0.9);
}

void runFormatter(float thresh) {
  srand (time(NULL));
  
  /*  Initialize ncurses  */
  if ( (mainwin = initscr()) == NULL ) {
    perror("error initialising ncurses");
    exit(EXIT_FAILURE);
  }
  // Define colors
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_WHITE, COLOR_RED);
  init_pair(3, COLOR_RED, COLOR_BLACK);


  struct winsize size;
  ioctl(fileno(stdout), TIOCGWINSZ, &size);

  rows = size.ws_row;
  cols = size.ws_col;
  centerX = cols/2;
  centerY = rows/2;
  swearcol = centerX - 5;


  noecho();
  nodelay(mainwin,true);
  keypad(mainwin, TRUE);
  oldcur = curs_set(0);

  /*  Initialize and draw it  */
  box(stdscr, 0, 0);
  
  // Top messages
  top_messages();

  // Formatting
  y += 2;
  x = 3;
  printmessage( "Formatting Disks... ", 20, 50, 3);

  y += 3;
  x = centerX - 29;
  printmessage( "POWEROFF SIGNALS ARE TRAPPED TO PROMPT A FATAL DISK ERROR.", 58, 0, 3);

  y += 1;
  x = centerX - 8;
  printmessage( "Have a nice day!", 16, 0, 3);
  
  y -= 3;  
  formatter(thresh);

 
  userget();
  /*  We never get here  */
  //  return EXIT_SUCCESS;
}

void printmessage( char *message, int len, int time, int colpair){

  int m =0;
  isPrinting = true;

  wattrset( mainwin, COLOR_PAIR(colpair));
  for (; m < len; m++){
    move(y,x);
    addch( message[m] );
    pause(time); //refresh gets called too
    x++;
  }
  COLOR_PAIR(1); // reset

  isPrinting = false;
}





int lastkey = -1;

void userget(){
  /*  Loop and get user input  */
  int key = getch();

 
  if (key != -1){

    // Debugging key presses
#ifdef debug
      move(0,0);
      char res [4];
      sprintf(res, "%03d", key );
      addch( res[0]);
      move(0,1);
      addch( res[1]);
      move(0,2);
      addch( res[2]);
#endif
    
    if (lastkey == 2){     // Ctrl + B, followed by
      if (key == KEY_UP){
	Quit();
      }
    }
    
    lastkey = key;
  }
}

void top_messages(){
  y = centerY - 5;
  x = centerX - 4;
  printmessage( "YOU", 3 , 100, 1);
  x ++;
  pause(200);
  printmessage( "WERE", 4 , 100, 1);

  // Blink  "Warned"
  y ++;
  pause(200);
  blink_warn();
}


void formatter(float thresh){
  int prog_line = y;
  int fillsize = cols - 5;
  char scroller [9] = "\\\\||//--";
  float perc_width = (float)(fillsize)/100;

  char perc_bar_complete [900] = "============================================================================================================================================================================================================================================================================================================",
    perc_bar_empty [900] = "---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------";

  int total_perc_width = (int)(perc_width * 100);

  float percentage = 1;
  unsigned int count = 0;
  endwin();
  while ( percentage < 98 ){
    y = prog_line;
    x = 3;

    // Fill up to percentage with '='
    int done_chars = (int)(percentage * perc_width) + 2;
    char donesofar [done_chars];
    sprintf(donesofar, "%.*s", done_chars, perc_bar_complete);

    char ppp [3] = "==";
    sprintf(ppp, "%2d", (int)(percentage));

    donesofar[done_chars-5] = ppp[0];
    donesofar[done_chars-4] = ppp[1];
    donesofar[done_chars-3] = '%';
      
    donesofar[done_chars-2] = scroller[count % 8]; // add scroller
    donesofar[done_chars-1] = '>';

    
    // Fill the rest with '-'
    char remaining [fillsize - done_chars];
    sprintf(remaining, "%.*s", (fillsize - done_chars) - 2, perc_bar_empty);

    // Make the percentage buffer
    char prog_buffer [ total_perc_width ];
    sprintf( prog_buffer, "[%s%s]", donesofar, remaining );

    // Clear swears
    y = swearline; x = swearcol;
    printmessage( "         " , 9, 0, 1);

    // resume
    y = prog_line; x = 3;
    printmessage( prog_buffer , total_perc_width, 0, 1);

    userget();

    float new_perc = (float)(rand() % 100)/100;
    new_perc *= new_perc;
    new_perc *= new_perc;
    new_perc *= new_perc;
    new_perc *= new_perc;
    new_perc *= new_perc;
	//	    new_perc *= new_perc;
	    //	        new_perc *= new_perc;

    if (new_perc < thresh){ new_perc -= 0.005;}
    else { new_perc = 1 + (rand() % 15)/10; }

          
    percentage += new_perc;
    count ++;
    pause(10);
  }
  endwin();
  exit(0);
}


void blink_warn(){
  // WARNED pre
  int playspace = cols - 2,
      l_pad = ( playspace - 6 ) / 2,
      r_pad = ( playspace - (l_pad + 6));
 
  char lpad [l_pad + 1],
       rpad [r_pad + 1];
  

  sprintf(lpad, "%*s", l_pad, " ");
  sprintf(rpad, "%*s", r_pad, " ");

  char warn_buffer [playspace]; 

  sprintf(warn_buffer,  "%sWARNED%s ", lpad, rpad);

  // WARNED print
  int blink = 12;
  while (blink-->0){
    x = 1;
    printmessage( warn_buffer, playspace , 0, 2 - blink%2);
    pause(300);
    userget();
  }
  x= 1;
  printmessage( warn_buffer, playspace , 0, 2);
}
