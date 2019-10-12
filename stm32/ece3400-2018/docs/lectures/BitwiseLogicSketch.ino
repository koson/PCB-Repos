
uint8_t *heapptr, *stackptr; // global variable

void setup() {
  Serial.begin(9600);
}

void loop() {

  /*Uint8_t is a type declaration, to port between operating systems*/
  /*0b means binary, print a binary number with BIN*/
  uint8_t var_o = 0b11000000; //Variable 1
  uint8_t var_t = 0b01111110; //Variable 2
  uint8_t var_r = 0; //Result

  /*Bitwise operators*/
  /*Negated*/
  var_r = ~var_o;   
  Serial.print(var_o,BIN); Serial.print(" negated: "); Serial.println(var_r,BIN); 
  /*AND*/
  var_r = var_o&var_t;
  /*Fyi, you can use short hand operators: var_o &= var_t; is the same as var_o = var_o&var_t;*/
  Serial.print("\n"); Serial.print(var_o,BIN); Serial.print(" & "); Serial.print(var_t,BIN); Serial.print(": "); Serial.println(var_r,BIN);
  /*OR*/
  var_r = var_o|var_t;
  Serial.print("\n"); Serial.print(var_o,BIN); Serial.print(" | "); Serial.print(var_t,BIN); Serial.print(": "); Serial.println(var_r,BIN);
  /*XOR*/
  var_r = var_o^var_t;
  Serial.print("\n"); Serial.print(var_o,BIN); Serial.print(" ^ "); Serial.print(var_t,BIN); Serial.print(": "); Serial.println(var_r,BIN);

  /*Bit shift*/
  var_r = var_o>>2;
  Serial.print("\n"); Serial.print(var_o,BIN); Serial.print(" >> 2 :"); Serial.println(var_r,BIN);
  /*Bit shift signed*/
  char var_f = 0b11000000;
  var_r=var_f>>2;
  Serial.print("\n"); Serial.println("Be careful with signed variables! "); 
  Serial.print(var_o,BIN); Serial.print(" >> 2 :"); Serial.println(var_f,BIN);
  /*Setting bits*/
  Serial.print("\n"); Serial.print(var_o,BIN);
  var_o |= (1<<3);
  Serial.print(" |= (1<<3) : "); Serial.print(var_o,BIN); Serial.println(", Set bit 3");
  /*Toggling bits*/
  Serial.print("\n"); Serial.print(var_o,BIN);
  var_o ^= (1<<6);   
  Serial.print(" ^= (1<<6) : "); Serial.print(var_o,BIN); Serial.println(", Toggle bit 6");

  //Notice, that you can save RAM by storing debugging strings in Flash using the F() macro, like this:
  Serial.print("\n"); Serial.println(F("Text saved in Flash"));

  /* Notice that implicit libraries may take up a lot of unnecessary RAM
   * E.g. the HardwareSerial.h has 64bytes reserved for both transmit and receive buffers
   * If you don't expect to send/receive a lot of text these could be scaled down advantageously.
   * This also goes for the FFT library!
   */
  
  //Can we track how much memory we use during run time? Sure!

  //Your Stack pointer should always be larger than the Heap pointer 
  check_mem(); 
  Serial.write("\nheapptr: "); Serial.print((int)heapptr);
  Serial.write(", stackptr: "); Serial.println((int)stackptr);
  //Notice that the Heap pointer starts ar 2301bytes (not 2KB). That is because the RAM address actually starts at 0x0100, not 0x0000.
  digitalWrite(ledPin, HIGH);

  while(1){}
}

//Let's you check the heap and stack pointer!
void check_mem() {
  stackptr = (uint8_t *)malloc(4);  // use stackptr temporarily
  heapptr = stackptr;               // save value of heap pointer
  free(stackptr);                   // free up the memory again (sets stackptr to 0)
  stackptr =  (uint8_t *)(SP);      // save value of stack pointer
}
