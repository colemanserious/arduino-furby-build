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

## Development Environment ##
[Stino Sublime Text Plugin](http://robot-will.github.io/Stino/)
[Sublime Text editor](link)

### TODO ###
- Drive Ethernet from laptop?  Or do I need some other means/mechanism?
- [Arduino F macro?](http://www.baldengineer.com/blog/2013/11/20/arduino-f-macro/)
- 
### Odd Notes ###
- Unable to use JsonParser within Stino, since it wants to compile against CPPUnit....  But that doesn't seem to matter for the Arduino environment.
    + Updated to remove JsonParserTest from library repository - resolved that issue, however now getting duplicate definition items
-  Desire to add commands to clear Serial Monitor, put in a blank line to separate runs