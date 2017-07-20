# attys_scope
## UDP receiver for UNREAL

![alt tag](unreal_editor_screenshot.png)

With the help of this C++ class you can
create an Attys outlet which provides you
with all filtered / visible channels
in AttysScope.

## Installation

 * Copy the cpp and header file in the source directory of your project.
 * Create a blueprint from it
 * You should see an Attys outlet

## Usage

 * Just make sure UDP broadcast is enabled in AttysScope and the port
   is 65000 (default).
 * Select the sensors you'd like to see
 * They will show up in the order you've selected them in UNREAL
 