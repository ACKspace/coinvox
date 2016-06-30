#include <Wire.h>

/////////////////////////////////////////////////////////////////////////////////////////

volatile boolean interrupt = false;
uint8_t keypad = 0;

void int_keypad( )
{
  interrupt = true;
}

/////////////////////////////////////////////////////////////////////////////////////////

void setup( )
{
  Wire.begin( );

  Serial.begin( 9600 );
  Serial.println( "Coinvox test app." );

  pinMode( 2, INPUT_PULLUP );
  attachInterrupt( digitalPinToInterrupt( 2 ), int_keypad, CHANGE );

  initDisplay( );
  writeWelcome( );

  initKeypad( );
}

void loop( )
{
  if ( interrupt )
  {
    char keypad = readKeypad( );

    // Anti-bounce
    delay( 50 );
    
    if ( keypad )
      Serial.print( keypad );

    initKeypad( );
    interrupt = false;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////

/*
C
last command
command continues

S011100?WaC

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

mode set
C 1 0 LP E B M1 M2 

M1 M0
1:4 MUX (4bp)
static 1bp
1:2 MUX (2bp)
1:3 MUX (3bp)

B
1/3 bias
1/2 bias

E
disabled (blank)
enabled

LP
normal mode
power saving

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

load data pointer
C 0 P5 P4 P3 P2 P1 P0
6-bit 0-39

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

device select
C 1 1 0 0 A2 A1 A0
select 1 of 8

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

bank select
C 1 1 1 1 0 I O

I (input bank selection, storage of arriving data)
ram bit 0 (bits 0 & 1)
ram bit 2 (bits 2 & 3)

O (output bank selection, retrieval of LCD display data, The BANK SELECT command has no effect in 1 : 3 and 1 : 4 multiplex drive modes)
ram bit 0 (bits 0 & 1)
ram bit 2 (bits 2 & 3)

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

blink
C 1 1 1 0 A BF1 BF0

BF1 BF0
off
2Hz
1Hz
0.5Hz

A
normal blinking
alternation blinking
*/

void initDisplay( )
{
  byte error;

  Serial.print( "Display init: " );

  // Talk to PCF8576
  Wire.beginTransmission( 0x38 );

  // Need to set mode and bank select the first time
  // Device select C 1 1 0 0 A2 A1 A0
  Wire.write( 0b11100000 );

  // Mode 4BP
  // C 1 0 LP E B M1 M2 
  Wire.write( 0b11001000 );
  // Bank select 2
  Wire.write( 0b11111010 );
  // Blink select (1Hz)
  //Wire.write( 0b11110001 );
  // Load data pointer C 0 P5 P4 P3 P2 P1 P0
  Wire.write( 0b00000000 );

  for ( int x = 0; x < 20; x++ ) // 10 nibbles of 4 backplanes
    Wire.write( 0xff );

  error = Wire.endTransmission( );

  if ( error != 0 )
  {
    Serial.print( " Error: " );
    Serial.print( error, HEX );
    Serial.print( "." );
  }
  else
  {
    Serial.print( "." );
  }
  
  delay( 1000 );

  // Talk to PCF8576
  Wire.beginTransmission( 0x38 );
  // Device select C 1 1 0 0 A2 A1 A0
  Wire.write( 0b11100000 );
  Wire.write( 0b00000000 );

  for ( int x = 0; x < 20; x++ ) // 10 nibbles of 4 backplanes
    Wire.write( 0x00 );

  error = Wire.endTransmission( );

  if ( error != 0 )
  {
    Serial.print( " Error: " );
    Serial.print( error, HEX );
    Serial.println( "." );
  }
  else
  {
    Serial.println( "Done" );
  }
}

void writeWelcome( )
{
  byte error;

/*
       15
   _________

 | \11 |   / |
2|  \  |7 /6 |14
 |   \ | /   |
10----  -----5
 |   / | \   |
1|  /  |  \4 |13
 | /9  |8  \ |
  __________
       0       o12
pin 3:N/C
*/

const uint16_t s14_a = 0b0010001000100001;
const uint16_t s14_d = 0b0110001000100001;
const uint16_t s14_f = 0b1000010000000110;
const uint16_t s14_h = 0b0010010000100110;
const uint16_t s14_k = 0b0000000111010000;
const uint16_t s14_o = 0b0010010000100011;
const uint16_t s14_K = 0b0000010001010110;

// From http://www.deater.net/weave/vmwprod/meter/
// a1: http://www.fonts2u.com/segment14.font
// a2: http://cdn.myfonts.net/s/aw/720x360/193/0/98937.png

uint16_t PROGMEM segment_14[/*28*/] = {
  0b0000000000000000, // space
  0b1000100101000001, // !
  0b0100000010000000, // "
  0b0110010110100000, // #
  0b1010010110100101, // $
  0b0010001001000100, // %
  0b1000110001010011, // &
  0b0000000001000000, // '
  0b0000000001010000, // (
  0b0000101000000000, // )
  0b0000111111110000, // *
  0b0000010110100000, // +
  0b0000001000000000, // ,
  0b0000010000100000, // -
  0b0000000000010000, // .
  0b0000001001000000, // /
  0b1110001001000111, // 0
  0b0110000001000000, // 1
  0b1100010000100011, // 2
  0b1110010000100001, // 3
  0b0110010001000100, // 4
  0b1000010000010101, // 5
  0b1010010000100111, // 6
  0b1000010101100000, // 7
  0b1110010000100111, // 8
  0b1110010000100100, // 9

  0b0000000110000000, // :
  0b0000001010000000, // ;
  0b0000001001000001, // <
  0b0000010000100001, // =
  0b0000100000010001, // >
  0b1100000100100100, // ?
  0b1110010100000011, // @ a1
  //0b1100000010100111, // @
  
  0b0000000000000000, // A
  0b1110000110100001, // B a1
  0b1000000000000111, // C
  0b1110000110000001, // D
  0b1000010000100111, // E
  0b1000010000100110, // F
  0b1010000000100111, // G
  0b0110010000100110, // H
  0b1000000110000001, // I
  0b0110000000000011, // J
  0b0000010001010110, // K
  0b0000000000000111, // L
  0b0110100001000110, // M
  0b0110100000010110, // N
  0b1110000000000111, // O
  0b1100010000100110, // P
  0b1110000000010111, // Q
  0b1100010000110110, // R
  0b1010010000100101, // S
  0b1000001100000000, // T
  0b0110000000000111, // U
  0b0000001001000110, // V
  0b0110001000010110, // W
  0b0000101001010000, // X
  0b0000100101000000, // Y
  0b1000001001000001, // Z

  0b1000111000000001, // [
  0b0000100000010000, // \
  0b1000000001110001, // ]
  0b0100000001000000, // ^
  0b0000000000000001, // _
  0b0000100000000000, // `
  
  0b0010001000100001  // a
};



  // Talk to PCF8576
  Wire.beginTransmission( 0x38 );
  // Device select C 1 1 0 0 A2 A1 A0
  Wire.write( 0b11100000 );
  Wire.write( 0b00000000 );

  Wire.write( ((byte*)&s14_d)[0] );
  Wire.write( ((byte*)&s14_d)[1] );
  Wire.write( ((byte*)&s14_f)[0] );
  Wire.write( ((byte*)&s14_f)[1] );
  Wire.write( ((byte*)&s14_o)[0] );
  Wire.write( ((byte*)&s14_o)[1] );
  Wire.write( ((byte*)&s14_o)[0] );
  Wire.write( ((byte*)&s14_o)[1] );
  Wire.write( ((byte*)&s14_h)[0] );
  Wire.write( ((byte*)&s14_h)[1] );
  Wire.write( ((byte*)&s14_k)[0] );
  Wire.write( ((byte*)&s14_k)[1] );
  Wire.write( ((byte*)&s14_a)[0] );
  Wire.write( ((byte*)&s14_a)[1] );
  Wire.write( ((byte*)&s14_K)[0] );
  Wire.write( ((byte*)&s14_K)[1] );
  error = Wire.endTransmission( );
}

/////////////////////////////////////////////////////////////////////////////////////////

void initKeypad( )
{
  byte error;

  // Talk to PCF8574
  Wire.beginTransmission( 0x20 );
  Wire.write( 0b00000000 );
  Wire.write( 0b00001111 );  
  error = Wire.endTransmission( );

  if (error != 0)
  {
    Serial.print( " Error: " );
    Serial.print( error,HEX );
    Serial.println( "." );
  }
}

char readKeypad( )
{
  uint8_t number = 0;
  uint8_t data;
  byte error;
  byte nRow;
  char arrNumbers[ 4 ][ 4 ] = { 
    { '1','2','3','a' },
    { '4','5','6','b' },
    { '7','8','9','c' },
    { '*','0','#','d' }
  };

  keypad = 0;

  for ( nRow = 0; nRow < 4; nRow++ )
  {
    Wire.beginTransmission( 0x20 );
    Wire.write( 0xff & ~(1 << nRow ) );
    error = Wire.endTransmission( );
    Wire.requestFrom( 0x20, 1, true ); 
    data = Wire.read( );
    data = ( (uint8_t)(~data) >> 4 );
    
    // 16 bit keypad assignment
    switch ( data )
    {
      case 0:
        break;

      case 1:
      case 2:
        return arrNumbers[ nRow ][ data - 1 ];
        break;

      case 4:
        return arrNumbers[ nRow ][ 2 ];
        break;

      case 8:
        return arrNumbers[ nRow ][ 3 ];
        break;

      default:
          Serial.println( "cannot decode combined keypresses" );
    }
  }

  Wire.beginTransmission( 0x20 );
  Wire.write( 0xff & ~(1 << ( nRow + 4 ) ) );
  error = Wire.endTransmission( );

  return keypad;
}

/////////////////////////////////////////////////////////////////////////////////////////

void scan( )
{
  byte error, address;
  int nDevices;

  Serial.println( "Scanning..." );

  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission( address );
    error = Wire.endTransmission( );
 
    if ( error == 0 )
    {
      Serial.print( "I2C device found at address 0x" );
      if ( address < 16 )
        Serial.print( "0" );
      Serial.print( address, HEX );
      Serial.println( "  !" );
 
      nDevices++;
    }
    else if ( error==4 )
    {
      Serial.print( "Unknow error at address 0x" );
      if ( address < 16 )
        Serial.print( "0" );
      Serial.println( address, HEX );
    }    
  }

  if ( nDevices == 0 )
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  // wait 5 seconds for next scan
  delay( 5000 );
}

