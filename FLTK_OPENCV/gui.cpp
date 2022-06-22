#include "gui.h"

M_Window::M_Window(int x, int y, int w, int h, const char* title) : Fl_Window(x, y, w, h, title), box(0), menu(0)
{
	menu = new MenuBar(0, 0, this->w(), 20, this);
	box = new Box(0, getMenu()->h(), this->w(), this->h(),this);

	this->resizable(this);
	this->end();
	this->show();
}
M_Window::M_Window(int w, int h, const char* title) : Fl_Window(w, h, title), box(0), menu(0)
{
	this->resizable(this);
}

int M_Window::w()
{
	return Fl_Window::w();
}

int M_Window::h()
{
	if (!menu)
		return Fl_Window::h();
	return Fl_Window::h() - menu->h();
}

Box* M_Window::getBox()
{
	return box;
}

MenuBar* M_Window::getMenu()
{
	return menu;
}

void M_Window::ChangeFileName(Fl_File_Chooser* fl, void* data)
{
	if (!fl->shown()) {
		((M_Window*)data)->getBox()->set_PathToFile(fl->value());
		((M_Window*)data)->getBox()->load_OriginalImage();
		delete fl;
	}
}

void M_Window::FileOpen(Fl_Widget* w, void* v)
{
	Fl_File_Chooser* window = new Fl_File_Chooser("Choose file", "*.jpg", NULL, 0);
	window->callback(M_Window::ChangeFileName, ((void*)v));
	window->show();

	return;
}

MenuBar::MenuBar(int x, int y, int w, int h, Fl_Widget* wd) : Fl_Menu_Bar(x, y, w, h), TargetH(h)
{
	Fl_Menu_Item* create;

	create = new Fl_Menu_Item[19]
	{
		{"File",  0,  (Fl_Callback*)M_Window::FileOpen, wd},
			{"Original",  0,  (Fl_Callback*)Box::LoadOriginal, wd},
			{"Resize",0,0,0, FL_SUBMENU } ,
				{"AutoResize",0,(Fl_Callback*)Box::AutoResize_Switcher, wd},
				{"No Resize",0,(Fl_Callback*)Box::SetResizeType, new data{(void*)2,wd}},
				{"Resize manually",0,(Fl_Callback*)Box::SetResizeType, new data{(void*)1,wd}},
				{"Proportional",0,(Fl_Callback*)Box::SetResizeType, new data{(void*)3,wd}},
			{0} ,
			{0} };

	this->menu(create);
	this->textcolor(0);
	this->show();
}

void MenuBar::draw()
{
	this->resize(this->x(), this->y(), this->w(), TargetH);
	Fl_Menu_Bar::draw();
}

Box::Box(int x, int y, int w, int h, Fl_Widget* wd) :Fl_Box(x, y, w, h), current(NULL),original(NULL)
{
	window = wd;
	typeR = new data{ 0,wd };
}

void Box::draw()
{
	if(original)
		Box::SetResizeType(NULL , typeR);
	Fl_Box::draw();
	((M_Window*)window)->getMenu()->draw();
}

void Box::load_OriginalImage()
{
	if (original)
		delete original;
	original = Image::create_Image((char*)pathToFile.c_str(),1,this);
	if (current)
		delete current;
	current = Image::create_Image((char*)original->array, 3, this, original->w(), original->h());
	this->resize(0, ((M_Window*)window)->getMenu()->h(), ((M_Window*)window)->w(), ((M_Window*)window)->h());
	this->image(current);
	((M_Window*)window)->redraw();
}

void Box::ResizeImg( int w , int h)
{
	if (!original || !current)
		return;
	cv::Mat mt(original->h(),original->w(), CV_8UC3, (void*)original->array);
	cv::Mat res(h, w, CV_8UC3);
	char* imgData = new char[h * w * 3];

	cv::resize(mt, res, res.size());
	if(current)
		delete current;

	memcpy(imgData, res.data, h * w * 3);
	current = Image::create_Image(imgData, 2, this, w, h);
	this->image(current);
}

void Box::set_AutoResize(int b) { typeR->val1 = (void*)b ; }
int Box::get_AutoResize() { return (int)typeR->val1; }
Image* Box::getOriginalImg() { return original; }
Image* Box::get_Current() { return current; }
void Box::set_Current(Image* img)
{
	if (current)
		delete current;
	current = img;
}

void Box::set_PathToFile(std::string str)
{
	pathToFile = str;
}

void Image::set_ImgData(char* c)
{
	if (ImgData)
		delete ImgData;
	ImgData = c;
}
char* Image::get_ImgData()
{
	return ImgData;
}

Image::~Image()
{
	if (ImgData)
		delete ImgData;
}

Image::Image(char* c ,int w , int h, Fl_Widget* b) : Fl_RGB_Image( (const uchar*)c , w , h )
{
	box = b;
	ImgData = c;
}

Image* Image::create_Image(char* c, int type, Fl_Widget* wd, int w , int h)
{
	Fl_RGB_Image *ptr = NULL;
	Image* img = NULL;
	char* buf = NULL;
	switch (type)
	{
	case 1:
		ptr = new Fl_JPEG_Image(c);
		buf = new char[ptr->w() * ptr->h() * 3 * 2];
		memcpy(buf, ptr->array, ptr->w() * ptr->h() * 3);
		img = new Image(buf , ptr->w() , ptr->h() , wd);
		delete ptr;
		break;
	case 2:
		img = new Image(c, w, h, wd);
		break;
	case 3:
		buf = new char[w * h * 3 * 2];
		memcpy(buf, c, w * h * 3);
		img = new Image(buf, w, h, wd);
	}

	return img;
}

Image* Box::copyToCurrent(Image *orig)
{
	return (current = Image::create_Image((char*)orig->array, 3, this, orig->w(), orig->h()));
}

void Box::AutoResize_Switcher(Fl_Widget* fl, void* wd)
{
	if (!((M_Window*)wd)->getBox()->get_AutoResize())
	{
		((M_Window*)wd)->getBox()->ResizeImg(((M_Window*)wd)->w(), ((M_Window*)wd)->h());
		((M_Window*)wd)->getBox()->resize(0, ((M_Window*)wd)->getMenu()->h(), ((M_Window*)wd)->w(), ((M_Window*)wd)->h());

		((M_Window*)wd)->redraw();
	}
	((M_Window*)wd)->getBox()->set_AutoResize(!((M_Window*)wd)->getBox()->get_AutoResize());
}
void Box::SetResizeType(Fl_Widget* fl, void* wd)
{
	int h, w, origH, origW;
	if (!((M_Window*)((data*)wd)->val2)->getBox()->getOriginalImg())
		return;

	switch ((int)((data*)wd)->val1)
	{
	case 1:
		((M_Window*)((data*)wd)->val2)->getBox()->ResizeImg(((M_Window*)(((data*)wd)->val2))->w(), ((M_Window*)(((data*)wd)->val2))->h());
		((M_Window*)((data*)wd)->val2)->getBox()->resize(0, ((M_Window*)((data*)wd)->val2)->getMenu()->h(), ((M_Window*)((data*)wd)->val2)->w(), ((M_Window*)((data*)wd)->val2)->h());
		((M_Window*)((data*)wd)->val2)->redraw();
		break;
	case 2:
		Box::LoadOriginal(NULL, ((data*)wd)->val2);
		break;
	case 3:
		origH = ((M_Window*)((data*)wd)->val2)->getBox()->getOriginalImg()->h();
		origW = ((M_Window*)((data*)wd)->val2)->getBox()->getOriginalImg()->w();
		if (origH > origW)
		{
			h = ((M_Window*)((data*)wd)->val2)->h();
			w = ((M_Window*)((data*)wd)->val2)->getBox()->getCoeffH(((M_Window*)((data*)wd)->val2)->h()) * origW;
		}
		else
		{
			w = ((M_Window*)((data*)wd)->val2)->w();
			h = ((M_Window*)((data*)wd)->val2)->getBox()->getCoeffW(((M_Window*)((data*)wd)->val2)->w()) * origH;
		}
		((M_Window*)((data*)wd)->val2)->getBox()->ResizeImg(w, h);
		((M_Window*)((data*)wd)->val2)->getBox()->resize(0, ((M_Window*)((data*)wd)->val2)->getMenu()->h(), ((M_Window*)((data*)wd)->val2)->w(), ((M_Window*)((data*)wd)->val2)->h());
		((M_Window*)((data*)wd)->val2)->getBox()->set_AutoResize(3);
		((M_Window*)((data*)wd)->val2)->redraw();
		break;
	}
}

float Box::getCoeffW(int w)
{
	return (float)w /original->w() ;
}
float Box::getCoeffH(int h)
{
	return  (float)h / original->h() ;
}

void Box::ManualResize(Fl_Widget* fl, void* wd)
{
}

void Box::LoadOriginal(Fl_Widget* fl, void* wd)
{
	((M_Window*)wd)->getBox()->set_AutoResize(0);
	((M_Window*)wd)->getBox()->load_OriginalImage();
}
