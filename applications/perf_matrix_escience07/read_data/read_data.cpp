#include <algorithm>
#include <iomanip>
#include <map>
#include <saga.hpp>
#include <fstream>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

typedef std::map<std::string, std::string, std::less<std::string> > matrix_map_t;
typedef std::pair<std::string, std::string> matrix_pair_t;

///////////////////////////////////////////////////////////////////////////////
bool parse_commandline(int argc, char *argv[], po::variables_map& vm)
{
    try 
    {
        po::options_description desc_cmdline ("Usage: read_data [options]");
        
        desc_cmdline.add_options()
          ("help,h", "print out program usage (this message)")
          ("contact,c", po::value<std::string>(), "advert-service contact string")
          ("fileid,f", po::value<std::string>(), "ouput filename identifier creates gnuplot-<fileid>.data and gnuplot-<fileid>.config");
        po::positional_options_description p;

        po::store(po::parse_command_line(argc, argv, desc_cmdline), vm);
        po::notify(vm);

        // verify all needed arguments are given
        if (!vm.count("fileid")) {
            std::cerr << "Missing ouput file identifier: use --fileid STRING" 
                      << std::endl;
            return false;
        }
        
        if (!vm.count("contact")) {
            std::cerr << "Missing advert-service contact string: use --contact STRING" 
                      << std::endl;
            return false;
        }

        // print help screen
        if (vm.count("help")) {
            std::cout << desc_cmdline;
            return false;
        }
    }
    catch (std::exception const& e) {
        std::cerr << "read_data: exception caught: " << e.what() << std::endl;
        return false;
    }
    return true;
}

int main (int argc, char* argv[])
{
    // analyze the command line
    po::variables_map vm;
    if (!parse_commandline(argc, argv, vm))
        return -2;
    
    std::ofstream datafile;
    std::ofstream configfile;

    std::string df_name = "gnuplot-"+vm["fileid"].as<std::string>()+".data";
    std::string cf_name = "gnuplot-"+vm["fileid"].as<std::string>()+".config";
 
    try {
	datafile.open(df_name.c_str());
	configfile.open(cf_name.c_str());
    } 
    catch(std::exception const & e)
    {
	std::cerr << "Error creating ouput files: " << e.what() << std::endl;
    }

    std::string advert_contact (vm["contact"].as<std::string>());
    int mode = saga::advert::Read;
    bool header_written = false;
 
    // write the essential parts of the config file
    configfile << "set xdata time" << std::endl;
    configfile << "set timefmt \"%Y-%m-%d-%H:%M:%S\"" << std::endl;
    configfile << "set grid" << std::endl;
    configfile << "set ylabel \"Throughput (10^6 bits/sec)\"" << std::endl;
    configfile << "set xlabel \"Time\"" << std::endl;
    configfile << "set title \"" << df_name << "\"" << std::endl;
    configfile << "set key bottom below" << std::endl;

    try {

    saga::advert_directory base_dir(advert_contact, mode); 
    std::vector<std::string> dir_content = base_dir.list();

    std::vector<std::string>::const_iterator it;
    for (it = dir_content.begin(); it != dir_content.end(); ++it)
    {
        if (base_dir.is_dir(advert_contact+"/"+(*it)))
        {
            saga::advert_directory subdir(advert_contact+"/"+(*it));
            std::vector<std::string> dir_content = subdir.list();
        
            matrix_map_t matrix;
	    std::string row_time;
            bool row_time_set = false;
 
	    std::vector<std::string>::const_iterator sub_it;
            for (sub_it = dir_content.begin(); sub_it != dir_content.end(); ++sub_it)
            {
		if (base_dir.is_entry(advert_contact+"/"+(*it)+"/"+(*sub_it)))
                {
		    saga::advert entry(advert_contact+"/"+(*it)+"/"+(*sub_it));

		    if(!row_time_set)
		    {
			row_time = entry.get_attribute("ctime");
		        row_time = row_time.substr( 0, row_time.find_first_of(".") );
                        //row_time = row_time.substr( 0, row_time.find_last_of(":") );
                        row_time.replace( row_time.find_first_of(" "), 1, "-" );
         	    }
                    std::string key = entry.get_attribute("description");
                    
		    matrix.insert(matrix_pair_t(key,entry.retrieve_string()));
		}
            }

	    if(!header_written)
	    {
		configfile << "plot ";
         	datafile << std::left << std::setw(22) << "# Timestamp" << std::endl;
		int counter = 2;
	        for(matrix_map_t::const_iterator p = matrix.begin(); p!=matrix.end(); ++p)
		{
		    datafile << std::left << "# " << std::setw(8) <<  p->first << std::endl;
		    
		    // write plot() commands to config file
		    configfile << "\"" << df_name << "\" using 1:" << counter  
			       << " title \"" << p->first << "\" with points" ;
		    if(counter <= matrix.size()) configfile << ", " << std::flush;
 
                    ++counter;
		}
		datafile << std::endl << std::endl;
		header_written = true;
	    }
            std::cout << "...iterating" << std::endl;
            datafile << std::setw(22) << row_time;
		
            for(matrix_map_t::const_iterator p = matrix.begin(); p!=matrix.end(); ++p)
		datafile << std::left << std::setw(10) << atof((p->second).c_str()) ;
            datafile << std::endl;
        
        } // matrix iterator loop
    } // root dir iterator loop
    
    } catch(saga::exception const & e) {
        std::cerr << "Something went wrong: " << e.what() << std::endl;
    } 
 
    std::cout << std::endl << "Writing files DONE." <<  std::endl;
    datafile.close();
    configfile.close();

    return 0;
}


 
