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
using namespace System::Drawing;

struct Click {
	POINT point;
	int sleep;
};

int constSleepBetweenClicks;
bool uniqueSleepBetweenClicks;
vector<Click> clickList;
std::atomic_bool Paused;
std::atomic_bool Reset;

void performMovementAndClick(POINT);
void goThroughClickSequence();
void initialSetup();
void displayMenu();
void configConsole();
void checkForKeyPress();
void moveMouse(size_t x, size_t y);
void leftClick();

void leftClick() {

	// Left down
	INPUT Input = { 0 };
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	::SendInput(1, &Input, sizeof(INPUT));

	// Left up
	Input = { 0 };
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	::SendInput(1, &Input, sizeof(INPUT));
}

void moveMouse(size_t x, size_t y) {

	double dx = x * (65535.0f / (::GetSystemMetrics(SM_CXSCREEN) - 1));
	double dy = y * (65535.0f / (::GetSystemMetrics(SM_CYSCREEN) - 1));

	INPUT Input = { 0 };

	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
	Input.mi.dx = LONG(dx);
	Input.mi.dy = LONG(dy);

	::SendInput(1, &Input, sizeof(INPUT));

	return;
}

// Moves mouse to proper location, and click
void performMovementAndClick(POINT p) {

	moveMouse(p.x, p.y);
	leftClick();

	return;
}

// Perform clicks and sleeps
void goThroughClickSequence() {

	// Main loop for thread
	while (1) {

		int i = 0;

		// Perform each click and sleep
		for (std::vector<Click>::iterator it = clickList.begin(); it != clickList.end(); ++it, i++) {

			// Continuously check for signal to kill the thread
			if (Reset) return;

			// Wait until thread is not paused to continue click sequence
			while (Paused) {
				// Continuously check for signal to kill the thread
				if (Reset) return;

				// Sleep
				Thread::Sleep(100);
			}


			performMovementAndClick(clickList[i].point);

			std::cout << i << ": Clicked (" << clickList[i].point.x << ", " << clickList[i].point.y << "): "
				<< "Sleeping(" << clickList[i].sleep << "ms)" <<  endl;

			Thread::Sleep(clickList[i].sleep);
		}
	}
}

// Get user input required to configure program
void initialSetup() {
	
	// TODO: Validate input.
	// TODO: Error if no keys in sequence

	// Reset values
	int temp = 0;
	char input;

	clickList.clear();
	//Paused = true;
	Reset = false;

	
	std::cout << "Unique sleep times? (y/n):  ";
	std::cin >> input;

	// Ask the user for a sleep each time, or use the same
	if (input == 'y') {
		uniqueSleepBetweenClicks = true;
	}
	else {
		// Sleep times are constant after each click
		uniqueSleepBetweenClicks = false;
		std::cout << "Constant sleep time between clicks in ms:  ";
		std::cin >> constSleepBetweenClicks;
	}

	std::cout << endl;
	std::cout << "***********************************" << endl;
	std::cout << "* 'SPACE' to set new position     *" << endl;
	std::cout << "* 'ESC' when done                 *" << endl;
	std::cout << "***********************************" << endl << endl;


	// Add new clicks to the sequence
	while (_getch()){

		Click click;

		if (GetAsyncKeyState(VK_ESCAPE)) {
			// Escape keypress ends adding new clicks
			Paused = false;
			break;
		}
		else if (GetAsyncKeyState(VK_SPACE)) {

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

	displayMenu();
	Sleep(100);

	return;
}

// Prints the options
void displayMenu() {

	// Console::Clear();

	// Menu
	std::cout << endl;
	std::cout << "***********************************" << endl;
	std::cout << "* 'F9' to toggle pause            *" << endl;
	std::cout << "* 'F10' to close program          *" << endl;
	std::cout << "* 'F12' to restart setup          *" << endl;
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

	while (1) {

		if (GetAsyncKeyState(VK_F10)) {
			// Kill the program
			exit(0);
		} else if (GetAsyncKeyState(VK_F9)) {
			// Toggle whether the clicking is paused
			Paused = !Paused;
			displayMenu();
		} else if (GetAsyncKeyState(VK_F12)) {
			// Set atomic flag so other threads know to end
			// and close this thread.
			Reset = true;
			return;
		}
		
		Thread::Sleep(100);
	}
}


int main(){

	// Make the console look pretty
	configConsole();

	// Loop allows for resetting the config when the threads are ended
	while (1) {
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
	}
}
