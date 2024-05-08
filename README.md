# uhppoted-breakout

Breakout board for the UHPPOTE TCP/IP Wiegand 26 access controllers.


## Raison d'être

Mostly curiosity? But it would be nice to have an access controller that is completely open source and
customisable to the limits of your ability and that perhaps runs a standard embedded OS so that you 
can do cute and/or clever things with it. And being able to run a RaspberryPi or somesuch directly on
the base board opens up a world of possibilities - to the least of which is no longer needing an external
host for all the uhppoted stuff.

Also - as per the manufacturer, the firmware on the access controllers is not updatable by the end user
so once you've bought a board you're stuck with that version till the end of time. Well, at least until
the onboard calendar runs out and the SDK suggests that the dates supported by the controllers may only
go up to about 2027 (at least in older controllers - presumably newer controllers have that date pushed
out a bit further).


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