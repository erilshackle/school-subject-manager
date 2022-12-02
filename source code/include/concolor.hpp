#ifndef CONSOLECOLOR_HPP
#define CONSOLECOLOR_HPP

#include <iostream>
#include <string>
#include <type_traits>
#include <windows.h>

typedef enum _concolors {
    cor_black, cor_blue, cor_green, cor_aqua, cor_red, cor_purple, cor_yellow, cor_white,
    cor_gray,cor_lightblue, cor_light_green, cor_lightaqua, cor_lightred, cor_lightpurple, cor_lightyellow, cor_brightwhite,
    cor_default = 0xFF
}corname;

static
struct _concor_{
    private:
        std::string __text;
        int _backcor, _forecor;
        bool _isset_cor = false, _isset_cor_back = false;
        struct _concor_* __cc = NULL;
        CONSOLE_SCREEN_BUFFER_INFO _cc_csbi;
        bool _cc_csbi_cor_set;
        int _cc_csbi_cor;

        int _color_fg(void) {
            CONSOLE_SCREEN_BUFFER_INFO cbi;
            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cbi);
            return  cbi.wAttributes;
        }
        int _color_back(void) { return _color_fg() / 16; }
        int _color_fore(void) { return _color_fg() % 16; }

        struct _concor_* _setnewcc(std::string text, int color){
            if(__cc != NULL) delete __cc;
            struct _concor_* cc = new struct _concor_();
            cc->__text = text;
            cc->_backcor = _color_back();
            cc->_forecor = color % 16;
            return cc;
        }
        struct _concor_* _setnewcc(int num, int color){
            return this->_setnewcc(std::to_string(num),color);
        }
        struct _concor_* _setnewcc(double num, int color){
            return this->_setnewcc(std::to_string(num),color);
        }

    public:

        _concor_(){
            _forecor = _color_fore();
            __text.clear();
        }
        struct _concor_& operator () (std::string text){
            _isset_cor = false;
            _isset_cor_back = false;
            __text = text;
            return *this;
        }
        struct _concor_& operator () (int i){
            return (*this)(std::to_string(i));
        }
        struct _concor_& operator () (double f){
            return (*this)(std::to_string(f));
        }


        void operator << (corname cor){
            if(!_cc_csbi_cor_set){
                GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &_cc_csbi);
                unsigned int attribute = cor + _color_back() * 16;
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attribute);
                _cc_csbi_cor_set  = true;
                _cc_csbi_cor = cor;
            }
            return;
        }
        void operator >> (corname cor){
            if(_cc_csbi_cor_set && cor == _cc_csbi_cor){
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), _cc_csbi.wAttributes);
                _cc_csbi_cor_set  = false;
                _cc_csbi_cor = cor_default;
            }
            return;
        }

        template<class E,class T> friend inline std::basic_ostream<E,T>&
        operator << (std::basic_ostream<E,T>& os, struct _concor_& cc);

        // acessible
        int _color(){return _forecor + _backcor * 16;}
        std::string _text(){return __text;}

        // themeing
        struct _concor_& themecor(corname cor, std::string str)          {return *_setnewcc(str,cor);}
        struct _concor_& themecor(corname cor, int num)                  {return *_setnewcc(num,cor);}
        struct _concor_& themecor(corname cor, double num)               {return *_setnewcc(num,cor);}

        // colors text
        struct _concor_& black(std::string str)         {return *_setnewcc(str,0x0);}
        struct _concor_& blue(std::string str)          {return *_setnewcc(str,0x1);}
        struct _concor_& green(std::string str)         {return *_setnewcc(str,0x2);}
        struct _concor_& aqua(std::string str)          {return *_setnewcc(str,0x3);}
        struct _concor_& red(std::string str)           {return *_setnewcc(str,0x4);}
        struct _concor_& purple(std::string str)        {return *_setnewcc(str,0x5);}
        struct _concor_& yellow(std::string str)        {return *_setnewcc(str,0x6);}
        struct _concor_& white(std::string str)         {return *_setnewcc(str,0x7);}
        struct _concor_& gray(std::string str)          {return *_setnewcc(str,0x8);}
        struct _concor_& bluelight(std::string str)     {return *_setnewcc(str,0x9);}
        struct _concor_& greenlight(std::string str)    {return *_setnewcc(str,0xA);}
        struct _concor_& aqualight(std::string str)     {return *_setnewcc(str,0xB);}
        struct _concor_& redlight(std::string str)      {return *_setnewcc(str,0xC);}
        struct _concor_& purplelight(std::string str)   {return *_setnewcc(str,0xD);}
        struct _concor_& yellowbright(std::string str)  {return *_setnewcc(str,0xE);}
        struct _concor_& whitebright(std::string str)   {return *_setnewcc(str,0xF);}
        // colors integer
        struct _concor_& black(int num)                 {return *_setnewcc(num,0x0);}
        struct _concor_& blue(int num)                  {return *_setnewcc(num,0x1);}
        struct _concor_& green(int num)                 {return *_setnewcc(num,0x2);}
        struct _concor_& aqua(int num)                  {return *_setnewcc(num,0x3);}
        struct _concor_& red(int num)                   {return *_setnewcc(num,0x4);}
        struct _concor_& purple(int num)                {return *_setnewcc(num,0x5);}
        struct _concor_& yellow(int num)                {return *_setnewcc(num,0x6);}
        struct _concor_& white(int num)                 {return *_setnewcc(num,0x7);}
        struct _concor_& gray(int num)                  {return *_setnewcc(num,0x8);}
        struct _concor_& bluelight(int num)             {return *_setnewcc(num,0x9);}
        struct _concor_& greenlight(int num)            {return *_setnewcc(num,0xA);}
        struct _concor_& aqualight(int num)             {return *_setnewcc(num,0xB);}
        struct _concor_& redlight(int num)              {return *_setnewcc(num,0xC);}
        struct _concor_& purplelight(int num)           {return *_setnewcc(num,0xD);}
        struct _concor_& yellowbright(int num)          {return *_setnewcc(num,0xE);}
        struct _concor_& whitebright(int num)           {return *_setnewcc(num,0xF);}
        // colors real
        struct _concor_& black(double num)              {return *_setnewcc(num,0x0);}
        struct _concor_& blue(double num)               {return *_setnewcc(num,0x1);}
        struct _concor_& green(double num)              {return *_setnewcc(num,0x2);}
        struct _concor_& aqua(double num)               {return *_setnewcc(num,0x3);}
        struct _concor_& red(double num)                {return *_setnewcc(num,0x4);}
        struct _concor_& purple(double num)             {return *_setnewcc(num,0x5);}
        struct _concor_& yellow(double num)             {return *_setnewcc(num,0x6);}
        struct _concor_& white(double num)              {return *_setnewcc(num,0x7);}
        struct _concor_& gray(double num)               {return *_setnewcc(num,0x8);}
        struct _concor_& bluelight(double num)          {return *_setnewcc(num,0x9);}
        struct _concor_& greenlight(double num)         {return *_setnewcc(num,0xA);}
        struct _concor_& aqualight(double num)          {return *_setnewcc(num,0xB);}
        struct _concor_& redlight(double num)           {return *_setnewcc(num,0xC);}
        struct _concor_& purplelight(double num)        {return *_setnewcc(num,0xD);}
        struct _concor_& yellowbright(double num)       {return *_setnewcc(num,0xE);}
        struct _concor_& whitebright(double num)        {return *_setnewcc(num,0xF);}

        // color advanced
//        struct _concor_& invert(){
//            __text = this->__text;
//            _backcor = this->_forecor;
//            _forecor = _color_back();
//            return *this;
//        }
        struct _concor_& bg(corname cor){
            if(!_isset_cor_back){
                if(cor == cor_default /* invert */){
                    _backcor = this->_forecor;
                    _forecor = _color_back();
                } else {
                    _backcor = cor;
                }
                _isset_cor_back = true;
            }
            return *this;
        }

        /** @def
        *   SYNTAX OF USAGE
        */
        //!  @concor << con_colorname;  ...  concor >> con_colorname;
        //!  @cout << ... << concor.con_colorname("Text") << ... ;
        //!  @cout << ... << concor.con_colorname(Number) << ... ;

}concor;


template<class E,class T>
inline std::basic_ostream<E,T>& operator << (std::basic_ostream<E,T>& os, struct _concor_& cc)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), cc._color());      // set cor
    os << cc._text();                                                           // apply cor
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), csbi.wAttributes); // reset cor
    delete &cc;
    return os;
}



#endif // CONSOLECOLOR_HPP
