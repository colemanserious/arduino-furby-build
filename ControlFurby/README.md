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

## Libraries ##
[aJSON](https://github.com/interactive-matter/aJson) - 
[MemoryFree](https://github.com/sudar/MemoryFree) - keep track of free memory
[Arduino F macro](http://www.baldengineer.com/blog/2013/11/20/arduino-f-macro/) - reduce memory used by strings
[Ethernet](arduino.cc/en/reference/ethernet) - set up client and server
[TMRpcm](https://github.com/TMRh20/TMRpcm/) - used to play wav files from an SD card
[ArduinoJson](https://github.com/bblanchon/ArduinoJson) - see his [blog post](http://blog.benoitblanchon.fr/arduino-json-parser/) on why he made the library
    * Note: if imported his library directly, go delete the JsonGeneratorTest and JsonParserTest folders, so not attempting to compile against CPPUnit
    * Opted to go with aJSON

## Development Environment ##
[Stino Sublime Text Plugin](http://robot-will.github.io/Stino/)
[Sublime Text editor](www.sublimetext.com)
 
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
