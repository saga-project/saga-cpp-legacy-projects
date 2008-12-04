
#ifndef OUTPUT_X11_HPP
#define OUTPUT_X11_HPP

#include <X11/Xlib.h>
#include <X11/Xcms.h>

#include "output_base.hpp"

// This class implements the abstract output_base, and can thus serve as output
// device for the mandelbrot class.  It displays the mandelbrot set using X11
// primitives.
//
// TODO: add configure script to detect/use X11
//
class output_x11 : public output_base
{
  private:
    // X11 state
    Display            * dpy_;
    int                  scr_;
    Window               root_;
    Window               win_;
    int                  posx_;
    int                  posy_;
    GC                   gc_;

    // colormap
    int                  cnum_; // nuimber of colors
    Colormap             cmap_;
    std::vector <long>   colors_;

    // window size
    int                  size_x_;
    int                  size_y_;


  public:
    // the c'tor takes the size the window.  The user of the device needs to
    // ensure that all drawinf requests lie inside that window.
     output_x11  (int size_x, 
                  int size_y, 
                  int cnum);
    ~output_x11 (void);

    // paint a rectangular box
    void paint_box (int x0, int n_x, 
                    int y0, int n_y,
                    std::vector <std::vector <int> > & data, 
                    std::string ident = "");
};

#endif // OUTPUT_X11_HPP

