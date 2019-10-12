/*
credit: http://arduino.vn/bai-viet/163-bai-12-phat-nhat-bang-arduino-voi-mot-cai-loa-hoac-buzzer
pin 12 -- 100 Ohm resistor -- Speaker pin
*/
#include "pitch.h"
//pin connect to speaker or buzzer
int speakerPin = 12;
// melody list
int melody[] = {
  NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4};
// thời gina các nốt nhạc: 4 = 1/4 nốt nhạc, 8 = 1/8nốt nhạc, ...:
int noteDurations[] = {
  4, 8, 8, 4,4,4,4,4 };
void setup() {
 
}

void loop() {
   //không cần phải pinMode cho speakerPin
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    /* bây giờ ta đặt một nốt nhạc là 1 giây = 1000 mili giây
     thì ta chia cho các thành phần noteDurations thì sẽ
     được thời gian chơi các nốt nhạc
     ví dụ: 4 => 1000/4; 8 ==> 1000/8 */
    int noteDuration = 1000/noteDurations[thisNote];
    tone(speakerPin, melody[thisNote],noteDuration);
    /* để phân biệt các nốt nhạc hãy delay giữa các nốt nhạc
     một khoảng thời gian vừa phải. Ví dụ sau đây thực hiện tốt
     điều đó: Ta sẽ cộng 30% và thời lượng của một nốt
     */
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    //Ngừng phát nhạc để sau đó chơi nhạc tiếp!
    noTone(speakerPin);
  }
}
