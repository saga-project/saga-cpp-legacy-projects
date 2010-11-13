
#include <sstream>
#include <iostream>

#include "output_png.hpp"


output_png::output_png (unsigned int size_x, 
                        unsigned int size_y, 
                        unsigned int cnum)
    : size_x_   (size_x),
      size_y_   (size_y),
      cnum_     (cnum * 256 - 1),
      png_      (size_x_, size_y_, (int) cnum_, "mandelbrot.png")
{
  // try to open font file
  char * saga_loc = ::getenv ("SAGA_LOCATION");

  if ( NULL == saga_loc )
  {
    font_loc_  = "./pngwriter/fonts/FreeSansBold.ttf";
  }
  else
  {
    font_loc_ = saga_loc;
    font_loc_ += "/share/pngwriter/fonts/FreeSansBold.ttf";
  }
}

output_png::~output_png (void)
{
  // close down PNG
  png_.close ();
}


// paint a rectangular box.  The given data determine the color index of the
// pixels to be drawn.  The identifier determines the box's label to be printed.
void output_png::paint_box (unsigned int x0, unsigned int n_x, 
                            unsigned int y0, unsigned int n_y,
                            std::vector <std::vector <int> > & data, 
                            std::string ident)
{
  // sanity check.  
  if ( data.size () != n_x )
  {
    std::stringstream ss;
    ss << "incorrect box size (x): " << data.size () << " - " << n_x;
    throw ss.str ().c_str ();
  }

  // iterate over all lines
  for ( unsigned int x = 0; x < n_x; x++ )
  {
    std::vector <int> line = data[x];

    // line sanity check
    if ( line.size () != n_y )
    {
      std::stringstream ss;
      ss << "incorrect line size (x): " << line.size () << " - " << n_y;
      throw ss.str ().c_str ();
    }

    // iterate over all pixels in line
    for ( unsigned int y = 0; y < n_y; y++ )
    {
      int r = 0;
      int g = (256 * line[y]) % (cnum_ - 2) + 2;
      int b = (256 * line[y]) % (cnum_ - 2) + 2;

      png_.plot (x0 + x, y0 + y, r, g, b);
    }
  }

  
  // for demo purposes, we also draw box boundaries
  for ( unsigned int bx = 0; bx < n_x; bx++ )
  {
    png_.plot (x0 + bx, y0 + 0  , 1.0, 1.0, 1.0);
    png_.plot (x0 + bx, y0 + n_y, 1.0, 1.0, 1.0);
  }

  for ( unsigned int by = 0; by < n_y; by++ )
  {
    png_.plot (x0 + 0  , y0 + by, 1.0, 1.0, 1.0);
    png_.plot (x0 + n_x, y0 + by, 1.0, 1.0, 1.0);
  }
  

  // print identifier as box label
  
  std::string tmp (ident);
  int         len = tmp.size ();

  if ( tmp.size () > 53 )
  {
    tmp[51] = '.';
    tmp[52] = '.';
    tmp[53] = '.';
    len     = 53;
  }

  char * txt = strdup (tmp.c_str ());
  png_.plot_text( (char*)(font_loc_.c_str ()), 8, x0+10, y0+10, 0, txt, 1.0, 1.0, 1.0);
  free (txt);
}

