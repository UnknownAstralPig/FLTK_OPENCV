#include "gui.h"

int main(int argc, char** argv)
{
	M_Window w(100, 100, 500, 500, "Simple Image Editor");
	w.show(argc, argv);

	return Fl::run();
}
