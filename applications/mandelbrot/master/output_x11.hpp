
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

    // window size
    unsigned int         size_x_;
    unsigned int         size_y_;

    // colormap
    unsigned int         cnum_; // nuimber of colors
    Colormap             cmap_;
    std::vector <long>   colors_;


  public:
    // the c'tor takes the size the window.  The user of the device needs to
    // ensure that all drawinf requests lie inside that window.
     output_x11  (unsigned int size_x, 
                  unsigned int size_y, 
                  unsigned int cnum);
    ~output_x11 (void);

    // paint a rectangular box
    void paint_box (unsigned int x0, unsigned int n_x, 
                    unsigned int y0, unsigned int n_y,
                    std::vector <std::vector <int> > & data, 
                    std::string ident = "");
};

#endif // OUTPUT_X11_HPP

