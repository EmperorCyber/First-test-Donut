#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>
#include <vector>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#endif

using namespace std;

// Function to clear the console screen
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    cout << "\033[2J\033[1;1H";
#endif
}

// Function to get console dimensions
pair<int, int> getConsoleDimensions() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return {csbi.srWindow.Right - csbi.srWindow.Left + 1, csbi.srWindow.Bottom - csbi.srWindow.Top + 1};
#else
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return {w.ws_col, w.ws_row};
#endif
}

// Function to sleep for a specified number of milliseconds
void sleepMilliseconds(int milliseconds) {
    this_thread::sleep_for(chrono::milliseconds(milliseconds));
}

int main() {
    // Rotation angles for the cube
    double A = 0, B = 0;
    // Loop variables
    double i, j;
    // z-buffer for depth sorting, size is console width * height
    vector<double> z;
    // Character buffer to store the rendered frame
    vector<char> buffer;
    // Console dimensions
    int screenWidth, screenHeight;

    // Main animation loop
    while (true) {
        // Get the current console dimensions
        tie(screenWidth, screenHeight) = getConsoleDimensions();

        // Resize the z-buffer and character buffer if the screen size changes
        z.resize(screenWidth * screenHeight);
        buffer.resize(screenWidth * screenHeight);

        // Clear the screen
        clearScreen();

        // Initialize the z-buffer and character buffer
        fill(buffer.begin(), buffer.end(), ' ');
        fill(z.begin(), z.end(), 0);

        // Loop through the cube's faces (approximated by angles)
        for (j = 0; j < 6.28; j += 0.07) {
            for (i = 0; i < 6.28; i += 0.02) {
                // Calculate the 3D coordinates of the current point on the cube
                double c = sin(i), d = cos(j), e = sin(A), f = sin(j), g = cos(A), h = d + 2;
                double D = 1 / (c * h * e + f * g + 5); // Projection factor
                double l = cos(i), m = cos(B), n = sin(B), t = c * h * g - f * e;

                // Calculate the 2D coordinates of the projected point
                int x = screenWidth / 2 + 30 * D * (l * h * m - t * n);
                int y = screenHeight / 2 + 15 * D * (l * h * n + t * m);
                // Calculate the index into the buffer
                int o = x + screenWidth * y;
                // Calculate the brightness (used to select a character)
                int N = 8 * ((f * e - c * d * g) * m - c * d * e - f * g - l * d * n);

                // Check if the point is within the screen bounds and in front of previous points
                if (y < screenHeight && y > 0 && x > 0 && x < screenWidth && D > z[o]) {
                    z[o] = D; // Store the depth
                    buffer[o] = ".,-~:;=!*#$@"[N > 0 ? N : 0]; // Select a character based on brightness
                }
            }
        }

        // Print the frame to the console
        for (int k = 0; k < screenWidth * screenHeight; k++) {
            cout << ((k % screenWidth) ? buffer[k] : '\n');
        }

        // Update the rotation angles for the next frame
        A += 0.04;
        B += 0.02;

        // Pause for a short time to control the animation speed
        sleepMilliseconds(30);
    }

    return 0;
}
