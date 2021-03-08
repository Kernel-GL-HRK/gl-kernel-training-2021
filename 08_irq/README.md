# Homework Interrupt handling

## Overwiew

The BBB board has four LEDs located next to the RESET and POWER buttons. Also, the board has one user button located next to the uSD socket.

On-board LESs

*  LED0 (D2):	`GPIO1_21`	"Heartbeat"
*  LED1 (D3):	`GPIO1_22`	"uSD access"
*  LED2 (D4):	`GPIO1_23`	"Active"
*  LED3 (D5):	`GPIO1_24`	"eMMC access"


The user button (`GPIO2_8`) may be used at boot stage to select eMMC or uSD boot and is not used in Limux kernel.
The uSD and MMC access LEDs are not used in nfs boot mode, so we can also use them.

In this homework we use legacy GPIO acces as simpler one because the main goal is use of timers and interrupts.
See the `onboard_io_demo` directory which contain a very simple module that tests access to the button and LEDs. Build and run it. 

Note that the uSD and eMMC LEDs are not used in the nfs boot mode, but are already requested by the correspondent drivers. If we call `gpio_request ()` for these GPIOs, we get an error.
So, we do not use `gpio_request ()` and `gpio_free ()` here, but control these LEDs (___dirty!___).
Use request / free if you want to run a free GPIO aimed at the connectors on the board.

## Homework tasks
1. Implement a `gpio_poll` module which uses periodic jiffies-based timer to poll the button state
  * Check the button state in 20 ms period. Use a simple debouncing method by comparing current and previous button state. If button state does not change, use it as a filtered state. On LED1 if the button is pressed and off otherwise.
  * If the filtered state changes, issue a pulse on LED3 output for 1 ms. Use high resolution timer.
2. implement a `gpio_irq` module which uses interrupts to detect button state changes.
  * Enable hardware debouncing on the button GPIO line (see Documentation/driver-api/gpio/legacy.rst).
  * Use a threaded interrupt, enable both edges.
    * Hardware handler must toggle LED1, increment 32-bit button state change counter and wake the threaded part.
    * The `thread_fn` must read the counter and print its state. 
   Note: Such access to variable from two code streams is a subject to race condition which will be covered later. The access is safe here as we have one writer and one reader and 32-bit access is atomic.
  * Add module parameter `simulate_busy` which enables simulation of long-time work in the `thread_fn`(sleep for several seconds). Use LED3 to indicate busy state. Print a message after the sleep ends.
    * Press the button several times during the sleep, check module output.
    * Try to remove the module just after button is pressed.
