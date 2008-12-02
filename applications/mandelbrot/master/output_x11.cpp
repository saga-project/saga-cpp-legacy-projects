
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

  // allow exposure events to be catched
  XSelectInput (dpy_, win_, ExposureMask);

  // map window to the screen
  XMapWindow   (dpy_, win_);
}

output_x11::~output_x11 (void)
{
}


void output_x11::paint_box (int x0, int n_x, 
                            int y0, int n_y,
                            std::vector <std::vector <int> > & data)
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
      XSetForeground (dpy_, gc_, colors_[line[y] % C_NUM]);

      // draw the point at given coordinates
      XDrawPoint     (dpy_, win_, gc_, 
                      x0 + x, 
                      y0 + y);
    }
  }
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

