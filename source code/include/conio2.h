enum DOS_COLORS {
     BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN,
     LIGHT_GRAY, DARK_GRAY, LIGHT_BLUE, LIGHT_GREEN, LIGHT_CYAN,
     LIGHT_RED, LIGHT_MAGENTA, YELLOW, WHITE
};
void gotoxy(int coluna, int linha);
void textcolor (DOS_COLORS iColor);
void textbackground (DOS_COLORS iColor);
void wait ( float seconds );
void printxy(int coluna, int linha, char texto[]) ;