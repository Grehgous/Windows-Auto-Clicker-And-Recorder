/*
TODO:
-Add struct that contains click location, wait period after, etc..
 Vector of these objects are traversed during click sequence.
-Better calculation of next_work_period. Should wait until sequence is done, then just
 wait for slpTimeAtEndOfSequence
-Input validation
-Way to go back to config step without restarting progtam
-Save config to file
-Get rid of one click option. Can just do array with 1 entry.
-Remove sleep time at end of sequence
-Show click position on screen with number in sequence


KNOWN ISSUES:
-next_work_period does not work properly at start of program
-Doesn't click until time to move to next position

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

int slpTimeAtEndOfSequence;
int constSleepBetweenClicks; 
int totalSlpTimeDuringSequence;						
char input;													// y or n from user
POINT p, r;													// location on screen
bool BREAK;
auto next_work_period = std::chrono::steady_clock::now();	// time to perform next click
bool arrayOfClicks;
bool sleepAfterEndOfSequence;
bool sleepAfterEachClick;
bool uniqueSleepBetweenClicks;
vector<POINT> pointList;
vector<int> sleepList; // List of steep times in ms

void performMovementAndClick(int, int);
void goThroughClickSequence();
void initialSetup();
void displayMenu();
void mainLoop();
void configConsole();
void checkForKeyPress();


// Moves mouse to proper location, and click
void performMovementAndClick(int x, int y) {

	SetCursorPos(x, y);

	mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);	// Mouse click
	mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);	// Mouse release

	return;
}

// Move mouse randomly by a few pixels
void goThroughClickSequence() {

	int i = 0;	// Index in pointList and sleepList

	// Iterate through coordinate list
	for (std::vector<POINT>::iterator it = pointList.begin(); it != pointList.end(); ++it, i++) {
		performMovementAndClick(pointList[i].x, pointList[i].y);
		
		// If not the end of sequence, do sleep if configured
		if (it != pointList.end() && arrayOfClicks && sleepAfterEachClick) {
			Sleep((sleepList[i]));
		}
	}
	return;
}

// Get user input required to configure program
void initialSetup() {
	
	// TODO: Validate input.
	// TODO: Get rid of one click option. Can just do array with 1 entry.

	int temp = 0;

	std::cout << "1) One click position" << endl;
	std::cout << "2) Array of positions" << endl;
	std::cin >> input;

	// Only one position
	if (input == '1') {

		arrayOfClicks = false;

		// Set X and Y coordinates to be clicked
		std::cout << "Press 'Enter' to set mouse position" << endl;
		system("PAUSE");

		GetCursorPos(&p);
		pointList.push_back(p);

		std::cout << "Position set to (" << p.x << "," << p.y << ") " << endl;	
	}

	// Multiple positions
	else if (input == '2') {
		arrayOfClicks = true;

		std::cout << "Sleep after each click? (y/n): ";
		std::cin >> input;

		if (input == 'y') {
			sleepAfterEachClick = true;

			std::cout << "Unique sleep times? (y/n): ";
			std::cin >> input;

			if (input == 'y') {
				uniqueSleepBetweenClicks = true;
			} else {
				// Sleep times are constant after each click
				std::cout << "Constant sleep time between clicks in ms: ";
				std::cin >> constSleepBetweenClicks;

				std::cout << "Sleep time at end of click sequence in ms: ";
				std::cin >> slpTimeAtEndOfSequence;
			}
		}

		std::cout << "Press 'ENTER' to set mouse position" << endl;
		std::cout << "Press 'ESC' when all keys are entered" << endl << endl;

		while (!GetAsyncKeyState(VK_ESCAPE)) {

			// Wait for next keypress
			system("PAUSE");

			// Escape keypress ends adding new clicks
			// TODO: Is this needed?
			if (GetAsyncKeyState(VK_ESCAPE)) {
				 break;
			} else if (GetAsyncKeyState(VK_RETURN)) {
				// Adda a new click to the sequence
				GetCursorPos(&p);
				pointList.push_back(p);
				std::cout << "Position set to (" << p.x << "," << p.y << ") " << endl;

				// add to paralell vector of unique sleep times
				if (uniqueSleepBetweenClicks) {
					std::cout << "Sleep time in ms: ";
					std::cin >> temp;
					totalSlpTimeDuringSequence += temp; // track total sleep time during sequence
					sleepList.push_back(temp);
				} else { 
					// Add the constant sleep time to the sleepList and increment total
					sleepList.push_back(constSleepBetweenClicks);
					totalSlpTimeDuringSequence += constSleepBetweenClicks;
				}
			}
		}
		std::cout << "Done filling array of points" << endl;

	} else {
		std::cout << "Invalid entry" << endl;

	}

	return;
}


// Prints the options
void displayMenu() {
	// Menu
	std::cout << "***********************************" << endl;
	std::cout << "* 'F9' to pause                   *" << endl;
	if (!arrayOfClicks) {
		std::cout << "* 'F10' to set new mouse position *" << endl;
	}
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

	if (!arrayOfClicks) {
		// Set new mouse position when F10 is Pressed. Only works for single click position.
		if (GetAsyncKeyState(VK_F10)) {
			GetCursorPos(&p);
			pointList[0] = p;

			std::cout << "Position changed to (" << p.x << "," << p.y << ") " << endl;
			BREAK = true;
		}
	}
}

// Drives the program
void mainLoop() {
	// Loop until user chooses to exit
	while (1) {

		// TODO: Why are there 3 checks for key presses here?
		checkForKeyPress();

		// To keep disk usage low, 100 may need to be changed
		Sleep(100);

		checkForKeyPress();

		// If current time is time to do next action
		// TODO: Is next_work_period needed or can you just do the sequence
		if (std::chrono::steady_clock::now() > next_work_period) {
			goThroughClickSequence();

			// Calculate next click time
			next_work_period += std::chrono::milliseconds((slpTimeAtEndOfSequence + totalSlpTimeDuringSequence));
		}

		checkForKeyPress();
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
