
Doxygen Reference
==================

Linduino documentation is automatically generated from the source code using
Doxygen. Refer to the following templates when documenting code.  

\b %DoxygenExample.ino:

[Source Code](file:./DoxygenExample_8cpp_source.html "Source Code")

[Doxygen Generated Output](file:./DoxygenExample_8cpp.html "Doxygen Generated Output")

\b %DoxygenExample.h:

[Source Code](file:./DoxygenExample_8h_source.html "Source Code")

[Doxygen Generated Output](file:./DoxygenExample_8h.html "Doxygen Generated Output")

\b %DoxygenExample.cpp:

[Source Code](file:./DoxygenExample_8cpp_source.html "Source Code")

[Doxygen Generated Output](file:./DoxygenExample_8cpp.html "Doxygen Generated Output")

Store the .ino file in a folder with the same name (i.e. "DoxygenDemo.ino" is
stored in a folder named "DoxygenDemo"). The folder may be stored in
LTSketchbook or in any folder not labeled as Libraries. 

If the .h and .cpp files will only be used by one .ino program, store them in the
same folder with the .ino file.  For example, "DoxygenExample.cpp" and
"DoxygenExample.h" are stored in the "DoxygenDemo" folder. Alternatively, if the
files form a library to be used by other .ino files, store the "DoxygenExample.h"
and "DoxygenExample.cpp" files in the Libararies folder of the LTSketchbook. 

Documenting the .ino
--------------------
<b>The .ino file must include the following lines, where "groupname" is replaced by a
descriptive name used to group the .ino, .cpp, and .h files.</b>

@verbatim

/*! @file
    @ingroup groupname
*/
@endverbatim

Documenting a function with parameters and no return:

@verbatim

//! Message that describes the function
//! @return void
void function1(int param1,      //!< Message that describes the function of the variable
               float param2,    //!< Message that describes the function of the variable
               char param3      //!< Message that describes the function of the variable
              )
{
}

@endverbatim
Click \ref function1() "here" to see the output.

Documenting a function with no parameters and a return:

@verbatim

//! Message that describes the function
//! @return Describe the return conditions
float function2()
{
}
@endverbatim
Click \ref function2() "here" to see the output.


Documenting for the C++ and Header File
------------------------------------------
<b>The header file (*.h) must include the following lines, where "groupname" is 
replaced by a descriptive name used to group the .ino, .cpp, and .h files.</b>

@verbatim

/*! @file
    @ingroup groupname
    Header for Description_of_the_header
*/

@endverbatim

<b>The C++ file (*.cpp) must include the following lines, where "groupname" is replaced by a
descriptive name used to group the .ino, .cpp, and .h files. "categoryname" is replaced by one
of the categories shown the table below. </b>

@verbatim

/*! @file
    @ingroup categoryname
    @{  
    @defgroup groupname Doxygen Documentation Example
    @}
    @ingroup groupname
    Library for Description_of_the_header
*/

@endverbatim

categoryname                 | Description
---------------------------- | -------------------------------------------------------------------------------------
Linduino                     | Linduino: Linear Technology Arduino-Compatible Demonstration Board Support Libraries
Analog_to_Digital_Converters | Analog-to-Digital Converters (ADC)
Digital_to_Analog_Converters | Digital-to-Analog Converters (DAC)
Hot_Swap                     | Hot Swap
Power_Monitors               | Power Monitors and Coulomb Counters
Power_System_Management      | Digital Power System Management and Power Supply Supervisors
Switching_Regulators         | Switching Regulators, Battery Chargers, and LED Drivers
Transceivers                 | Transceivers
Temperature_Monitors         | Temperature Monitors
RF_Timing                    | RF, Silicon Oscillators, PLL Synthesizers, and VCOs
BMS                          | Battery Management System (BMS)
PMBus_SMBus                  | PMBus and SMBus Support Libraries
Example_Designs              | Example Designs
User_Contributed             | User Contributed Libraries (Not Supported by Linear Technology) 
Third_Party                  | Third-Party Libraries (Not Supported by Linear Technology)
Documentation                | Documentation
WIP                          | Work in Progress

<b>It is recommended that the Doxygen documentation for functions be placed in the
header file (*.h) and not the C++ file (*.cpp).</b>
 
Documenting a function with no parameters and no return:
 
@verbatim

//! Message that describes the main function
//! @return void
void function3();

@endverbatim
Click \ref function3() "here" to see the output.

Documenting a function with parameters and a return:

@verbatim

//! Message that describes the main function
//! @return Describe the return conditions
int function4(int var1,     //!< Message that describes the function of the variable
              float var2    //!< Message that describes the function of the variable
              );
              
@endverbatim
Click \ref function3() "here" to see the output.

<b> Note: If the comment that describes the variable is placed after the semicolon, Doxygen
will not document the last variable. Be sure to comment the variables before the semicolon.</b>

Documenting Global Variables, Constants, Defines, and Enum
----------------------------------------------------

@verbatim
static float float_var; //!< Message that describes the function of the variable
const int int_var;      //!< Message that describes the function of the constant variable
#define var             //!< Message that describes the function of the define

//! Enum Description
enum EnumType
    {
      int EVal1,     /**< enum value 1 */
      int EVal2      /**< enum value 2 */
    };
@endverbatim

Adding a Title
---------------
@verbatim
/*! @name TITLE
@{
    The section that receives the title.
@}
*/
@endverbatim

Making Tables
-------------

@verbatim
First Header  | Second Header
------------- | -------------
Content Cell  | Content Cell 
Content Cell  | Content Cell 
@endverbatim

First Header  | Second Header
------------- | -------------
Content Cell  | Content Cell 
Content Cell  | Content Cell

Column alignment can be controlled via one or two colons at the header separator line:

@verbatim
| Right | Center | Left  |
| ----: | :----: | :---- |
| 10    | 10     | 10    |
| 1000  | 1000   | 1000  |
@endverbatim


| Right | Center | Left  |
| ----: | :----: | :---- |
| 10    | 10     | 10    |
| 1000  | 1000   | 1000  |

Making Lists
------------
@verbatim
- Item 1

  More text for this item.

- Item 2
  + nested list item.
  + another nested item.
- Item 3
@endverbatim

- Item 1

  More text for this item.

- Item 2
  + nested list item.
  + another nested item.
- Item 3

Links
------
@verbatim
[The link text](http://www.linear.com/ "Link title")
@endverbatim
[The link text](http://www.linear.com/ "Link title")

Images
-------
@verbatim
![Caption text](/path/to/img.jpg "Image title")
@endverbatim

Emphasis
---------
@verbatim
_single underscores_
@endverbatim
_single underscores_

Bold
-----
@verbatim

\b word

<b> more than one word </b>

@endverbatim
\b word

<b> more than one word </b>

Adding a bug to the bug list
--------------------------------
Add the following comment above the function with the bug
@verbatim

//! @bug  bug description

@endverbatim

More Information
----------------
For more information please refer to the Doxygen manual in the [Doxygen website](http://www.doxygen.org/ "Doxygen")

@defgroup Linduino Linduino: Linear Technology Arduino-Compatible Demonstration Board Support Libraries
@defgroup Analog_to_Digital_Converters Analog-to-Digital Converters (ADC)
@defgroup Digital_to_Analog_Converters Digital-to-Analog Converters (DAC)
@defgroup Hot_Swap Hot Swap
@defgroup Power_Monitors Power Monitors and Coulomb Counters
@defgroup Power_System_Management Digital Power System Management and Power Supply Supervisors
@defgroup Switching_Regulators Switching Regulators, Battery Chargers, and LED Drivers
@defgroup Transceivers Transceivers
@defgroup Temperature_Monitors Temperature Monitors
@defgroup RF_Timing RF, Silicon Oscillators, PLL Synthesizers, and VCOs
@defgroup BMS Battery Management System (BMS)
@defgroup PMBus_SMBus PMBus and SMBus Support Libraries
@defgroup Example_Designs Example Designs
@defgroup User_Contributed User Contributed Libraries (Not Supported by Linear Technology) 
@defgroup Third_Party Third-Party Libraries (Not Supported by Linear Technology)
@defgroup Documentation Documentation
@defgroup WIP Work in Progress