// заголовок с параметрами шрифтов и внешнего вида функц. блока 

#pragma once

namespace fbsvg {

struct Layout {
    int minWidth = 1200;        
    int minHeight = 300;
    int headerHeight = 32;
    int portRowHeight = 100;
    
    int portTextOffset = 4;   

    int leftMargin = 50;      
    int rightMargin = 50;

    int columnGap = 60;        

    int nameFontSize = 18;
    int portFontSize = 18;
};



} // namespace fbsvg
