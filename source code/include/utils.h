#ifndef UTILS_H
#define UTILS_H

#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <utility>
#include <iostream>
#include <fstream>
#include <thread>
#include <future>
#include <vector>
#include <type_traits>
#include <cstdlib>
#include <cstdio>
#include <conio.h>
#include <iomanip>

#define SLEEP(ms) Sleep(ms)
#define PAUSESCREEN getch()
#define CLEARSCREEN clearScreen_()

#define ESCAPED -27
#define SELECTABLE  "   "

// globals
int getConfigScreenX(void);


/// color
namespace concolor
{
    static CONSOLE_SCREEN_BUFFER_INFO* currentconcolorinfo = NULL;
    enum colorsname
    {
        Black, Blue, Green, Aqua, Red, Purple, Yellow, White,                                           // normal
        Gray, Blue_light, Green_light, Aqua_light, Red_light, Purple_light, Yellow_light, White_bright, // Light
        Color = 1                                                                                       // Reset
    };
    static bool enable_setcolor = true;
    static colorsname lscolor = Color;
    colorsname textcolor(void);
    colorsname backcolor(void);
    void setcolor(int,int = backcolor());
    void resetcolor(void);
    colorsname current_colors(void);

    template<class elem,class traits>
    inline std::basic_ostream<elem,traits>& operator << (std::basic_ostream<elem,traits>& os, colorsname cc)
    {
        if(enable_setcolor && cc != Color)
        {
            currentconcolorinfo = new CONSOLE_SCREEN_BUFFER_INFO;
            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), currentconcolorinfo);
            resetcolor();
            os.flush();
            setcolor(cc);
            lscolor = cc;
            enable_setcolor = false;
        }
        return os;
    }

    template<class elem,class traits>
    inline std::basic_ostream<elem,traits>& operator >> (std::basic_ostream<elem,traits>& os, colorsname cc)
    {
        if( (!enable_setcolor && cc == lscolor) || cc == Color )
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), currentconcolorinfo->wAttributes);
            //os.clear();
            delete currentconcolorinfo;
            enable_setcolor = true;
        }
        return os;
    }
}
void printcolor(const std::string&, concolor::colorsname, concolor::colorsname=concolor::backcolor());

/// object color struct colour
static struct _Colour {
private:
    CONSOLE_SCREEN_BUFFER_INFO _cscolorinfo_;
    bool _allowdye, _dyeback;
    int _backcolor;
    std::string _buff;
    int _forec(void){
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&csbi);
        int F=csbi.wAttributes;
        return F%16;
    }
    int _backc(void){
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&csbi);
        int B=csbi.wAttributes;
        B = (B/16)%16;
        return B;
    }
    void _dyeing(int fc,int bc){
        if((fc%16)==(bc%16))fc+=8;
        fc%=16;
        bc%=16;
        unsigned short wAttributes= ((unsigned)bc<<4)|(unsigned)fc;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wAttributes);
    }
public:
    std::string txtout;
    _Colour(){
        _buff.clear(); txtout.clear();
        _allowdye = _dyeback = false;
        _backcolor =  _backc();
    }
    struct _Colour& operator << (std::string& text){
        _buff = txtout + text;
        txtout.clear();
        _allowdye = true;
        return *this;
    }
    struct _Colour& operator >> ( concolor::colorsname color){
        if(!_allowdye) return *this;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &_cscolorinfo_);
        _dyeing(color,  (_dyeback ? _backcolor : _backc()));
        std::cout << _buff;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), _cscolorinfo_.wAttributes);
        txtout.clear();
        _allowdye = _dyeback = false;
        return *this;
    }
    struct _Colour& operator [] (int color){    /// @def set the backroud of the output
        _backcolor = color;
        _dyeback = true;
        return *this;
    }
    struct _Colour& operator () (std::string text){    /// @def set the raw text of the output (txtout)
        _buff = txtout = text;
        _allowdye = true;
        return *this;
    }
    struct _Colour& operator () (void){    /// @def set the raw text of the output (txtout)
        _buff = txtout;
        _allowdye = (!txtout.empty());
        return *this;
    }
    /**< USAGE
    *   colour << stringvar >> colorname;
    *   colour(text) >> colorname;
    *   colour(text) << stringvar >> colorname;
    *   colour[backgrund_colorname] << stringvar >> colorname;
    *   colour.txtout = "Ola Mundo!"
        colour() >> colorname;
    ****\fails
    *   colour("bug")
        colour >> colorname; // output bug
    */

} colour;

static struct _CONPRINT {
private:
    CONSOLE_SCREEN_BUFFER_INFO _cscolorinfo_;
    bool _allowdye, _dyeback;
    int _backcolor;
    std::string _buff;
    int _fgc(void){
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&csbi);
        int F=csbi.wAttributes;
        return F%16;
    }
    int _bgc(void){
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&csbi);
        int B=csbi.wAttributes;
        B = (B/16)%16;
        return B;
    }
    void _setcolor(int fc,int bc){
        if((fc%16)==(bc%16))fc+=8;
        fc%=16;
        bc%=16;
        unsigned short wAttributes= ((unsigned)bc<<4)|(unsigned)fc;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wAttributes);
    }
public:

    enum conprint_color {
        cor_info=9, cor_danger=5, cor_warn=0xE, cor_hint=0x8, cor_black=0x0, cor_white=0x7,
        cor_success=0xa, cor_error=0xc, cor_invalid=0x0, cor_purple=0x5
    };

    _CONPRINT(){
        _buff.clear();
        _allowdye = _dyeback = false;
        _backcolor =  _bgc();
    }
    struct _CONPRINT& operator () (std::string text){
        _buff = text;
        _allowdye = true;
        return *this;
    }
    struct _CONPRINT& out (std::string text) {
        return (*this)(text);
    }
    void outline(std::string text) {
        std::cout << text << std::endl;
    }
    void color(int foreground, int background = -1){
        if(!_allowdye) return;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &_cscolorinfo_);
        _setcolor(foreground,  (background<0 ? _bgc() : background) );
        std::cout << _buff;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), _cscolorinfo_.wAttributes);
        _allowdye = _dyeback = false;
        return;
    }
    void highlight(int color){
        if(!_allowdye) return;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &_cscolorinfo_);
        _setcolor(_fgc(),  color);
        std::cout << _buff;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), _cscolorinfo_.wAttributes);
        _allowdye = _dyeback = false;
        return;
    }
    void onMiddle(int screenSize=60){
        int mlen = _buff.length()/2.0;
        std::cout << std::setw(screenSize/2-mlen)<<' '<<_buff<<'\n';
        return;
    }
    void onRight(int screenSize=60){
        std::cout << std::right << std::setw(screenSize)<<_buff<<'\n';
        return;
    }
    void atPos(short int y_line, short int x_col=0){
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        COORD pos;
        pos.X = x_col; pos.Y = y_line;
        SetConsoleCursorPosition(hConsole, pos);
        std::cout <<_buff;
        SetConsoleCursorPosition(hConsole, csbi.dwCursorPosition);
        return;
    }

    void typewrite(int miliseconds=25){
        unsigned int i = 0;
        while(i < _buff.length()){
            putchar(_buff[i++]);
            fflush(stdin);
            SLEEP(miliseconds);
        }
        _allowdye = _dyeback = false;
        return;
    }
    void loader(int progress, int speed){
        static int r = 0;
        progress = progress > 100 ? 0 : progress;
        int complete = r + progress;
        while(r++ < 100){
            std::cout<<std::left<<std::setw(3)<<r<<"% \r";
            SLEEP(speed);
            if(r == complete) break;
        }
        if(r >= 100) r = 0;
        return;
    }
    /**< USAGE
    * conprint("msg").color(foreground, background);
    * conprint("msg").typewrite(milisconds);
    * conprint("msg").hightlight(color_backgorund);
    */

} conprint;

// console color struct
enum colors
{
    // black, blue, green, aqua, red, purple, yellow, white
    blk, blu, grn, aqu, red, prp, ylw, wht,       // normal
    GRY, BLU, GRN, AQU, RED, PRP, YLW, WHT,       // light
    off = 0x0ff                                   // reset
};
typedef struct ConsoleColor
{
private:
    CONSOLE_SCREEN_BUFFER_INFO _cscolorinfo_;
    bool colorkept = false;
    int fcolor(void)
    {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&csbi);
        int F=csbi.wAttributes;
        return F%16;
    }
    int bcolor(void)
    {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&csbi);
        int B=csbi.wAttributes;
        B = (B/16)%16;
        return B;
    }
    void applycolor(int fcolor,int bcolor)
    {
        if((fcolor%16)==(bcolor%16))fcolor+=8;
        fcolor%=16;
        bcolor%=16;
        if(!colorkept)
        {
            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &_cscolorinfo_);
            colorkept = true;
        }
        unsigned short wAttributes= ((unsigned)bcolor<<4)|(unsigned)fcolor;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wAttributes);
    }
public:
    const int off = 0x0ff;

    /** \usage STRUCT << Background >> foreground; */
    std::string  operator() (const std::string msg, const int color)  // cout mcolor marker
    {
        int fc = fcolor();
        applycolor(color, bcolor());
        std::cout << msg;
        applycolor(fc, bcolor());
        return "";
    }

    struct ConsoleColor operator << (int clr)  // background
    {
        applycolor(fcolor(), clr);
        return *this;
    }

    struct ConsoleColor operator >> (int clr)   // foreground
    {
        if(clr == off)
        {
            if(colorkept)
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), _cscolorinfo_.wAttributes);
                colorkept = false;
            }
        }
        else
            applycolor(clr, bcolor());
        return *this;
    }
    void operator!()
    {
        if(colorkept)
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), _cscolorinfo_.wAttributes);
            colorkept = false;
        }
    }
    void inputs(int fieldColor, int MaxLength)
    {
        // set color
        applycolor(fcolor(), fieldColor);
        // spacing field
        for(int i = 0; i < MaxLength; i++) std::cout.put(' ');
        for(int i = 0; i < MaxLength; i++) std::cout.put('\b');
        //color off
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), _cscolorinfo_.wAttributes);
        colorkept = false;
        return;
    }

} CONSOLECOLORING;

bool color_apply(unsigned int, unsigned int=concolor::backcolor());
void color_reset(void);

// strings and numbers

/// case transform
inline std::string& transformUppercase(std::string& str)
{
    for(unsigned int i = 0; i < str.length() ; i++)
        str[i] = (char) toupper(str.at(i));
    return str;
}

template <typename T>
T StringToNumber ( const std::string &Text );
float StringToDecimal(const std::string &Text);

bool stringToDateArray(int [3], std::string);

/// heading
void heading(std::string, int= getConfigScreenX());

/// fill chars
void fillch (char= ' ', unsigned int = getConfigScreenX());

/// String Input Controller
std::string stringInputController(int, bool = false, int = concolor::backcolor());

/// path string
typedef struct psbc
{
private:
    std::vector<std::string> path_vector;
    std::string path;
    void _makebread_(void)      // show current path
    {
        path="";
        unsigned int s = path_vector.size();
        for(unsigned int p = 0; p < s; p++)
        {
            path += path_vector.at(p);
            if( p+1 < s) path += separator;
        }
    }
public:
    char separator[3] = "/";
    std::string operator()(void)      // show current path
    {
        path.clear();
        unsigned int s = path_vector.size();
        for(unsigned int i = 0; i < s; i++)
        {
            path += path_vector.at(i);
            if( i+1 < s) path += separator;
        }
        return path;
    }
    void operator = (std::string _entry)    // BC="string" -> reassign as root path
    {
        _entry = transformUppercase(_entry);
        path_vector.clear();
        path_vector.push_back(_entry);
        path = _entry;
    }
    void operator += (std::string _entry)    // BC>>"string"  -> push new path
    {
        _entry = transformUppercase(_entry);
        path_vector.push_back(_entry);
        _makebread_();
    }
    std::string operator!()     // !BC  -> return current path {eg: home/profile -> >>PROFILE<< }
    {
        std::string current = path_vector.back();
        return current;
    }
    void operator--(int i)      // BC-- -> pop back path
    {
        path.clear();
        path_vector.pop_back();
    }
    std::string operator >> (std::string _entry)    // BC>>"string"  -> push new path and return string
    {
        _entry = transformUppercase(_entry);
        path_vector.push_back(_entry);
        _makebread_();
        return path;
    }
    std::string operator << (std::string _entry)    // BC<<"string" -> replace current path and return string
    {
        path.clear();
        path_vector.pop_back();
        _entry = transformUppercase(_entry);
        path_vector.push_back(_entry);
        _makebread_();
        return path;
    }
    void empty(void)        // BC.empty() -> clear all path, no trail
    {
        path_vector.clear();
        path.clear();
    }
} BREADCUMBS;


namespace loader
{
bool loader_start(int = 1);
void loader_step(void);
void loader_end(void);
}

// console displaying
void configScreen(const char*,const int, const int);
VOID clearScreen_(SHORT = 0);
VOID highlight(unsigned int, unsigned int, unsigned int);
VOID remark(unsigned int, unsigned int, unsigned int, unsigned int);
// console interaction
void gotoxy(int x, int y);
INT optionSelector(SHORT, SHORT,BOOL=TRUE, INT=1, BOOL=TRUE);
INT optionMouseSelector(SHORT, SHORT, SHORT=25);
void* detectMouseClickOn(COORD xy, INT len);



// Validation
inline bool exists_file(const std::string& name);

#endif // UTILS_H
