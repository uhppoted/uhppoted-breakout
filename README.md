# uhppoted-breakout

Breakout board for the UHPPOTE TCP/IP Wiegand 26 access controllers. Replaces the removable CPU board with a
hackable board that exposes the interface connector pins as a header.

## Raison d'être

Mostly curiosity?

But .. it would be nice to have an access controller:
- that is completely open source and customisable to the limits of your ability
- that runs a standard embedded RTOS so that you can do cute and/or clever things with it
- can host something a _RaspberryPi_ or somesuch directly on the base board obviating the need
  for an external host

Also, the manufacturer has stated [categorically](https://github.com/uhppoted/uhppoted/blob/main/documentation/FAQ.md#faq)
that the firmware on the access controllers is not updatable by the end user. Which is quite reasonable but does mean that
older controllers which are otherwise running just fine have to be replaced to get upgraded functionality. And some 
installations have a **lot** of controllers.

## Roadmap

### Rev.0

**IN DEVELOPMENT**

Initial breakout board. Designed to be relatively easy to hand solder and debug - all the components are on the front (top)
of the PCB and it uses a WaveShare RP2040-Zero module.

### Rev.1

### Rev.2

### Rev.3

## Notes

### Footprints
1. The silkscreen footprint for J10 and J12 on the CPU board of the controller is either mirrored or
   using the footprint for a pin header (presumably because the pin and socket headers were swapped for
   easier assembly and the silkscreen was never updated).

2. The silkscreen footprint for J10 and J11 on the base board of the controller is either mirrored or
   using the footprint for a pin socket (presumably because the pin and socket headers were swapped for
   easier assembly and the silkscreen was never updated).

3. The breakout board uses the correct footprints for the J10 and J12 socket headers that connect the
   board to the base board.