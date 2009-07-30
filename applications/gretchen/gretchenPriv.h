/*
 *  gretchenPriv.h
 *  gretchen
 *
 *  Created by Ole Weidner on 7/25/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

/* The classes below are not exported */
#pragma GCC visibility push(hidden)

class gretchenPriv
{
	public:
		void HelloWorldPriv(const char *);
};

#pragma GCC visibility pop
