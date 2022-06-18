# Basic Cubing Timer

This is my first step in cubing timer creation.  This version will be a basic prototype to create the functionality of a basic "speed-stacks-style" timer.

Interface:  We'll have two buttons for left and right hand, a "reset" button, and an LCD screen for status.

Functionality:
When in "reset" state, the status screen will read "waiting".

Pressing the reset button here will do nothing.

Having both the left and right buttons pressed for half a second (TBR) will move us to the "armed" state.

In armed state, we're waiting for the buttons to be released.  Either button being released will move us to "timinig" state.

Note:  could do this so that we only go to counting when *BOTH* are released...but I like catching the start motion to prevent one handed solves.

In counting state, we display the time increasing.  Fractional seconds are good.  We're waiting for both hands to be placed again.

Once both buttons are down, we go to "stopped" state, and display the final time.
In stopped, a press of the reset button for half a second (TBR) will clear the time and move us back to "reset" state.
