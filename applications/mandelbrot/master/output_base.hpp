
#ifndef OUTPUT_BASE_HPP
#define OUTPUT_BASE_HPP

#include <vector>

class output_base
{
  public:
    virtual void paint_box (int x0, int n_x, 
                            int y0, int n_y,
                            std::vector <std::vector <int> > & data,
                            std::string ident = "") = 0;
    virtual void control (void) = 0;
};

#endif // OUTPUT_BASE_HPP

