# Stock LED Pattern

- on release of user button, switch the blinking LED from green -> yellow -> red.
- green LED flashes every 1s, yellow flashes every 0.5s, red flashes every 0.25s

## Sunday, July 3, 2022 - Yong Da
- Got it working for non-flashing
- I think I need interrupts to get the flashing functionality working
- indian guy: https://www.youtube.com/watch?v=qd_tevhJ2eE
	- need some setup
- DigiKey Shawn Hymel - https://www.youtube.com/watch?v=VfbW6nfG4kw
	- more about timer interrupts

- super loop is blinking LED according to state variable
- user button press triggers interrupt, which updates state variable