/*
 #  Copyright (c) 2010 Katerina Stamou (kstamou@cct.lsu.edu)
 # 
 #  Distributed under the Boost Software License, Version 1.0. (See accompanying
 #  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/


#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <openssl/evp.h>
#include "util/hash.hpp"

namespace digedag
{
  namespace util
  {
        hash::hash (std::string filename)
        { 
		if ( filename.empty() )
		{
		  std::cerr << "filename is blank" << std::endl;
		  exit(-1);
		}

		FILE *fp;
		char *buf;

		if ((fp = fopen(filename.c_str(), "r")) == NULL)
                {
		  std::cerr << "could not open file: " << filename << std::endl;
		  exit(-1);
		}

		EVP_MD_CTX mdctx;
		const EVP_MD *md;
		unsigned char md_value[EVP_MAX_MD_SIZE];
		unsigned int  md_len;

		md = EVP_md5();
		EVP_MD_CTX_init(&mdctx);
		EVP_DigestInit_ex(&mdctx, md, NULL);

		fseek(fp, 0, SEEK_END);
		unsigned int size = ftell(fp);
		rewind(fp);
		buf = (char*) malloc(size);

		//while ( (len = fread(buf, 1, 1024, fp)) )
		fread(buf, 1, size, fp);
		EVP_DigestUpdate(&mdctx, buf, size);

		if (fp != NULL) {
		  fclose(fp);
		}

		EVP_DigestFinal_ex(&mdctx, md_value, &md_len);
		EVP_MD_CTX_cleanup(&mdctx);

		for (unsigned int i = 0; i < md_len; i++) {
		  daghash << std::hex << std::setw(2) << std::setfill('0') << int(md_value[i]);
		}

        }

	hash::~hash (void) { }

        std::string hash::get_hash (void)
        {
           return daghash.str();
	}

  } // namespace util

} // namespace digedag

