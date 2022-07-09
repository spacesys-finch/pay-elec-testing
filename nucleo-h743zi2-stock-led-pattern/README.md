# Stock LED Pattern

- on release of user button, switch the blinking LED from green -> yellow -> red.
- green LED flashes every 1s, yellow flashes every 0.5s, red flashes every 0.25s

## references
- https://www.youtube.com/watch?v=VfbW6nfG4kw
    - DigiKey STM32 tutorial series by Shawn Hymel

## Friday, July 8, 2022 - Yong Da
- working for non-flashing
    - used a single timer (timer16) which generates an interrupt every 0.25 seconds
    - then used that timer tick to update a timer tick variable
    - also keep track of a button press tick variable
    - depending the state of that the 2 variables (timer tick and button press), we toggle on/off the appropriate light


- HCLK 64MHz (system clock) -> /2 *2 pre-scalers -> APB2 timer clocks
    - https://community.st.com/s/question/0D53W00000mjv1LSAQ/what-is-the-internal-clock-ckint-for-the-stm32h7-timers
    - H7 reference manual (RM0433): 8.7.56 RCC APB2 Sleep Clock Register (RCC_APB2LPENR)


- configured timer16 to have its own prescaler of 6,400
    - count period of 10,000
    - set to generate global interrupt
    - effectively triggers interrupt once every 0.25s

- https://photos.app.goo.gl/wxJ9mFvdx8sUWkn17

## Sunday, July 3, 2022 - Yong Da
- Got it working for non-flashing
- I think I need interrupts to get the flashing functionality working
- indian guy: https://www.youtube.com/watch?v=qd_tevhJ2eE
	- need some setup
- DigiKey Shawn Hymel - https://www.youtube.com/watch?v=VfbW6nfG4kw
	- more about timer interrupts

- super loop is blinking LED according to state variable
- user button press triggers interrupt, which updates state variable
- in the ISR, probably need to turn off all LEDs and enter the switching function of the new state
	- otherwise we'd have to wait until the old switching function cycles back to the "check for updated state variable" state
