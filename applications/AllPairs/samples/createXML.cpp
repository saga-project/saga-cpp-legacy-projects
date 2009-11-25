#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

int getdir (string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL) {
        string temp(string(dirp->d_name));
        if(temp.compare(".") == 0 || temp.compare("..") == 0 || temp[0] == '.' || temp.compare("upload.sh") == 0) {
        } else {
           files.push_back(string(dirp->d_name));
        }
    }
    sort(files.begin(), files.end());
    closedir(dp);
    return 0;
}

void my_print(string i)
{
   if(i.compare(".") == 0 || i.compare("..") == 0) {
   } else {
      cout << i << endl;
   }
}

int main(int argc, char **argv)
{
   vector<string> list;
   if(argc < 5)
   {
      cout << "Usage: ./createXML -s scheme -d folder-to-data-files" << endl;
      return -1;
   }
   string scheme(argv[2]);
   string dir_name(argv[4]);
   if(getdir(dir_name, list) != 0) {
      cerr << "Unable to find directory" << endl;
   }
   else {
      for(vector<string>::const_iterator it = list.begin(); it != list.end(); it++) {
         cout << "   <CompareAssignment>" << endl;
         for(vector<string>::const_iterator jt = list.begin(); jt != list.end(); jt++) {
               cout << "     <Compare source=\"" << scheme << dir_name + "/" + *it << "\"" << endl;
               cout << "              target=\"" << scheme << dir_name + "/" + *jt << "\"/>" << endl;
         }
         cout << "   </CompareAssignment>" << endl;
      }
   }
   return 0;
}
