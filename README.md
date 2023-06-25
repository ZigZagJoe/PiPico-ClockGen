# PiPico-ClockGen
RP2040 chip supports outputting internal clocks such as system clock to GPIOs. It also supports dynamic frequency changes with a surprising amount of granularity. This is a simple sketch using the RP2040 arduino core and a pi pico to generate clock signals for debugging or overclocking.

provided UF2 file defaults to a frequency of 50mhz. Flash it, connect to the com port, and enter new clock frequencies. easy enough.

Output pin is GP21 (this cannot be changed). Output signal will be 3.3v which should work fine with TTL logic.
