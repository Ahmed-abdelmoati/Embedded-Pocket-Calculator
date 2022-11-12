Project Name: Embedded Calculator

Team members:

احمد عبد المعطي محمد

كريم عصام الدين سيد

محمد مصطفى ثابت

Main components:
- Atmega32 with Keypad &LCD
Description:
Embedded Calculator is a simple calculator that performs the
main mathematical operations, the expression is entered using the
keypad and displayed on the LCD then the result is written on the
LCD after pressing “=" on keypad.

features:
• Support binary operators +, -, *, /, %, and ^ (on integers only).

• It can deal with integers and float numbers.

• Support complex expressions

• Support nested parentheses

• Support unary operators -, +

Operation:

• Expression is entered character by character using keypad and
stored as a string, the LCD prints the inputs until "=" button is
pressed, when "=" is pressed the input expression -infix exp- is
converted to postfix one and calculations are made on it then the
result is converted back to string by ftoa function and printed on
screen.
