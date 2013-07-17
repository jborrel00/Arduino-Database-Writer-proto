Arduino-Database-Writer-proto
=============================

--Takes a python hex serial output and stores it in an array, to be used to write info onto a nfc card
====================================================================================================

This program works in conjuction with the pydatabsemanager_test.py file. Which looks through the MySQL database 
'foosball' for the 'test' entry and its respective hex code. That program then takes that information and serially 
sends it to the arduino. The arduino code then stores test's hex code in a data array. The goal is to then include this 
code snippet in the CODE.ino program that is currently used to write hex data onto card. This way, you would not have to 
enter the code manually into the arduino code every time you add a new player, you would only have to include them in 
the MySQL database and they would then be added automatically to the arduino code and coded onto their own card.
