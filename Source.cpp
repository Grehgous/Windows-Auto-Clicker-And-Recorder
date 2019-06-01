/*
TODO:
-Input validation
-Way to go back to config step without restarting progtam
-Save config to file
-Show click position on screen with number in sequence
-Remove globals


KNOWN ISSUES:
- ? Doesn't click until time to move to next position
-During a sleep, key presses aren't registered

*/

#include <iostream>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <chrono>
#include "wtypes.h"
#include <vector>
#include <iterator>

using namespace std;
using namespace System; 
using namespace System::Windows::Forms;

struct Click {
	POINT point;
	int sleep;
};

int constSleepBetweenClicks;
bool uniqueSleepBetweenClicks;
vector<Click> clickList;

void performMovementAndClick(POINT);
void goThroughClickSequence();
void initialSetup();
void displayMenu();
void mainLoop();
void configConsole();
void checkForKeyPress();



// Moves mouse to proper location, and click
// TODO: Take in point type
void performMovementAndClick(POINT p) {

	SetCursorPos(p.x, p.y);

	mouse_event(MOUSEEVENTF_LEFTDOWN, p.x, p.y, 0, 0);	// Mouse click
	mouse_event(MOUSEEVENTF_LEFTUP, p.x, p.y, 0, 0);	// Mouse release

	return;
}

// Move mouse randomly by a few pixels
void goThroughClickSequence() {

	int i = 0;

	// Perform each click and sleep
	for (std::vector<Click>::iterator it = clickList.begin(); it != clickList.end(); ++it, i++) {
		performMovementAndClick(clickList[i].point);
		
		// TODO: Next work period instead of sleep?
		Sleep(clickList[i].sleep);
	}

	return;
}

// Get user input required to configure program
void initialSetup() {
	
	// TODO: Validate input.
	// TODO: Default sleep time
	// TODO: Export/import settings
	// TODO: Remove "press any key"

	int temp = 0;
	char input;


	std::cout << "Unique sleep times? (y/n): ";
	std::cin >> input;

	// Ask the user for a sleep each time, or use the same
	if (input == 'y') {
		uniqueSleepBetweenClicks = true;
	} else {
		// Sleep times are constant after each click
		std::cout << "Constant sleep time between clicks in ms: ";
		std::cin >> constSleepBetweenClicks;
	}


		// Menu
		std::cout << "***********************************" << endl;
		std::cout << "* 'ENTER' to set mouse position   *" << endl;
		std::cout << "* 'ESC' when done                 *" << endl;
		std::cout << "***********************************" << endl << endl;

		// Add new clicks to the sequence
		while (GetAsyncKeyState(VK_ESCAPE) == 0) {
			Click click;

			// Wait for next keypress
			system("PAUSE");

			// Escape keypress ends adding new clicks
			if (GetAsyncKeyState(VK_ESCAPE) != 0) {
					return;
			
			} else if (GetAsyncKeyState(VK_RETURN) != 0) {		// TODO: This does not wait for enter. Works for any key
				// Set the click position
				GetCursorPos(&click.point);
				std::cout << "Position set to (" << click.point.x << "," << click.point.y << ") " << endl;

				if (uniqueSleepBetweenClicks) {
					std::cout << "Sleep time in ms: ";
					std::cin >> click.sleep;

				} else { 
					click.sleep = constSleepBetweenClicks;
				}

				// Add to the list
				clickList.push_back(click);
			}
		}

	return;
}


// Prints the options
void displayMenu() {
	// Menu
	std::cout << "***********************************" << endl;
	std::cout << "* 'F9' to pause                   *" << endl;
	std::cout << "* 'F11' to close program          *" << endl;
	std::cout << "***********************************" << endl << endl;
}

// Make the console loop pretty
void configConsole() {

	SetConsoleTitle("AutoClicker");											// Window title
	Console::SetWindowSize(40, 15);											// Window size
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);			// Font color
	SetLayeredWindowAttributes(GetActiveWindow(), NULL, 100, LWA_ALPHA);	// Opacity 0-255
}


// See if key has been pressed
void checkForKeyPress() {

	if (GetAsyncKeyState(VK_F11)) {
		std::cout << "Exit triggered" << endl;
		exit(0);
	}

	// Pause when F9 is Pressed
	if (GetAsyncKeyState(VK_F9)) {
		std::cout << "Paused. Press enter to continue" << endl;
		system("PAUSE");
		std::cout << "Continued." << endl;
	}
}

// Drives the program
void mainLoop() {
	// Loop until user chooses to exit
	while (1) {

		checkForKeyPress();

		goThroughClickSequence();
	}
}


int main()
{
	// Make the console look pretty
	configConsole();

	// Take user through initial setup options
	initialSetup();
	
	// Menu
	displayMenu();

	// Loop until user chooses to exit
	mainLoop();

	exit(0);
}
