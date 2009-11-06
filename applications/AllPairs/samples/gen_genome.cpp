#include <iostream>
#include <cstdlib>
#include <sstream>

int main()
{
   for(int f = 0; f < 16; f++)
   {
      FILE *fp;
      std::string s("genome-");
      std::stringstream out;
      out << f;
      s += out.str();
      fp = fopen(s.c_str(), "w");
      for(int c = 0; c < 300000000; c++)
      {
         int number = rand() % 4;
         if(number == 0)
         {
            fprintf(fp, "%c", 'a');
         }
         else if(number == 1)
         {
            fprintf(fp, "%c", 'c');
         }
         else if (number == 2)
         {
            fprintf(fp, "%c", 't');
         }
         else if (number == 3)
         {
            fprintf(fp, "%c", 'g');
         }
      }
      fclose(fp);
   }
}
