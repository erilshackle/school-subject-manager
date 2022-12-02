#include "utils.h"

#include <string>
#include <iomanip>
#include <ctime>
#include <cwchar>

// Console Displaying
INT ScreenWidth = 0;
void configScreen(const char* title, const int width, const int height)
{
    char mode[35];
    HWND consoleWindow = GetConsoleWindow();
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    // SSet title
    SetConsoleTitleA((LPCSTR)title);
    // Set screen size
    sprintf(mode,"mode con: cols=%i lines=%i 2>nul", width, height);
    system(mode);
    ScreenWidth = width;
    // Disable Resize
    SetWindowLong(consoleWindow, GWL_STYLE, GetWindowLong(consoleWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);
    // set backgound defult color
    csbi.wAttributes = 0x0F;
    SetConsoleTextAttribute(hConsole, csbi.wAttributes);
    return;
}
int getConfigScreenX(void) {return ScreenWidth > 0? ScreenWidth : 0;}

// Colors
CONSOLE_SCREEN_BUFFER_INFO currcolor;
BOOL _currcolorisdefined = FALSE, lsc;
concolor::colorsname concolor::textcolor(void)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&csbi);
    int F= (csbi.wAttributes) % 16;
    return (concolor::colorsname)F;
}
concolor::colorsname concolor::backcolor(void)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&csbi);
    int B=csbi.wAttributes;
    B = (B/16)%16;
    return (concolor::colorsname)B;
}
void concolor::setcolor(int textcolor,int backcolor)
{
    if((textcolor%16)==(backcolor%16))textcolor+=8;
    textcolor%=16;
    backcolor%=16;
    if(!lsc)
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &currcolor);
    _currcolorisdefined = TRUE;
    unsigned short wAttributes= ((unsigned)backcolor<<4)|(unsigned)textcolor;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wAttributes);
}
concolor::colorsname concolor::current_colors(void)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&csbi);
    int BF = (csbi.wAttributes);
    return (concolor::colorsname)BF;
}


void concolor::resetcolor(void)
{
    if(_currcolorisdefined == TRUE)
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), currcolor.wAttributes);
}
void printcolor(const std::string& str, concolor::colorsname fc, concolor::colorsname bc)
{
    concolor::setcolor(fc,bc);
    std::cout << str;
    concolor::resetcolor();
    return;
}

// string and numbers

template <class T>
T StringToNumber ( const std::string &Text )
{
    // Usage: StringToNumber<Type> ( String );
    std::istringstream ss(Text);
    T result;
    return ss >> result ? result : (T)0;
}
float StringToDecimal(const std::string &Text){
    std::istringstream ss(Text);
    float result;
    return ss >> result ? result : (float)0;
}

bool stringToDateArray(int myDate[3], std::string str_time)
{
    time_t rightnow = time(0);
    struct tm* now = localtime(&rightnow);
    std::string date("");
    int month_cmp;

    int defTime[5];
    defTime[0] = now->tm_mday;
    defTime[1] = now->tm_mon+1;
    defTime[2] = now->tm_year+1900;
    defTime[3] = defTime[4] = 0;

    month_cmp = defTime[1];

    short int t = 0;

    for(unsigned int i = 0; i < str_time.length(); i++)
    {
        if(t >= 3 ) break;

        if(isdigit(str_time[i])) {
            date += str_time[i];
        }
        else if( !date.empty() ) {
            defTime[t] = StringToNumber<int>(date);
            if(t == 1)
            {
                month_cmp -= defTime[1];
                if( month_cmp > 5 )         defTime[2] += 1;
                else if (month_cmp < -5)    defTime[2] -= 1;
            }
            date = "";
            t++;
        }
    }
    if( !date.empty() )
    {
        defTime[t] = StringToNumber<int>(date);
        if(++t == 2)
        {
            if( month_cmp - defTime[1] > 5 )         defTime[2] += 1;
            else if (month_cmp - defTime[1] < -5)    defTime[2] -= 1;
        }
    }
    defTime[3] = 0; // hour
    defTime[4] = 0; // minute

    for(int r = 0; r < 3; r++)
    {
        if( defTime[r] <= 0) return false;
        myDate[r] = defTime[r];
    }

    myDate[2] += myDate[2] < 1000 ? 2000 : 0;

    return true;
}


// heading
void heading(std::string TITLE, int w)
{
    if (TITLE.length() > (unsigned)getConfigScreenX() && getConfigScreenX() > 0){
        std::cerr << TITLE;
        return;
    }
    w *= w < 0 ? -1 : 1;
    TITLE = transformUppercase(TITLE);
    if(w == getConfigScreenX()) w--;
    else if(w == 1){ w = TITLE.length(); w+=2; }
    else if(w < 5 && w != 0) w = getConfigScreenX()/abs(w)+1;
    concolor::setcolor(concolor::backcolor(), concolor::textcolor());
    std::cout << TITLE<<std::endl;
    concolor::resetcolor();
    for(int i = 0; i < w; i++)
        std::cout.put('=');
    std::cout<<std::endl;
}

// fill chars
void fillch (char c, unsigned int w){
    if(w == (unsigned)getConfigScreenX()) w--;
    while(w-- > 0) std::cout.put(c);
    std::cout<<std::endl;
}

// string Input Controller
std::string stringInputController(int field_length, bool trunc_input, int field_color)
{
    char BS = 0x08, CR = 0x0D, LF = 0x0A, ESC = 0x1B, SPACE = 0x20;
    std::string input;
    char ch;
    int len = 0;
    int field_textcolor = 0x0, field_backcolor = field_color % 16;

    if( (0x0 == field_color) && (concolor::Black == concolor::backcolor())){
        field_backcolor = 0xF;
    }
    if(field_backcolor < 8) field_textcolor = 0xF;

    concolor::resetcolor();
    concolor::setcolor(field_textcolor, field_backcolor);

    for(int i = 0; i < field_length; i++) std::cout.put(SPACE);
    for(int i = 0; i < field_length; i++) std::cout.put(BS);

    input.clear();
    if(trunc_input){
        while(true)
        {
            ch = (char) getch();

            if(ch == LF || ch == CR) break;
            else
            if(ch == ESC){
                input.clear();
                break;
            }
            else if(ch < 0x20 && ch != BS) continue;
            else
            if(len >= 0 && len <= field_length)
            {
                if(ch == BS && len > 0){
                        input.erase(len-1);
                        std::cout << "\b \b";
                }
                else if(ch < 32) continue;
                else if(len < field_length && len >= 0 && ch != '\b'){
                        input += ch;
                        std::cout.put(ch);
                }
                len = input.length();
            }
        }
    }else
    {
        std::getline(std::cin, input);
        if(input.length() >= (unsigned)field_length){
            while(input.length() >= (unsigned)field_length) input.pop_back();
        }
    }
    concolor::resetcolor();
    std::cout.put('\n');
    return input;
}

// loader
int _loader_status_[3] = {
    0, 0, 100   // start, end, division
};
bool loader::loader_start(int steps){
    if(steps <= 0) return false;
    _loader_status_[0] = 0;
    _loader_status_[1] = steps;
    _loader_status_[2] = (int)100/steps;
    std::cout << "0%"<<"\b\b";
    return true;
}
void loader::loader_step(void){
    if(_loader_status_[1] == 0) return;
    int load = 0;
    _loader_status_[0]++;
    while(load++ < _loader_status_[2]){
        std::cout << std::setw(2)<< load * _loader_status_[0] <<"%"<<"\b\b\b";
        SLEEP(15);
    }
    if(_loader_status_[0] == _loader_status_[1]){
        _loader_status_[0] = _loader_status_[1] = 0;
        _loader_status_[2] =  100;
    }
}
void loader::loader_end(void){
    if(_loader_status_[1] == 0) return;
    int load = _loader_status_[0] * _loader_status_[2];
    while(load++ < 100){
        std::cout << std::setw(2)<< load <<'%'<<"\b\b\b";
        SLEEP(15);
    }
    std::cout.put('\b');
    _loader_status_[0] = _loader_status_[1] = 0;
    _loader_status_[2] =  100;
    return;
}

VOID highlight(unsigned int line_length, unsigned int X, unsigned int Y)
{
    /// not ready
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD line, area;
    GetConsoleScreenBufferInfo(hStdOut, &csbi);
    csbi.wAttributes = ((csbi.wAttributes) % 16) * 0x0E;
    COORD curpos = {(SHORT)X, (SHORT)Y};
    area = line_length;
    if(!FillConsoleOutputAttribute(hStdOut,csbi.wAttributes, area, curpos, &line)) return;
    SetConsoleCursorPosition(hStdOut, curpos);
    return;
}
VOID remark(unsigned int posX, unsigned int posY, unsigned int length, unsigned int color)
{
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD line, width;
    GetConsoleScreenBufferInfo(hStdOut, &csbi);
    // getcolor
    int backg = csbi.wAttributes, foreg = color%16;
    backg = (backg/16)%16;
    if((foreg%16)==(backg%16))foreg+=8;
    // cursor
    COORD curpos = {(SHORT)posX, (SHORT)posY};
    width = length;
    SetConsoleCursorPosition(hStdOut, curpos);
    FillConsoleOutputAttribute(hStdOut, backg*16+foreg, width, curpos, &line);
}

VOID clearScreen_(SHORT fromline)
{
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD line, area;
    COORD homepoint = {0, fromline};
    if(hStdOut == INVALID_HANDLE_VALUE) return;
    if(!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
    area = csbi.dwSize.X * csbi.dwSize.Y;
    if(!FillConsoleOutputCharacter(hStdOut,0x20, area, homepoint, &line)) return;
    if(!FillConsoleOutputAttribute(hStdOut,csbi.wAttributes, area, homepoint, &line)) return;
    SetConsoleCursorPosition(hStdOut, homepoint);
    return;
}

// Console Interaction
void gotoxy(int x, int y)
{
    COORD coord;
    coord.X = x; coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    return;
}
INT optionSelector(SHORT posini, SHORT items, BOOL last_opt_return, INT skips, BOOL esc_do_return)
{
    unsigned char KEY;
    const unsigned short int KEY_UP = 72, KEY_DOWN = 80, KEY_ESC = 0x1B, KEY_RETURN = 0x0D, KEY_BACKSPACE = 0x08;
    const char indicator[] = " >\b", unselect[] = " \b";
    if(!esc_do_return && !last_opt_return) esc_do_return = TRUE; // there must be a way to return ESCAPE
    if(skips < 1 || skips > 3) skips = 1;
    int pointer = 0, len = strlen(indicator);
    INT Selector;
    // console
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE hin = GetStdHandle(STD_INPUT_HANDLE);
    COORD pos = {0, posini};
    SetConsoleCursorPosition(hConsole, pos);
    WriteConsole(hConsole, indicator, len, NULL, NULL);
    FlushConsoleInputBuffer(hin);
    bool selected = false;
    items--;
    fflush(stdin);
    while(!selected){
        KEY = getch();
        switch(KEY)
        {
        case KEY_UP:    //if (GetAsyncKeyState(VK_UP) != 0){
            if (pointer > 0){
                pointer--;
                pos.Y = (SHORT) (posini+pointer*skips );
                WriteConsole(hConsole, unselect, len-1, NULL, NULL);
                SetConsoleCursorPosition(hConsole, pos);
                WriteConsole(hConsole, indicator, len, NULL, NULL);
            }else pointer = 0;
            break;

        case KEY_DOWN:  //else if (GetAsyncKeyState(VK_DOWN) != 0){
            if (pointer < items){
                pointer++;
                pos.Y = (SHORT) (posini+pointer*skips);
                WriteConsole(hConsole, unselect, len-1, NULL, NULL);
                SetConsoleCursorPosition(hConsole, pos);
                WriteConsole(hConsole, indicator, len, NULL, NULL);
            }else pointer = items;
            break;

        case KEY_ESC:   //else if (GetAsyncKeyState(VK_ESC) != 0 ){
        case KEY_BACKSPACE:
            Selector = esc_do_return ? ESCAPED : -0x08;
            selected = true;
            break;

        case KEY_RETURN://else if (GetAsyncKeyState(VK_RETURN) != 0 && (time(NULL) > wait+0.5) ){
            if(last_opt_return && pointer == items)
                Selector = ESCAPED;
            else
                Selector = pointer;
            selected = true;
            std::cout << "\b>";
            Sleep(150);
            break;
        }
         //Sleep(150);
    }
    FlushConsoleInputBuffer(hin);
    std::cin.clear();
    pos.Y = (SHORT) (2+posini+items);
    SetConsoleCursorPosition(hConsole, pos);
    return Selector;
}
INT optionMouseSelector(SHORT iniLine, SHORT n_items__lastReturns, SHORT listWidth)
{
    // declaration
    //HANDLE hout= GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE hin = GetStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD InputRecord;
    DWORD Events;
    // vars
    int mouseY, endLine = iniLine+n_items__lastReturns-1;
    SLEEP(200);
    FlushConsoleInputBuffer(hin);

    SetConsoleMode(hin, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);
    while(true)
    {
        ReadConsoleInput(hin, &InputRecord, 1, &Events);

            switch ( InputRecord.EventType ){
                case MOUSE_EVENT: // mouse input
                    if( (InputRecord.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) &&
                        (InputRecord.Event.MouseEvent.dwEventFlags == DOUBLE_CLICK)   )
                    {
                        //mouseX = InputRecord.Event.MouseEvent.dwMousePosition.X;
                        mouseY = InputRecord.Event.MouseEvent.dwMousePosition.Y;
                        if ( mouseY >= iniLine && mouseY <= endLine){
                            remark(0,mouseY, listWidth, concolor::Yellow_light);
                            SLEEP(500);
                            FlushConsoleInputBuffer(hin);
                            if(mouseY == endLine)
                                return ESCAPED;
                            else
                                return mouseY-iniLine;
                        }
                    }// mouse
        }
    }
}

void* detectMouseClickOn(COORD xy, INT len)
{
    HANDLE hin = GetStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD InputRecord;
    DWORD Events;
    //SetConsoleMode(hin, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);
    int mX, mY;
    FlushConsoleInputBuffer(hin);
    while(true)
    {
        ReadConsoleInput(hin, &InputRecord, 1, &Events);

        switch ( InputRecord.EventType ){

            case MOUSE_EVENT: // mouse input
                if(InputRecord.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
                {
                    mX = InputRecord.Event.MouseEvent.dwMousePosition.X;
                    mY = InputRecord.Event.MouseEvent.dwMousePosition.Y;
                    if (mX >= xy.X && mY == xy.Y && mX <= len){
                            std::cout<<"Opetion " << mY <<" selected." << std::endl;
                            Sleep(500);
                            //SM::_update_updater();
//                            args_->__fn();
                            return  NULL;
                        }
                }// mouse
        }
    }
}


inline bool exists_file(const std::string& filepath)
{
    if (FILE *file = fopen(filepath.c_str(), "r")) {
        fclose(file);
        return true;
    }
    return false;
}
