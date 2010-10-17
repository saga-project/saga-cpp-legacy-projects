
#ifndef OUTPUT_BASE_HPP
#define OUTPUT_BASE_HPP

#include <vector>

// the mandelbrot class uses implementations of this abstract
// base class to draw the mandelbrot set after collecting the
// data.  Such implementations need to be 'registered' in the
// mandelbrot c'tor.
class output_base
{
  public:
    // paint a given rectangular box.  All coordinates are in
    // pixels.  data must contain n_x * n_y data items, which
    // are integers, and should be represented as pixel color
    // indexes.  The ident string identifies the client which
    // computed the data, and can optionally be printed into the
    // box.  Also, the box boundaries can optionally be printed
    // in a different color, to demonstrate the spatial chop up.
    virtual void paint_box (unsigned int x0, unsigned int n_x, 
                            unsigned int y0, unsigned int n_y,
                            std::vector <std::vector <int> > & data,
                            std::string ident = "") = 0;
};

#endif // OUTPUT_BASE_HPP

