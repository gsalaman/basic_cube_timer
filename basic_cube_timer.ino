/* Basic Cube Timer */



#include <LiquidCrystal.h>
/*================
 * LCD CONNECTIONS:  (note...we're using 4 bit mode here...)
 *   1 to GND
 *   2 to 5V
 *   3 to the contrast control...I did a hardcoded voltage divider.
 *   4 to Arduino digital pin LCD_REG_SEL
 *   5 to GND
 *   6 to Arduino digital pin LCD_ENABLE
 *   7 (no connection)
 *   8 (no connection)
 *   9 (no connection)
 *   10 (no connection)
 *   11 to Arduino  digital pin LCD_D4
 *   12 to Arduino  digital pin LCD_D5
 *   13 to Arduino  digital pin LCD_D6
 *   14 to Arduino  digital pin LCD_D7
 *   15 to 5V
 *   16 to GND
 *====================*/
#define LCD_D7         4 
#define LCD_D6         5
#define LCD_D5         6
#define LCD_D4         7
#define LCD_ENABLE     8
#define LCD_REG_SEL    9

// Our LCD has 2 rows of 16 characters.
#define LCD_CHARS 16
#define LCD_ROWS 2

LiquidCrystal lcd(LCD_REG_SEL, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

#define RH_PIN    10
#define LH_PIN    11
#define RESET_PIN 12

uint32_t start_time;

typedef enum
{
  STATE_RESET,
  STATE_ARMED,
  STATE_TIMING,
  STATE_STOPPED
} state_type;

state_type current_state = STATE_RESET;

/* State initalization forward declarations */
void init_reset_state( void );
void init_armed_state( void );
void init_timing_state( void );
void init_stopped_state( void );

/* State processing function forward declarations */
/* Note:  still returning next state, even though right now, our state machine always 
 *  knows what the next state will be.  Maybe this will change in the future...
 */
state_type process_reset_state( void );
state_type process_armed_state( void );
state_type process_timing_state( void );
state_type process_stopped_state( void );

typedef void (*init_func_type)(void);
typedef state_type (*proc_func_type)(void);

/* init function table */
init_func_type init_func[] = 
{
  init_reset_state,
  init_armed_state,
  init_timing_state,
  init_stopped_state
};

/* processing function table */
proc_func_type proc_func[] = 
{
  process_reset_state,
  process_armed_state,
  process_timing_state,
  process_stopped_state
};

/*=================================================================
 * display text
 * 
 * This is a wrapper function for text display.
 * Put text out to both serial and top line of the LCD display.
 =================================================================*/
void display_text(char *text)
{
  Serial.println(text);
  lcd.setCursor(0,0);
  lcd.print(text);
}

/*=================================================================
 * display text - overload
 * 
 * This is a wrapper function for text display.
 * Put text out to both serial and text at the specified row/column of the lcd
 =================================================================*/

void display_text(char *text, int row, int col)
{
  Serial.println(text);
  lcd.setCursor(col,row);
  lcd.print(text);
}

/*=================================================================
 * display_elapsed_time
 * 
 * This function displays the current elapsed time from the previous start.
 * Note we use the global "start_time" to do this calc.
 =================================================================*/
void display_elapsed_time( void )
{
  uint32_t current_time;
  uint32_t elapsed_time;
  uint32_t elapsed_sec;
  uint32_t elapsed_ms;

  current_time = millis();

  elapsed_time = current_time - start_time;
  elapsed_sec = elapsed_time/1000;
  elapsed_ms = elapsed_time % 1000;

  /* Serial print portion */
  Serial.print("sec: ");
  Serial.print(elapsed_sec);
  Serial.print(" ms: ");
  Serial.println(elapsed_ms);

  /* LCD version goes on second line */
  lcd.setCursor(0,1);
  lcd.print(elapsed_sec);
  lcd.print(".");
  lcd.print(elapsed_ms);
}  /* end of display_elapsed_time */

/*=================================================================
 * init_reset_state
 =================================================================*/
void init_reset_state( void )
{
  lcd.clear();
  display_text("  Press both");
  display_text("buttons to arm", 1, 0);
  
}  /* end of init_reset_state */

/*=================================================================
 * init_armed_state
 =================================================================*/
void init_armed_state( void )
{
  lcd.clear();
  display_text("Ready...");
  
}  /* end of init_armed_state */

/*=================================================================
 * init_timing_state
 =================================================================*/
void init_timing_state( void )
{
  start_time = millis();
  display_elapsed_time();
  
}  /* end of init_timing_state */


/*=================================================================
 * init_stopped_state
 =================================================================*/
void init_stopped_state( void )
{

  // One last update...
  display_text("reset 2 go again");
  display_elapsed_time();
  display_text("=your time",1,6);
  
}  /* end of init_stopped_state */

/*=================================================================
 * process_reset_state
 * 
 * Wait for both LH and RH buttons to be pressed.  This will move us
 * to "armed".
 =================================================================*/
state_type process_reset_state( void )
{
  int rh_pin;
  int lh_pin; 

  rh_pin = digitalRead(RH_PIN);
  lh_pin = digitalRead(LH_PIN);
  
  //#if 0
  Serial.print("pins: ");
  Serial.print(rh_pin);
  Serial.print(" ");
  Serial.println(lh_pin);
  //#endif
  
  if ((rh_pin == LOW) && (lh_pin == LOW))
  {
    return(STATE_ARMED);
  }
  else
  {
    return(STATE_RESET);
  }
  
}  /* end of process_reset_state */

/*=================================================================
 * process_armed_state
 * 
 * Once both buttons are released, we'll start timing.
 =================================================================*/
state_type process_armed_state( void )
{
  int rh_pin;
  int lh_pin;

  rh_pin = digitalRead(RH_PIN);
  lh_pin = digitalRead(LH_PIN);

  //#if 0
  Serial.print("pins: ");
  Serial.print(rh_pin);
  Serial.print(" ");
  Serial.println(lh_pin);
  //#endif
  
  if ((digitalRead(RH_PIN) == HIGH) && (digitalRead(LH_PIN) == HIGH))
  {
    return(STATE_TIMING);
  }
  else
  {
    return(STATE_ARMED);
  }
  
}  /* end of process_armed_state */

/*=================================================================
 * process_timing_state
 * 
 * In here, we're counting time, until both buttons are pressed again.
 =================================================================*/
state_type process_timing_state( void )
{
  if ((digitalRead(RH_PIN) == LOW) && (digitalRead(LH_PIN) == LOW))
  {
    return(STATE_STOPPED);
  }
  else
  {
    display_elapsed_time();
    return(STATE_TIMING);
  }
  
}  /* end of process_timing_state */


/*=================================================================
 * process_stopped_state
 * 
 * Just waiting for the reset button to be pressed for half a second.
 =================================================================*/
state_type process_stopped_state( void )
{ 
  /* Okay, I'm gonna be bad here.  Instead of a proper debounce, I'm going to pull
   *  out the sledgehammer and use "delay".  
   */
  if (digitalRead(RESET_PIN) == LOW)
  {
    delay(500);
    if (digitalRead(RESET_PIN) == LOW)
    {
      return(STATE_RESET);
    }
  }
  return(STATE_STOPPED);
  
}  /* end of init_stopped_state */

void setup( void )
{
  Serial.begin(9600);

  pinMode(RH_PIN, INPUT_PULLUP);
  pinMode(LH_PIN, INPUT_PULLUP);
  pinMode(RESET_PIN, INPUT_PULLUP);

  lcd.begin(LCD_CHARS, LCD_ROWS);
  lcd.clear();
  
  init_reset_state();
  
}

void loop ( void )
{
  state_type next_state;

  next_state = proc_func[current_state]();
  if (next_state != current_state)
  {
    init_func[next_state]();
    current_state = next_state;
  }
  
}
