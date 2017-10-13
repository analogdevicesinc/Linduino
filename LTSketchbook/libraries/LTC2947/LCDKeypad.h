/*
  LCDKeypad.h
  */

// ensure this library description is only included once
#ifndef LCDKeypad_h
#define LCDKeypad_h

// library interface description
#define KEYPAD_NONE -1
#define KEYPAD_RIGHT 0
#define KEYPAD_UP 1
#define KEYPAD_DOWN 2
#define KEYPAD_LEFT 3
#define KEYPAD_SELECT 4

//! @ingroup Third_Party
//! @{
//! @defgroup PRINT_MACROS LCD serial print macros
//! @}

/**  Macros used to print simultaneously on LCD and serial port
*  @{
*/
/** \brief  writes degree C sign */
#define WRITE_DEGREE_C() \
      { if (lcd->foundLCD)               {lcd->write((uint8_t)0); lcd->print('C');} \
    if (lcd->EnableSerialOutput) {Serial.write((uint8_t)176); Serial.print('C');} }
/** \brief  clear screen */
#define CLEAR_SCREEN() {  if (lcd->foundLCD) { \
    lcd->clear(); \
    lcd->display(); \
    lcd->setCursor(0, 0);} \
    if (lcd->EnableSerialOutput) Serial.println(); }
/** \brief  print newline */
#define NEWLINE() { if (lcd->foundLCD) lcd->setCursor(0, 1); if (lcd->EnableSerialOutput) Serial.println();  }
/** \brief  print value / text
\param  A  value / text to be written */
#define PRINTSCREEN(A) { if (lcd->foundLCD) lcd->print(A); if (lcd->EnableSerialOutput) Serial.print(A);   }
/** \brief  print formatted value
\param  A  value
\param  B  formatting argument */
#define PRINTSCREEN2(A,B) { if (lcd->foundLCD) lcd->print(A,B); if (lcd->EnableSerialOutput) Serial.print(A,B); }
/** \brief  move cursor
\param  A  column
\param  B  row */
#define SCREENSETCURSOR(A,B) { if (lcd->foundLCD) lcd->setCursor(A,B); if (lcd->EnableSerialOutput) Serial.print(' '); }
/** @} */ // end of PRINT_MACROS


class LCDKeypad : public LiquidCrystal
{
  public:
    LCDKeypad(uint8_t ledCtrlPin);
    int button();
    void searchForLCD();
    void Disable4BitLCD();
    void LEDon();
    void LEDoff();
    static uint8_t degree[8];

    boolean foundLCD = false;
    boolean EnableSerialOutput;
  private:
    uint8_t _ledCtrlPin;
};

#endif



