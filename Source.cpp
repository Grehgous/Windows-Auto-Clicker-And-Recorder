/*
TODO:
-Input validation
-Way to go back to config step without restarting program
-Save config to file
-Show click position on screen with number in sequence
-Remove globals


KNOWN ISSUES:
- During initial setup F keys hit the ESCAPE key catch and end the setup

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
#include <conio.h>
#include <atomic>

using namespace std;
using namespace System; 
using namespace System::Windows::Forms;
using namespace System::Threading;

struct Click {
	POINT point;
	int sleep;
};

int constSleepBetweenClicks;
bool uniqueSleepBetweenClicks;
vector<Click> clickList;
std::atomic_bool Paused;

void performMovementAndClick(POINT);
void goThroughClickSequence();
void initialSetup();
void displayMenu();
void configConsole();
void checkForKeyPress();



// Moves mouse to proper location, and click
void performMovementAndClick(POINT p) {

	SetCursorPos(p.x, p.y);

	mouse_event(MOUSEEVENTF_LEFTDOWN, p.x, p.y, 0, 0);	// Mouse click
	mouse_event(MOUSEEVENTF_LEFTUP, p.x, p.y, 0, 0);	// Mouse release

	return;
}

// Perform clicks and sleeps
void goThroughClickSequence() {

	// Main loop for thread
	while (1) {

		int i = 0;

		// Perform each click and sleep
		for (std::vector<Click>::iterator it = clickList.begin(); it != clickList.end(); ++it, i++) {

			// Wait until thread is not paused to continue click sequence
			while (Paused) {
				Thread::Sleep(100);
			}

			performMovementAndClick(clickList[i].point);

			Thread::Sleep(clickList[i].sleep);
		}
	}
}

// Get user input required to configure program
void initialSetup() {
	
	// TODO: Validate input.
	// TODO: Default sleep time
	// TODO: Export/import settings
	// TODO: Error if no keys in sequence

	int temp = 0;
	char input;

	// Make the console look pretty
	configConsole();


	std::cout << "Unique sleep times? (y/n):  ";
	std::cin >> input;

	// Ask the user for a sleep each time, or use the same
	if (input == 'y') {
		uniqueSleepBetweenClicks = true;
	}
	else {
		// Sleep times are constant after each click
		std::cout << "Constant sleep time between clicks in ms:  ";
		std::cin >> constSleepBetweenClicks;
	}

	std::cout << endl;
	std::cout << "***********************************" << endl;
	std::cout << "* 'ENTER' to set new position     *" << endl;
	std::cout << "* 'ESC' when done                 *" << endl;
	std::cout << "***********************************" << endl << endl;


	// Add new clicks to the sequence
	while (_getch()){

		Click click;

		if (GetAsyncKeyState(VK_RETURN)) {

			// Set the click position
			GetCursorPos(&click.point);
			std::cout << "Position set to (" << click.point.x << "," << click.point.y << ") " << endl;

			if (uniqueSleepBetweenClicks) {
				std::cout << "Sleep time in ms: ";
				std::cin >> click.sleep;

			} 
			else { 
				click.sleep = constSleepBetweenClicks;
			}

			// Add to the list
			clickList.push_back(click);

		} 
		else if (GetAsyncKeyState(VK_ESCAPE)) {
			// Escape keypress ends adding new clicks
			break;
		}
	}

	displayMenu();

	return;
}


// Prints the options
void displayMenu() {

	Console::Clear();

	// Menu
	std::cout << endl;
	std::cout << "***********************************" << endl;
	std::cout << "* 'F9' to start/stop clicks       *" << endl;
	std::cout << "* 'F10' to close program          *" << endl;
	std::cout << "***********************************" << endl;

	// Status
	if (Paused) {
		std::cout << "* Status:  PAUSED.                *" << endl;
		std::cout << "***********************************" << endl << endl;
	}
	else {
		std::cout << "* Status:  ACTIVE.                *" << endl;
		std::cout << "***********************************" << endl << endl;
	}
}

// Make the console loop pretty
void configConsole() {

	SetConsoleTitle("AutoClicker");											// Window title
	Console::SetWindowSize(50, 15);											// Window size
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);			// Font color
	SetLayeredWindowAttributes(GetActiveWindow(), NULL, 100, LWA_ALPHA);	// Opacity 0-255
}


// See if key has been pressed
void checkForKeyPress() {
	std::cout << "Thread started" << endl;

	// Main loop for thread
	while (1) {

		// Kill program
		if (GetAsyncKeyState(VK_F10)) {
			exit(0);
		}

		// Change the Pause state and update menu
		if (GetAsyncKeyState(VK_F9)) {
			if (!Paused) {
				Paused = true;
				displayMenu();

			}
			else if (Paused) {
				Paused = false;
				displayMenu();
			}
		}
		
		Thread::Sleep(100);
	}
}

int main(){

	// Take user through initial setup options
	initialSetup();

	// This thread loops checking for relevant key presses
	Thread^ th1 = gcnew Thread(gcnew ThreadStart(&checkForKeyPress));

	// This thread repeats the click sequence
	Thread^ th2 = gcnew Thread(gcnew ThreadStart(&goThroughClickSequence));

	th1->Start();
	th2->Start();

	// Wait for the threads to finish.
	th1->Join();
	th2->Join();

	exit(0);
}
