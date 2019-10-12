#include "U8glib.h"
 //Driver ST7920 and PSC connect to Vcc ( mode parallel )
U8GLIB_ST7920_128X64_1X u8g(8, 9, 10, 11, 4, 5, 6, 7, 18, 17, 16);   // 8Bit Com: D0..D7: 8,9,10,11,4,5,6,7,en=18,di(rs pin)=17,rw=16, RST should goes to Vcc
void draw(void) {
  u8g.setFont(u8g_font_unifont);
  u8g.drawStr( 0, 10, "Dinh Thong BK");
}
 
void setup(void) {
 
}
 
void loop(void) {
  // picture loop
  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );
  
}
