
#ifndef OUTPUT_PNG_HPP
#define OUTPUT_PNG_HPP

#include "pngwriter/pngwriter.h"

#include "output_base.hpp"

// This class implements the abstract output_base, and can thus serve as output
// device for the mandelbrot class.  It displays the mandelbrot set using PNG
// primitives.
//
// TODO: add configure script to detect/use PNG
//
class output_png : public output_base
{
  private:
    // PNG state

    // window size
    unsigned int         size_x_;
    unsigned int         size_y_;

    // colormap
    unsigned int         cnum_; // number of colors
    std::vector <long>   colors_;

    // font location
    std::string          font_loc_;

    pngwriter            png_;


  public:
    // the c'tor takes the size the window.  The user of the device needs to
    // ensure that all drawinf requests lie inside that window.
     output_png  (unsigned int size_x, 
                  unsigned int size_y, 
                  unsigned int cnum);
    ~output_png (void);

    // paint a rectangular box
    void paint_box (unsigned int x0, unsigned int n_x, 
                    unsigned int y0, unsigned int n_y,
                    std::vector <std::vector <int> > & data, 
                    std::string ident = "");
};

#endif // OUTPUT_PNG_HPP

