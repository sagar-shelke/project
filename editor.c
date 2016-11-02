/*****************************************************************************
 * Copyright (C) SAGAR S. shelkesagar6500@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#include<curses.h> 
#include<menu.h> 
#include<stdio.h> 
#include<stdlib.h>
#include<string.h> 
#include<malloc.h> 
void right1();

struct node {
	int count;
   	char *array;			
   	struct node *up,*down;
 	};

       
MEVENT event;
				
typedef struct node *HEADER; 		
HEADER first, temp, last;
char copied[1000]; 
int count_copied;

WINDOW *win, *w1, *w2, *w3, *add, *curses;
/* WINDOW DESCRIPTIONS-
* Win1 = outer window containing menubar
* Win2 = inner window containing text area
* Win = actual area of text entry
* Win3 = bottom part of screen containing page no etc
*/
int posx, posy, LMARGIN, RMARGIN, BEGIN, END, WIDTH, HEIGHT;
int saved,pageno,line=1, flag = 0;
char find[20], replace[20];
char* first_flag;
char filename[10],ch; 
static int value = 2;

/*FUNCTION PROTOTYPES */

void gotomouse(int ,int);
void filehead();							/*To display the menubar*/
void openfile();	
void savefile();
void putfile();	  							 /*To display the contents of the saved file */
void newfile();								
void filemenu();	
void editmenu();	 						/*To display edit menu */
void filehead();	
void mrefresh();
void wrow(int, char *);
void addname();		
void print_loc();							/* Display page no, and position of cursor */
void up();
void down();	
void left();
void end();
void home();
void help();
void newline();
void charinsert(chtype);
void insertline();							/*Editing options */
void deletenlines(); 
void deleteline(); 
void deleteword(); 
void deletechar(); 
void dcur();
void backspace();
char displaymesg();
void helpmenu();							/*To display help menu */
void about();
void set();

void debugwindow();

HEADER getnode(); 

int main()
{
   	chtype ch; 
	/*Initializing the screen */
   	initscr();							
   	start_color();		
	/* Enable color  */								
     	mousemask(ALL_MOUSE_EVENTS, NULL);				
    	init_color(COLOR_WHITE, 0, 0, 0);					
    	init_pair(1, COLOR_WHITE, COLOR_BLUE); 				
    	init_pair(2, COLOR_BLACK, COLOR_WHITE); 
    	init_pair(3, COLOR_CYAN, COLOR_BLACK);
    	init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
    	init_pair(5, COLOR_YELLOW, COLOR_BLACK);
    	init_pair(6, COLOR_RED, COLOR_BLACK);
    	init_pair(7, COLOR_BLACK, COLOR_YELLOW);
	/* length, width, starting X Y positions of new window */
    	win = newwin(LINES-4, COLS-2, 2, 1); 				

	wbkgd(win,COLOR_PAIR(5));

    	cbreak(); 							
    	noecho();							

    	keypad(win,TRUE); 						
    	
	filehead();							/*display menubar */
	/*refreshing windows*/
    	wrefresh(w1);
    	wrefresh(w2); 
    	wrefresh(win);
    	wrefresh(w3);
    	posx = posy = 0;	   					
    	HEIGHT = LINES-4;	
    	saved = 1;
    	filename[0] ='\0';
    	last = NULL; 
    	first = getnode();						
    	temp = first;

    	wmove(win, posx, posy); 
    	wrefresh(win);
    	flushinp();							

    	addname(); 
    	wrefresh(w3);
    	wrefresh(win); 
    	print_loc();
	/* Enable the keypad */
        while(1) {
          	keypad(win, TRUE); 					
         	ch = wgetch(win);    
         	switch(ch)  {
              		case KEY_UP: up(); 
				break; 
              		case KEY_DOWN: down(); 
				break; 
              		case KEY_RIGHT: right1(); 
				break;
              		case KEY_LEFT:left();	
				break; 
              		case KEY_HOME: home(); 
				break;
              		case KEY_END:end(); 
				break;
              		case KEY_MOUSE: 
	               		if(getmouse(&event) == OK) {
             				if(event.bstate && BUTTON1_CLICKED )                                                                  
				    {												      
				       		gotomouse(event.x+1, event.y+1);
 				       		}
                         		else if(event.bstate && BUTTON3_CLICKED) {         
                              			helpmenu();
						mrefresh();
						return 0; 
						}
                              		}
                          	break;
                                 
              		case 10:newline(); 
				break;
              		case 263: backspace(); 
				break; 
              		case 330: dcur(); 
				break;
              		case 12: deleteline(); 
				break; 
              		case 23: deleteword();
				break;
              		case 6 : filemenu(); 
				mrefresh();  
				break;
              		case 5 : editmenu(); 
				mrefresh();  
				break;
              		case 8:helpmenu(); 
				mrefresh(); 
				break;
              		case 2:set(); 
				mrefresh(); 
				break; 
              		default : charinsert(ch); 
				break;
       			}
       			wmove(win, posy, posx);
       			touchwin(win); 
       			wrefresh(win);  
    		}
  }



void gotomouse(int x, int y) {
	if(y >= 0 && y <= 2) { 
		if(x >= 0 && x <= 15){
			filemenu(); 
			mrefresh();
			return;
		}
         	if(x >= 15 && x <= 30) {
			editmenu();
			mrefresh();
			return;
		}
        	if(x >= 30 && x <= 45){
			helpmenu();
			mrefresh();
			return ;
		}
        
        }

   }

/*Create a node*/

HEADER getnode() {
	HEADER tmp;
	tmp=(HEADER) (malloc(sizeof(struct node))); 
	tmp->array = (char *)malloc(sizeof(char)*WIDTH); 
	tmp->up=tmp->down = NULL;
	tmp->count = 0;
	tmp->array[0] = '\0';
	return tmp;
}

/*Display the file name */
void addname() {
	add = newwin(1, 12, 1, 31);							/*new window of height 1 width 12 starting at x = 1 y = 31 to display the filename */
	wbkgd(add, COLOR_PAIR(6));
	if(filename[0] == '\0')
    		mvwprintw(add, 0, 2,"%s", "UNTITLED");
	else  
 	   	mvwprintw(add, 0, (12-strlen(filename))/2, filename);
    		wmove(win, 0, 0);
    		wrefresh(add);
	return;
}

/*DISPLAY THE MENU */

void filehead()	{
    	w1 = newwin(1, COLS, 0, 0);						
    	wattrset(w1, A_BOLD);						
   	wbkgd(w1, COLOR_PAIR(1));						
   	waddstr(w1, "FILE[^F]     EDIT [^E]     HELP[^H] ");
   	w2 = newwin(LINES-2, COLS, 1, 0);						
   	wbkgd(w2, COLOR_PAIR(6));						
   	wborder(w2, ACS_VLINE, ACS_VLINE,ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);	
  	wrefresh(w2);								
}


/*COPY THE CURRENT LINE*/
void copyline()	{
    	int i;
     	count_copied = 0; 
     	for(i = 0; i < temp->count; i++) {
        	copied[i]=temp->array[i]; 
        	count_copied++;
        } 
    	mrefresh();
}
/*PASTE CONTENTS OF BUFFER*/
void pasteline() {
      	int i;
        for(i=0; i<count_copied ;i++) {
               	temp->array[i] = copied[i];
               	temp->count++;
             } 
      	temp->array[i] = '\0';
      	wrow(posy, temp->array);
      	mrefresh();
}



/*DISPLAY POSITION OF  CURSOR*/
void print_loc() {
   	w3 = newwin(1, COLS,LINES-1, 0);								/*this is the blue window at the bottom of the editor*/
  	wattrset(w3, A_BOLD);
  	wbkgd(w3, COLOR_PAIR(5));
  	wrefresh(w3);	
  	mvwprintw(w3, 0, 60, "%s", "PAGE NO.:");
  	pageno = (int)(line/21) + 1;
  	mvwprintw(w3, 0, 71, "%d", pageno); 
  	mvwprintw(w3, 0, 10, "%s", "ROWS:");
  	mvwprintw(w3, 0, 16, "%3d", line); 
  	mvwprintw(w3, 0, 22, "COLS:");
  	mvwprintw(w3, 0, 28, "%3d", posx+1);
  	wrefresh(w3);
  	wmove(win, posx, posy); 								/*the cursor position*/
  	return;
}
void mrefresh() {										/*REFRESH THE SCREEN*/
    	touchwin(w2);
    	wrefresh(w2); 
    	touchwin(win); 
    	wrefresh(win); 
    	touchwin(add); 
    	wrefresh(add);
    	touchwin(w3); 
    	wrefresh(w3);
    	touchwin(w1); 
    	wrefresh(w1);
}
/* DEALLOCATE  MEMORY*/
void memclose()	{
    	HEADER tmp;
    	while(first) {
        	tmp = first;
         	free(tmp->array); 
         	free(tmp);
         	first = first->down;
       	} 
    	delwin(win);
   }
/*DISPLAY FILE MENU */
void filemenu()		
{
   	WINDOW *p;
   	int i = 1, A = 0;
   	chtype ch;
   	char c;
   	p = newwin(8, 12, 1, 0);
   	wbkgd(p, COLOR_PAIR(2));
   	wattrset(p, A_BOLD);
      	wborder(p, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
   	keypad(p,TRUE);
   	mvwaddstr(p, 1, 1, "NEW");
   	mvwaddstr(p, 2, 1, "OPEN...");
   	mvwaddstr(p, 3, 1, "SAVE");
   	mvwaddstr(p, 4, 1, "SAVE AS...");
   	mvwaddstr(p, 5, 1, "EXIT");
   	wmove(p, 1, 1);
   	wrefresh(p);
   	do{
     		ch = wgetch(p);
         		if(ch == KEY_MOUSE) {
             			if(getmouse(&event) == OK) {
                			if(event.bstate && BUTTON2_PRESSED)
                   				A = 1;
                			if(event.bstate && BUTTON2_RELEASED)
                				A = 2;
                			if(event.bstate && BUTTON1_CLICKED)
                				A = 3;
 
                			}
 
   			switch(event.y-1) {
   	       			case 1: i=1;
					break;
   	    	 		case 2: i=2;
					break;
	   	 		case 3: i=3;
					break;
	   	 		case 4: i=4;
					break;
	   	 		case 5: i=5;
					break;
	   	 		case 6: i=6;
					break;
 	      		}
         	}
     		if(ch == KEY_UP) {
			if (i == 1)
 		  		i = 6;
			else
			 	i--;
       		}
    		if(ch == KEY_DOWN) {
 			if( i == 6) 
   		  	i=1;
			else
   		 	i++;
      		}
   		if(ch == KEY_RIGHT) {
  			delwin(p); 
  			mrefresh(); 
  			editmenu();
  			mrefresh();
  			return;
		}
   		if( ch == KEY_LEFT) {
  			delwin(p);
  			mrefresh();
  			helpmenu();
    	  		mrefresh(); 
  		return;
     	 }


   	wmove(p, i, 1);
   	wrefresh(p);
 	}
  	while(ch != 27 && ch != 10 && A != 3);
  	delwin(p);
  	touchwin(w2); 
  	wrefresh(w2); 
  	mrefresh();
  	if(ch == 27)
  		return;
  		wrefresh(w2); 

  	switch(i) {
		case 1: newfile(); 
			break;
		case 2: openfile(); 
			break;
		case 3: savefile(); 
			break;
		case 4: filename[0]='\0'; 
			savefile(); 
			break;
		case 5: 
			if(!saved)
              			c = displaymesg(); 
        	  	else
               			c = 'n';
     			if(c == 'y')
     				savefile();
     				memclose(); 
     				delwin(win);
     				delwin(w2); 
     				delwin(w1); 
     				system ("clear");
     				endwin();
     				exit(0);
 
      		default : 
			break;
    	}
 }

/*OPEN A FILE*/
void openfile() {
    	WINDOW *op;
    	FILE *fp; 
    	char c;
	if(!saved) {
		c = displaymesg();
		mrefresh();
	if( c == 'y') {
		touchwin(win);
		wrefresh(win);
		savefile();
   		}
         }
	op = newwin(3, 45, 18,1);
	wbkgd(op, A_STANDOUT);
	wborder(op, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
	wmove(op, 1, 4);
	waddstr(op, "OPEN FILE:");
	wrefresh(op);
	echo(); 
	wgetstr(op, filename);
	noecho();

	delwin(op);

	posx = 0; 
	posy = 0;
	wmove(win, posy, posx);
	wclrtobot(win);
	touchwin(win);
	wrefresh(win);
	memclose();
	delwin(win);
	win=newwin(LINES-4, COLS-2, 2, 1);
	wbkgd(win, COLOR_PAIR(value));
	LMARGIN = RMARGIN = 0;
	HEIGHT = LINES-4;
	WIDTH = COLS-2;

	if(filename[0] == '\0') {
		delwin(op);
		wrefresh(op);
		return;
   	} 
	fp = fopen(filename, "r");
	if(!fp) {
		fp = fopen(filename, "w");
		fputc('\n', fp);
   	}
	fclose(fp);
	line = 1;
	putfile();
	print_loc();
}

/*DISPLAY OPTION TO SAVE FILE*/
char displaymesg(){
	WINDOW *diswin;
	char ch;
	diswin = newwin(3, 45, 18, 1);
	wbkgd(diswin, A_STANDOUT); 
	wborder(diswin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
	echo();
	mvwaddstr(diswin,1,4," WANT TO SAVE THE FILE(y/n)? :"); 
	wrefresh(diswin);
	ch = wgetch(diswin);
	noecho();
	delwin(diswin);
	return(ch);
}
/*SAVE THE FILE*/
void save() {
     	FILE *fp;
     	HEADER next = first; 
     	fp = fopen(filename, "w");
    	if(fp) {
     	while(next) {
   		fputs( next->array, fp);
  		fputc('\n', fp); 
   		next = next->down;
		}
        }
	fclose(fp);
	addname();
}

/*SAVE FILE AS*/	
void savefile() {
   	WINDOW *w;
	if(!strlen(filename)) {
		w = newwin(3, 45, 18, 1);
 		wbkgd(w, A_STANDOUT);
		box(w, ACS_VLINE, ACS_HLINE); 
		wmove(w, 1, 4);
		waddstr(w, "SAVE FILE AS:");
		wrefresh(w);
		echo();
		wgetstr(w, filename);
  		if(filename[0] == '\0') {
			delwin(w);
  			wrefresh(w); 
  			return;
		}
  		if(filename[0] == '\n') {
  			delwin(w);
  			wrefresh(w); 
  			return;
                  } 
		wrefresh(w);
		save();
		noecho(); 
		addname();
		delwin(w); 
		mrefresh();
     	}
  	else
 		save();
  		saved = 1;
}

/*DISPLAY A FILE ON THE SCREEN*/
void putfile() {
	FILE *fp;
	HEADER ret;
	int i;
	char c;
	fp = fopen(filename, "r"); 
	saved = posx = posy = 0; 
	first = getnode();
	temp = first;
	ret = first;
	i = 0;
  	while((c = fgetc(fp)) != EOF) {
    		if(c == '\n'|| i == (WIDTH - 1)) {
			ret->down = getnode();
			(ret->down)->up = ret; 
			ret = ret->down;
			i = 0;
    		} 
   		else {
 			ret->array[i+1] = ret->array[i];
 			ret->array[i] = c;
 			ret->count++; 
 			i++;
			}
  		}
      		(ret->up)->down = NULL;
 		free(ret->array);
 		free(ret);
 		ret = first;
   		for(i = 0; i < HEIGHT && ret; i++) {
 			wrow(i,ret->array);
 			ret = ret->down;
		} 
		addname();
 }

/*CREATE A NEW FILE*/

void newfile()  {
   	char c; 
	if(!saved) {
   		c = displaymesg();
   		mrefresh(); 
    	if(c=='y') {
  		touchwin(win);
  		wrefresh(win); 
  		wrefresh(win); 
  		savefile();
		}
  	}
  	filename[0] = '\0'; 
  	saved = 0;
  	posx = posy = 0;
  	line = 1;
  	wmove(win, posy, posx);
  	wclrtobot(win); 
  	touchwin(win); 
  	wrefresh(win); 
  	memclose();
  	LMARGIN = RMARGIN = BEGIN = END = 0;
  	WIDTH = COLS - 2;
  	HEIGHT = LINES-4;
  	first = getnode(); 
  	temp = first;
  	delwin(win); 
  	win = newwin(HEIGHT, WIDTH, 2, 1);
  	wbkgd(win, COLOR_PAIR(value));
  	addname();
  	print_loc();
}


void wrow(int y,char *arr) { 
  	int i;
  	for(i = 0; arr[i] != '\0'; i++) { 
     		wmove(win, y, i);
     		waddch(win, arr[i]);
    	}
}

/*INSERT CHARACTER AT CURSOR POSITION*/
void charinsert(chtype c) {
  	int i;
   	if(temp->count == WIDTH-1)
     		beep(); 
  	else if(isprint(c)){
		saved=0;
		for(i = temp->count; i >= posx; i--)
			temp->array[i+1]=temp->array[i]; 
			temp->array[posx] = c;
			temp->count++;
			winsch(win,c);
			posx++;
    		} 
 	else
   		saved = 1; 
  		print_loc();
}

/*NEW LINE INSERTION WHEN ENTER KEY PRESSED*/
void newline() {
   	HEADER prev, next;
   	int i, j;
   	line++;
   	saved = 0;
   	prev = temp;
   	next = temp->down;
   	temp = getnode(); 
   	for(i = posx, j = 0; i <= prev->count;  i++, j++)
   		temp->array[j] = prev->array[i];
   		prev->array[posx] = '\0';
   		temp->count = prev->count-posx;
   		prev->count = posx; 
   		temp->up = prev; 
   		temp->down = next; 
   		prev->down = temp; 

   	if(next)
    		next->up = temp; 
   		wclrtoeol(win);
  		posy++;
  	if(posy >= HEIGHT) {
  	  	wmove(win, 0, 0);
  		wdeleteln(win); 
  		posy = HEIGHT-1;
     	} 
 	else {
    		wmove(win, posy, posx); 
    		winsertln(win);
   	} 
   	wrow(posy, temp->array);
   	posx = 0; 
   	print_loc();
}
/*CURSOR MOVEMENT UP  */
void up() {               
   	if( !temp->up)	
    		beep(); 
   	else {
		line--;	
		temp = temp->up; 
		posy--;
		posx = (posx>(temp->count)) ? temp->count : posx; 
	if(posy < 0) {
  	 	wmove(win, 0, 0);	
   		winsertln(win);	
   		wrow(0,temp->array); 
   		posy = 0;
 		}
    	} 
  	print_loc();
}

void down() {
   	if(!temp->down) 
		beep();
   	else {
 		line++;
 		temp = temp->down;
 		posy++;
 		posx = (posx>(temp->count)) ? temp->count : posx;
       if( posy >= HEIGHT) {
		wmove(win, 0, 0);
		wdeleteln(win);
		wrow(HEIGHT-1, temp->array); 
		posy = HEIGHT-1;
   		}
      	}
	print_loc();
}

void right1() {
	if(posx == temp->count)
		beep(); 
	else
		posx++;
		print_loc();
}

void left() {
	if(posx == 0)
 	  	beep(); 
	else
 		posx--; 
		print_loc();
}

														/*WHEN HOME KEY PRESSED*/
void home()  {
	posx = 0;
    	wmove(win, posx, posy); 
    	wrefresh(win);
    	print_loc();
    	return;
}

														/*WHEN END KEY PRESSED*/
void end()  {
   	posx = temp->count-1; 
   	wmove(win, posx, posy); 
   	wrefresh(win);
   	print_loc();
   	return;
}

													/*WHEN BACKSPACE IS PRESSED*/
void backspace() {
  	int i;
	if(posx == 0 && posy == 0)
 		beep(); 
	else if (posx == 0 && posy != 0) {
  		if(temp->count == 0) {
			up();
			for(i = 0; i < temp->count; i++) {
				right1();
				print_loc();
			}
		}
   	else
   		deleteline();
	}
	else {
  		saved = 0; 
  		posx--;
  		wmove(win, posy, posx); 
  		dcur();
 		}
		print_loc();
}

void dcur() {
  	int i; 
  	if(posx == temp->count)
    		beep();
  	else {
		saved = 0;
		for(i = posx; i < temp->count; i++) 
 	   		temp->array[i] = temp->array[i+1]; 
			temp->count--;
     			wdelch(win);
    		}
 }

/*DISPLAY HELP MENU*/
void help() {
  	chtype w;
  	WINDOW *he;
  	he = newwin(25, 40, 3, 25);
  	wbkgd(he, COLOR_PAIR(2));
  	wattrset(he, A_BOLD);
  	wborder(he, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER,ACS_LLCORNER,ACS_LRCORNER);
  	wrefresh(he);
  	mvwaddstr(he, 1, 15, "HELP MENU");
  	mvwaddstr(he, 2, 2, "FILE MENU	CTRL-F");
  	mvwaddstr(he, 3, 2, "EDIT MENU	CTRL-E");
  	mvwaddstr(he, 6, 2, "CURSOR UP	UP ARROW");
  	mvwaddstr(he, 7, 2, "CURSOR DOWN	DOWN ARROW");
  	mvwaddstr(he, 8, 2, "CURSOR RIGHT	RIGHT ARROW");
  	mvwaddstr(he, 9, 2, "CURSOR LEFT	LEFT ARROW");
  	mvwaddstr(he, 10, 2, "DELETE WORD	CTRL-W");
  	mvwaddstr(he, 11, 2, "DELETE LINE	CTRL-L");
  	mvwaddstr(he, 13, 2, "DELETE CHARACTER	DELETE KEY");
  	mvwaddstr(he, 15, 2, "GOTO HOME	HOME KEY ");
  	mvwaddstr(he, 16, 2, "GOTO END	END KEY ");
  	mvwaddstr(he, 14, 2, "INSERT LINE	INSERT KEY");
  	mvwaddstr(he, 5, 2, "HELP	CTRL-H");
  	mvwaddstr(he, 4, 2, "BACKGND	CTRL-B");
  	mvwaddstr(he, 19, 2,"PROJECT BY");
  	mvwaddstr(he, 20, 2, "SAGAR SHELKE. ");
  	wmove(he, 1, 1); 
  	wrefresh(he);
  	keypad(he, TRUE); 

   	do {
		w = wgetch(he); 
		wmove(he, 1, 1); 
		wrefresh(he);
    	}
  	while(w != 27 && w != 10 && w != KEY_MOUSE );
   	delwin(he); 
   	mrefresh(); 
  	if( ch == 27)
	return;
}

/*DISPLAY EDIT MENU*/
void editmenu() {
   	WINDOW *edit;
   	int  i = 1, A = 0;
   	chtype k;
   	edit = newwin(25, 20, 1, 15);
  	wbkgd(edit, COLOR_PAIR(2));
   	wattrset(edit, A_BOLD);
  	wborder(edit, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER,ACS_LLCORNER, ACS_LRCORNER); 
  	mvwaddstr(edit, 1, 1 ,"DELETE N-LINES"); 
  	mvwaddstr(edit, 2, 1, "DELETE A LINE (^L)"); 
  	mvwaddstr(edit, 3, 1, "DELETE WORD (^W)"); 
 	wmove(edit, 1, 1);
  	wrefresh(edit);
  	keypad(edit, TRUE);

	do {
		k=wgetch(edit);
 		switch(k) {
    		case KEY_MOUSE:
  	        	if(getmouse(&event) == OK) {
           			if(event.bstate && BUTTON2_PRESSED)
                   		    	A = 1;
               			if(event.bstate && BUTTON2_RELEASED)
                		          A = 2;
                		 if(event.bstate && BUTTON1_CLICKED)
                			A = 3;

          			                            
		switch(event.y)
 				  {
   			case 1: i = 1;   
				break;
   			case 2: i = 2;   
				break;
   			case 3: i = 3;   
				break;
  			case 4: i = 4;   
				break;
  			
 			 }

                 }   


		case KEY_UP : 
			if(i != 1)
 		  		i--;
			else
 				i = 4; 
			break;

		case KEY_DOWN :
 			if(i != 4)
        			i++;
 	 		else
  				i = 1;
   			break; 
		case KEY_RIGHT : 
   			delwin(edit);
   			mrefresh();
   			set();
   			mrefresh(); 
  			return;
         		break;
		case KEY_LEFT :
 			delwin(edit); 
			mrefresh(); 
 			filemenu(); 
 			mrefresh(); 
 			return;
 			break;
		default:
			break;
  		} 
 		wmove(edit, i, 1);
 		wrefresh(edit);
		}
  		while(k != 27 && k != 11 && A != 3); 
  			delwin(edit);
  			mrefresh();
  			if(k == 27)
  				return;
 		switch(i) {
			case 1 :deletenlines(); 
				break;
			case 2 :deleteline();   
				break; 
			case 3 :deleteword();   
				break; 
			case 4: dcur();         
				break;
	
   		}
}


void dline() {
 	int c;
 	HEADER prev;
 	HEADER tex = temp;
 	prev = temp->up;
  	for(c = posy; c < HEIGHT && tex; c++) {
		prev = tex;
		tex = tex->down;
    	}
  	wdeleteln(win); 
  	if(c == HEIGHT)
   		wrow(HEIGHT-1, prev->array);
 		posx = 0;
}

/*DELETE CURRENT LINE*/
void deleteline() {
      HEADER prev, next;
	saved=0;
	if(!temp -> up && !temp -> down) {
		temp->count = 0; 
		temp->array[0] = '\0';
		posx = 0;
		wmove(win,posy, posx); 
		wclrtoeol(win);
		return;
   	} 
	if(!temp->down) {
     		line--;
		print_loc();
		prev = temp->up;
		free(temp->array);
		free(temp);
		temp = prev;
		temp->down = NULL;
		posx = 0;
		wmove(win, posy, posx); 
		wclrtoeol(win);
	if(posy)
  		posy--;
	else
 		 wrow(0, temp->array);
	return;
       }


	if(!(temp->up)) {
		next = temp->down;
		free(temp->array);
		free(temp); 
		temp = next; 
		first = temp;
		temp->up = NULL;
		dline(); 
		return;
  	}
	prev = temp->up; 
	next = temp->down;
	prev->down = next; 
	next->up = prev; 
	free(temp->array); 
	free(temp);
	temp = next;
    	dline();
}

/*DELETE CURRENT CHARACTER*/

void deletechar() {
    	int i;
    	saved = 0; 
    	if(temp->count == 0)
 		beep();
    	for(i = posx; i <= temp->count; i++) 
  		temp->array[i] = temp->array[i-1]; 
    		temp->count--;
    		wdelch(win);
    		print_loc();
}
/*DELETE CURRENT WORD*/
void deleteword() {
	int cnt,r1,j;
	cnt = posx; 
	if(temp->array[posx] == '\0') {
    		beep();
         	return;
  	}
	if(temp->array[cnt] == ' ') {
  		beep();
        	return;
	}
	else{
 		if(temp->array[cnt-1] != ' ' && temp->array[cnt-1] != '\0')
   	      	beep();
  		else{ 
			while(temp->array[cnt] != ' ' && temp->array[cnt] != '\0') { 
          	   		cnt++;
				temp->count--;
				wdelch(win);
	       		}
        	}
      }
  	for(j = cnt, r1 = posx; j < COLS-2-RMARGIN-LMARGIN; j++, r1++)
  		temp->array[r1]=temp->array[j];
  		wrefresh(win);
  		saved = 0;
  		return ;
}

/*DELETE N LINES*/
void deletenlines() {
  	WINDOW *clr;
  	HEADER prev; 
  	int num;
  	char a[5];
  	prev = temp->up;
  	clr = newwin(3, 45, 18, 1);
  	wbkgd(clr, COLOR_PAIR(1));
  	wborder(clr, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER,ACS_LLCORNER,ACS_LRCORNER);
  	wmove(clr, 1, 4);
  	waddstr(clr, "ENTER NUMBER OF LINES TO BE CLEARED:");
  	echo();
  	wgetstr(clr, a);
  	noecho();
  	posx = 0;
  	sscanf(a, "%d", &num);
  	while(temp != NULL && num > 0 &&temp != prev) {
        	deleteline(); 
        	num--;
      	}
}
  
/*DISPLAY HELP MENU*/
void helpmenu() {
	WINDOW *op1;
	chtype c;
	int i = 1, A = 0;
	op1 = newwin(12, 24, 5, 28); 
	wattrset(op1, A_BOLD); 
	wbkgd(op1, COLOR_PAIR(2));
	wborder(op1,ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
	wattrset(op1, A_BOLD); 
	mvwaddstr(op1, 1 ,1, "TOPICS");
	wmove(op1, 1, 1);
	wrefresh(op1);
	keypad(op1, TRUE); 
  	do {
		c = wgetch(op1); 
		switch(c) {
   			case KEY_MOUSE:
      				if(getmouse(&event) == OK) {
           				if(event.bstate && BUTTON2_PRESSED)
                   				A = 1;
                  			if(event.bstate && BUTTON2_RELEASED)
                   				A = 2;
                 			if(event.bstate && BUTTON1_CLICKED)
                   				A = 3;

       	    			switch(event.y-1) {
  			 		case 1: i = 1;
						break;
  
 			 		}

				}
   				break;
			case KEY_UP: 
				if(i == 2)
 		 			i--;
				else
 		 			i = 1; 
					break;

			case KEY_DOWN: 
 				if(i == 1)
  	 				i++;  
 	 			else
  	  				i = 1;
				 	break;

			case KEY_RIGHT:
				delwin(op1);
				mrefresh();
				filemenu();
				mrefresh();
				return;
				break;

			case KEY_LEFT:delwin(op1); 
				mrefresh(); 
				helpmenu(); 
				mrefresh(); 
				return;
				break;
			default : break;
  			}
  	wmove(op1, 1, 1); 
  	wrefresh(op1); 
	}
	while(c != 27 && c != 10 &&A != 3);
  	delwin(op1); 
	touchwin(w2); 
  	wrefresh(w2); 
 	mrefresh();
   	if(c == 27)
		return; 
  		wrefresh(w2); 
  	switch(i) {
		case 1: help();
			break; 
		default:
                     	break;
      		}
  	}


void set() {
	WINDOW *w;
	chtype a;
	int i = 1, A = 0;
	w = newwin(3, 8, 1, 30); 
	wattrset(w, A_BOLD); 
	wbkgd(w, COLOR_PAIR(5));
	wborder(w, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
	wrefresh(w);
	wattrset(w, A_BOLD);
	mvwaddstr(w, 1, 1, "COLORS"); 
	wmove(w, 1, 1);
	wrefresh(w);
	keypad(w, TRUE);
	do {
		a = wgetch(w);
		switch(a) {
			case KEY_MOUSE:
  	 	      		if(getmouse(&event) == OK) {
           		   		if(event.bstate &&BUTTON2_PRESSED)
                  	 			A = 1;
                	   		if(event.bstate && BUTTON2_RELEASED)
               		 			A = 2;
                	   		if(event.bstate && BUTTON1_CLICKED)
                				A = 3;

         		  	}
   

			case KEY_UP:
 	 			i = 1; 
 				break;
			case KEY_DOWN:
				i = 1; 
				break;
			case KEY_RIGHT: 
  				delwin(w);
  				mrefresh();
       				helpmenu();
  				mrefresh();
  				return;
				break;
			case KEY_LEFT: 
  				delwin(w);
 				mrefresh(); 
 				editmenu(); 
			 	mrefresh();
 				return;
 	 			break; 
			default: 
				break;
    		}
   		wmove(w, 1, 1);
	}while(a != 27 && a != 10 && A != 3); 
      	delwin(w);
	mrefresh();
	if(a == 27)
  		return;
	
	
}
/*FUNCTION TO DEBUG PROGRAM BY DISPLAYING A WINDOW */

void debugwindow(char *s)        	
{
	WINDOW *w;
	char *st;
	w = newwin(4, 55, 20, 2);
	wbkgd(w, A_STANDOUT);
	box(w, ACS_VLINE, ACS_HLINE); 
	wmove(w, 1, 4);
	waddstr(w, s);
	wrefresh(w);
	echo();
	wgetstr(w, st);
	wrefresh(w);
	noecho(); 
	delwin(w); 
	mrefresh();
}
