#include "window/window.h"

int main(int argc, char** argv) {
	Window window = unwrap(window_new("Test", 800, 600));
	return 0;
}
