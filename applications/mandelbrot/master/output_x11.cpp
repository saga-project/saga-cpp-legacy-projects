
#include <sstream>
#include <iostream>

#include "output_x11.hpp"

// toggle for box borders and labels
#define DEMO true

output_x11::output_x11 (unsigned int size_x, 
                        unsigned int size_y, 
                        unsigned int cnum)
    : size_x_   (size_x),
      size_y_   (size_y),
      cnum_     (cnum), 
      fallback_ (false)
{
  // Open the display
  dpy_ = XOpenDisplay (NULL);

  // if that did not work, we don't bother with X11 output, but instead just
  // print a message of what we *would* paint
  if ( dpy_ == NULL )
  {
    std::cout << "init x11 output failed - continue without graphics\n";
    fallback_ = true;
  }
  else
  {
    std::cout << "init x11 output\n";
    
    // get default screen
    scr_ = DefaultScreen (dpy_);

    // get root window
    root_ = DefaultRootWindow (dpy_);

    // Create the window
    win_ = XCreateSimpleWindow (dpy_, root_, 
                                0, 0,                     // offset for NW corner
                                size_x_, size_y_,         // window size
                                0,                        // borderwidth
                                BlackPixel (dpy_, scr_),  // bordercolor
                                BlackPixel (dpy_, scr_)); // backgroundcolor

    // get MapNotify events
    XSelectInput (dpy_, win_, StructureNotifyMask);

    // "map" the window (that is, make it appear on the screen)
    XMapWindow (dpy_, win_);

    // create a "graphics context"
    gc_ = XCreateGC (dpy_, win_, 0, NULL);

    // wait for the MapNotify event
    XEvent event;
    while ( event.type != MapNotify )
    {
      XNextEvent (dpy_, &event);
    }

    // get default colormap
    cmap_ = DefaultColormap (dpy_, scr_);

    // get black and white, to determine the color range in RGB
    
    XcmsCCC ccc = XcmsCCCOfColormap (dpy_, cmap_);

    XcmsColor black;
    XcmsColor white;

    XcmsQueryBlack (ccc, XcmsRGBFormat, &black);
    XcmsQueryWhite (ccc, XcmsRGBFormat, &white);

    // colormap range vars.  This should be obtained form the cmap, but well...
    int    min   = black.spec.RGB.red;
    int    max   = white.spec.RGB.red;
    double delta = (max - min) / (cnum_ - 1);

    // fill colormap with nice colors
    //
    // TODO: colormap should actually be filled logarithmic or so, as the
    // mandelbrotset values raise later, but fairly steep, so the image is rather
    // dark when drawn with a linear color map
    for ( unsigned int c = 0; c < cnum_; c++ )
    {
      XcmsColor color;

      color.format         = XcmsRGBFormat;
      color.spec.RGB.red   = min;
      color.spec.RGB.green = min + delta * c;
      color.spec.RGB.blue  = min + delta * c;

      if ( XcmsAllocColor (dpy_, cmap_, &color, XcmsRGBFormat) == XcmsFailure )
      {
        throw "Oops";
      }

      // remember the color index for later use
      colors_.push_back (color.pixel);
    }

    // store balck and white at end of vector, used for box boundaries
    colors_.push_back (BlackPixel (dpy_, scr_));
    colors_.push_back (WhitePixel (dpy_, scr_));

    // allow exposure events to be catched
    XSelectInput (dpy_, win_, ExposureMask);

    // map window to the screen
    XMapWindow   (dpy_, win_);
  }
}

output_x11::~output_x11 (void)
{
  if ( ! fallback_ )
  {
    // shut down X11
    XFreeGC        (dpy_, gc_);
    XDestroyWindow (dpy_, win_);
    XCloseDisplay  (dpy_);
  }
}


// paint a rectangular box.  The given data determine the color index of the
// pixels to be drawn.  The identifier determines the box's label to be printed.
void output_x11::paint_box (unsigned int x0, unsigned int n_x, 
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
      if ( ! fallback_ )
      {
        // set paint color according to data value
        // (first two colors are reserved
        XSetForeground (dpy_, gc_, colors_[line[y] % (cnum_ - 2) + 2]);

        // draw the point at given coordinates
        XDrawPoint     (dpy_, win_, gc_, 
                        x0 + x, 
                        y0 + y);
      }
    }
  }

  if ( DEMO )
  {
    if ( ! fallback_ )
    {
      // for demo purposes, we also draw box boundaries
      for ( unsigned int bx = 0; bx < n_x; bx++ )
      {
        XSetForeground (dpy_, gc_, colors_[cnum_ + (bx % 2)]);
        XDrawPoint     (dpy_, win_, gc_, x0 + bx, y0 + 0);
        XDrawPoint     (dpy_, win_, gc_, x0 + bx, y0 + n_y);
      }

      for ( unsigned int by = 0; by < n_y; by++ )
      {
        XSetForeground (dpy_, gc_, colors_[cnum_ + (by % 2)]);
        XDrawPoint     (dpy_, win_, gc_, x0 + 0,   y0 + by);
        XDrawPoint     (dpy_, win_, gc_, x0 + n_x, y0 + by);
      }
    }

    // print identifier as box label
    if ( ! fallback_ )
    {
      std::string tmp (ident);
      int         len = tmp.size ();

      if ( tmp.size () > 53 )
      {
        tmp[51] = '.';
        tmp[52] = '.';
        tmp[53] = '.';
        len     = 53;
      }


      XSetForeground (dpy_, gc_, WhitePixel (dpy_, scr_));
      XDrawString    (dpy_, win_, gc_, x0 + 10, y0 + 20, 
                      tmp.c_str (), len);
    }
    else
    {
      std::cout << " draw box for " << ident << std::endl;
    }
  }

  if ( ! fallback_ )
  {
    // flush window contents to make sure everything gets drawn
    XFlush (dpy_);
  }
}

