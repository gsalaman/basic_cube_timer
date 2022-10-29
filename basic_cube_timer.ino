/* Basic Cube Timer */

#define RH_PIN    7
#define LH_PIN    8
#define RESET_PIN 9

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
 * For starters, I'm just going to use the serial port,
 * but eventually I'll put this on the LCD.
 =================================================================*/
void display_text(char *text)
{
  Serial.println(text);
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
  elapsed_ms = elapsed_time - elapsed_sec;

  /* Serial print for now...eventually going to LCD. */
  Serial.print(elapsed_sec);
  Serial.print(".");
  Serial.println(elapsed_ms);
}  /* end of display_elapsed_time */

/*=================================================================
 * init_reset_state
 =================================================================*/
void init_reset_state( void )
{
  display_text("Waiting...press both buttons to arm");
  
}  /* end of init_reset_state */

/*=================================================================
 * init_armed_state
 =================================================================*/
void init_armed_state( void )
{
  display_text("Ready...release to begin");
  
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
  Serial.println("Done!!!");
  display_elapsed_time();
  Serial.println("press reset to go again");
  
}  /* end of init_stopped_state */

/*=================================================================
 * process_reset_state
 * 
 * Wait for both LH and RH buttons to be pressed.  This will move us
 * to "armed".
 =================================================================*/
state_type process_reset_state( void )
{
  if ((digitalRead(RH_PIN) == LOW) && (digitalRead(LH_PIN) == LOW))
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

  #if 0
  Serial.print("pins: ");
  Serial.print(rh_pin);
  Serial.print(" ");
  Serial.println(lh_pin);
  #endif
  
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
