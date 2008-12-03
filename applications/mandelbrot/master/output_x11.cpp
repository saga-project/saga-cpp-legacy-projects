
#include <sstream>
#include <iostream>

#include "output_x11.hpp"

output_x11::output_x11 (int size_x, 
                        int size_y)
    : size_x_ (size_x),
      size_y_ (size_y)
{
  std::cout << "init x11 output\n";

  // Open the display
  dpy_ = XOpenDisplay (NULL);

  // get default screen
  scr_ = DefaultScreen (dpy_);

  // get root window
  root_ = DefaultRootWindow (dpy_);

  // create window
  posx_ = 10;
  posy_ = 10;

  // Create the window
  win_ = XCreateSimpleWindow (dpy_, root_, posx_, posy_,
                              size_x_,
                              size_y_,
                              0,
                              BlackPixel (dpy_, scr_), 
                              BlackPixel (dpy_, scr_));

  // We want to get MapNotify events
  XSelectInput (dpy_, win_, StructureNotifyMask);

  // "Map" the window (that is, make it appear on the screen)
  XMapWindow (dpy_, win_);

  // Create a "Graphics Context"
  gc_ = XCreateGC (dpy_, win_, 0, NULL);

  // Wait for the MapNotify event
  XEvent event;
  while ( event.type != MapNotify )
  {
    XNextEvent (dpy_, &event);
  }

  // get colormap
  cmap_ = DefaultColormap (dpy_, scr_);

  int    min   = 0;
  int    max   = 65536;
  double delta = (max - min) / (C_NUM - 1);

  // fill colormap with nice colors
  for ( int c = 0; c < C_NUM; c++ )
  {
    XcmsColor color;

    color.format         = XcmsRGBFormat;
    color.spec.RGB.red   = min;
    color.spec.RGB.green = min + delta * c;
    color.spec.RGB.blue  = min;

    if ( XcmsAllocColor (dpy_, cmap_, &color, XcmsRGBFormat) == XcmsFailure )
    {
      throw "Oops";
    }

    colors_.push_back (color.pixel);
  }

  // store balck and white at end of vector
  colors_.push_back (BlackPixel (dpy_, scr_));
  colors_.push_back (WhitePixel (dpy_, scr_));

  // allow exposure events to be catched
  XSelectInput (dpy_, win_, ExposureMask);

  // map window to the screen
  XMapWindow   (dpy_, win_);
}

output_x11::~output_x11 (void)
{
  control ();
}


void output_x11::paint_box (int x0, int n_x, 
                            int y0, int n_y,
                            std::vector <std::vector <int> > & data, 
                            std::string ident)
{
  if ( data.size ()    < 1 || 
       data[0].size () < 1 )
  {
    return;
  }

  if ( data.size () != n_x )
  {
    std::stringstream ss;
    ss << "incorrect box size (x): " << data.size () << " - " << n_x;
    throw ss.str ();
  }

  for ( int x = 0; x < n_x; x++ )
  {
    std::vector <int> line = data[x];

    if ( line.size () != n_y )
    {
      std::stringstream ss;
      ss << "incorrect line size (x): " << line.size () << " - " << n_y;
      throw ss.str ();
    }

    for ( int y = 0; y < n_y; y++ )
    {
      // set paint color according to data value
      // (first two colors are reserved
      XSetForeground (dpy_, gc_, colors_[line[y] % (C_NUM - 2) + 2]);

      // draw the point at given coordinates
      XDrawPoint     (dpy_, win_, gc_, 
                      x0 + x, 
                      y0 + y);
    }
  }

  // for demo purposes, we also draw box boundaries
  for ( int bx = 0; bx < n_x; bx++ )
  {
    if ( bx % 2 )
    {
      XSetForeground (dpy_, gc_, colors_[C_NUM + 0]);
    }
    else
    {
      XSetForeground (dpy_, gc_, colors_[C_NUM + 1]);
    }

    XDrawPoint     (dpy_, win_, gc_, 
                    x0 + bx, 
                    y0 + 0);
    XDrawPoint     (dpy_, win_, gc_, 
                    x0 + bx, 
                    y0 + n_y);
  }

  for ( int by = 0; by < n_y; by++ )
  {
    if ( by % 2 )
    {
      XSetForeground (dpy_, gc_, colors_[C_NUM + 0]);
    }
    else
    {
      XSetForeground (dpy_, gc_, colors_[C_NUM + 1]);
    }

    XDrawPoint     (dpy_, win_, gc_, 
                    x0 + 0, 
                    y0 + by);
    XDrawPoint     (dpy_, win_, gc_, 
                    x0 + n_x, 
                    y0 + by);
  }

  // draw signature
  XSetForeground (dpy_, gc_, WhitePixel (dpy_, scr_));
  XDrawString    (dpy_, win_, gc_, x0 + 10, y0 + 20, 
                  ident.c_str (), ident.length ());

  // make sure everything gets drawn
  XFlush (dpy_);
}


// TODO: control should run in it's own thread.  paint_box() should draw into
// a pixmap, and control should map that pixmap into the win on refresh
void output_x11::control (void)
{
  XEvent event;

  while (1) 
  {
    XNextEvent (dpy_, &event);

    switch (event.type) 
    {
      case Expose:
        // refresh image here
        break;
    }
  }
}  

