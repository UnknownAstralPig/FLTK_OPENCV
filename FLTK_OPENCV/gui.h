typedef struct IUnknown IUnknown;

#define _CRT_SECURE_NO_WARNINGS
#define WIN32

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Widget.H>
#include <Fl/Fl_File_Chooser.H>
#include <Fl/Fl_Text_Display.H>
#include <Fl/Fl_Text_Buffer.H>
#include <Fl/Fl_JPEG_Image.H>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/types_c.h>

struct data
{
	void* val1,
		* val2;
};

class Image : public Fl_RGB_Image
{
private:
	char* ImgData;
	Fl_Widget* box;
public:
	~Image();
	Image(char*, int, int, Fl_Widget*);
	void set_ImgData(char*);
	char* get_ImgData();

	static Image* create_Image(char*, int, Fl_Widget*, int = 0 , int = 0);
};

class Box : public Fl_Box
{
private:
	Image* original;
	Image* current;
	std::string pathToFile;
	Fl_Widget* window;
	data* typeR;
public:
	Box(int x, int y, int w, int h, Fl_Widget* wd = 0);
	void draw();
	void set_AutoResize(int b);
	void set_PathToFile(std::string str);
	void set_Current(Image*);
	Image* get_Current();
	int get_AutoResize();
	void load_OriginalImage();
	void ResizeImg( int , int );
	Image* copyToCurrent(Image*);
	Image* getOriginalImg();
	float getCoeffW( int );
	float getCoeffH( int );

	static void LoadOriginal(Fl_Widget*, void*);
	static void AutoResize_Switcher(Fl_Widget*, void*);
	static void SetResizeType(Fl_Widget*, void*);
	static void ManualResize(Fl_Widget*, void*);
	static void blur(Fl_Widget*, void*);
};
class MenuBar : public Fl_Menu_Bar
{
private:
	int TargetH;
public:
	MenuBar(int x, int y, int w, int h, Fl_Widget* wd = 0);
	void draw();
};

class M_Window : public Fl_Window
{
private:
	MenuBar* menu;
	Box* box;

public:
	M_Window(int x, int y, int w, int h, const char* title );
	M_Window(int w, int h, const char* title );

	static void FileOpen(Fl_Widget*, void*);
	static void LoadOriginal(Fl_Widget*, void*);
	static void SetResizeType(Fl_Widget*, void*);
	static void ChangeFileName(Fl_File_Chooser*, void*);

	int w();
	int h();

	Box* getBox();
	MenuBar* getMenu();
};