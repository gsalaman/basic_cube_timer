# Reverse Engineering the Stackmat

Put the mic cable on the scope.  The 3.5mm is TRS:
* S=GND
* T=signal

The signal is a 1200 baud N-8-1 digital stream.  Looks like most of the time we get:  
0x49 <D1> <D2> <D3> <D4> <D5> <D6> 0x4E 0x0A 0x0D

The time is in fixed point ascii, with the decimal point between D3 and D4.
  
Gotta now check out 0x49 and 0x4e to see if they change based on the timer state.
  
49 seems to be consistent.  
4E goes to something weird and undecodable when stopped.

1st word seems to go to 0x20 when running.  Stays 0x49 when stopped.
  
