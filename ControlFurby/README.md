This is a project info sheet for the MakeFurbyDoIt project shown at OSCON 2014.

## System Concept ##

## Design ##

## Improvements ##
- Show means of authenticating to Jenkins

## Other Design Options Considered ##
 Other options: 
   - set up a proxy on the laptop, push to Arduino sketch - skipped because don't want to install client 
   - set up a chat client here, send from the Jenkins using a plugin?
   - ...?

## Things Learned ##
[String.reserve](https://learn.adafruit.com/memories-of-an-arduino/optimizing-sram)

## Libraries ##
[ArduinoJson](https://github.com/bblanchon/ArduinoJson) - see his [blog post](http://blog.benoitblanchon.fr/arduino-json-parser/) on why he made the library
    * Note: if imported his library directly, go delete the JsonGeneratorTest and JsonParserTest folders, so not attempting to compile against CPPUnit
[aJSON](https://github.com/interactive-matter/aJson) - prefer its manner of pulling from a stream...
[MemoryFree](https://github.com/sudar/MemoryFree) - keep track of free memory
[Arduino F macro](http://www.baldengineer.com/blog/2013/11/20/arduino-f-macro/) - reduce memory used by strings

## Development Environment ##
[Stino Sublime Text Plugin](http://robot-will.github.io/Stino/)
[Sublime Text editor](link)

### TODO ###
- Drive Ethernet from laptop?  Or do I need some other means/mechanism?
- 
### Odd Notes ###
- Unable to use JsonParser within Stino, since it wants to compile against CPPUnit....  But that doesn't seem to matter for the Arduino environment.
    + Updated to remove JsonParserTest from library repository - resolved that issue, however now getting duplicate definition items
    + Was able to upload via Ardunio IDE
-  Desire to add commands to clear Serial Monitor, put in a blank line to separate runs... - mitigated by close / reopen
-  DHCP + aJSON - no bueno.

### Furby wiring notes ###
- Speakers - brown wires, left side of furby, white speaker.  
    - Unplugging removes sound
    - What about potentiometer to control volume from Arduino
- Purple wires, near speakers...  ??
    + Not sound - sound device still works...
    + 
- Red and brown wire pairing, right side - no real movement...
    + eyes still come on...  still talks, but no movement...
        * eyelids, body, etc...
        * without movement, seems to shut itself back down quickly..
    +  guess: brown is motor activation, red is power??
- Light tan wires, near red/brown wire pairing... 
    +  timer to kick back in?  [ABS-5](http://www.mouser.com/ProductDetail/Cooper-Bussmann/ABS-5/?qs=sGAEpiMZZMuMS2dUaCDnDKsZ4y%252bS389n) is a specialty fuse
- 4 prong items...
    + keeps going
    + some sort of randomizing thing???
    + 
