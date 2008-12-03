
#ifndef OUTPUT_X11_HPP
#define OUTPUT_X11_HPP

#include <X11/Xlib.h>
#include <X11/Xcms.h>

#include "output_base.hpp"

#define C_NUM 256

class output_x11 : public output_base
{
  private:
    Display            * dpy_;
    int                  scr_;
    Window               root_;
    Window               win_;
    int                  posx_;
    int                  posy_;
    GC                   gc_;
    Colormap             cmap_;
    std::vector <long>   colors_;
    int                  size_x_;
    int                  size_y_;

  public:
    output_x11  (int size_x, int size_y);
    ~output_x11 (void);

    void paint_box (int x0, int n_x, 
                    int y0, int n_y,
                    std::vector <std::vector <int> > & data, 
                    std::string ident = "");

    void control (void);
};

#endif // OUTPUT_X11_HPP

