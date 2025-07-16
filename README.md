# General-Purpose-Input-Output-GPIO-Interfaces
> STM32F14

In order to realize this laboratory, we need to implement a timer, so we can count the time elapsed
while the user pushes the botton.
For this laboratory, we decided to use the TIM2.
To implement this timer, we see in the block diagram that we need the enable the APB1 clock.

<img width="582" height="706" alt="Captura de pantalla 2025-07-15 a la(s) 11 41 13 p m" src="https://github.com/user-attachments/assets/072f938f-b808-44ff-91ec-2675a27d8cab" />

We will also use the ports C and D from the GPIO of the board, because the button is located in
PC_15, and the LEDs are located in PD_13, PD_14 and PD_15. In order to do this, we need to enable
the clock AHB1, as we can see in the diagram.

<img width="586" height="354" alt="Captura de pantalla 2025-07-15 a la(s) 11 41 49 p m" src="https://github.com/user-attachments/assets/491fface-5b11-4881-8e14-6d69ffa32b1c" />

With this timer, we can count the time elapsed, or generate and interruption when a certain amount
of time is reached. In this case, we want to count the time elapsed, that’s why in the register CCER
(capture/compare enable register) we enabled the register to work in the capture mode.
This timer works as follow, when a certain amount of time has passed in the main clock, the counter
increases by 1. We can define this amount of time in the register PSC.

The logic of the code is as follow:
  • We wait until the button is pressed.
  • We start the counter and read its present value.
  • Wait until the button is released.
  • Read the present value from the counter.
  • Compute the elapsed time, as current value – previous value.
    o If the elapsed time is smaller than 500ms, we add 1 to the counter.
    o If the elapsed time is bigger, we subtract 1 to the counter.


As a result, when the button is pressed fast, the counter increases by 1, and the LEDs start turning
on making a binary counter as in the next figure:

<img width="211" height="259" alt="Captura de pantalla 2025-07-15 a la(s) 11 42 57 p m" src="https://github.com/user-attachments/assets/e45c579b-a50e-4700-b793-0b7c60df232f" />

When the 3 is reached, as we don’t have more LEDs to continue the counting, we get an overflow.
Every time we continue adding one, we will see an animation, both LEDs will blink the number of
times that we have reached the overflow.
The counter also works in the opposite side. When we press the button for a longer time, the
counter subtract 1. When we reach the 0 and continue subtracting, we get an underflow, and again
we see an animation of the LED blinking for each time we have reached the underflow.
