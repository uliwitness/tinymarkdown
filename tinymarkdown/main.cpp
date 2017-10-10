//
//  main.cpp
//  tinymarkdown
//
//  Created by Uli Kusterer on 10.10.17.
//  Copyright © 2017 Uli Kusterer. All rights reserved.
//

#include "tinymarkdown.hpp"
#include <iostream>

int main( int argc, const char * argv[] )
{
	FILE* markdownFile = fopen( argv[1], "r" );
	fseek( markdownFile, 0, SEEK_END );
	long fileLength = ftell( markdownFile );
	char* markdownStr = (char*) malloc( fileLength +1 );
	fseek( markdownFile, 0, SEEK_SET );
	fread( markdownStr, 1, fileLength, markdownFile );
	fclose( markdownFile );
	markdownStr[fileLength] = '\0';
	
	tinymarkdown::parser	parser;
	
	std::cout << parser.parse( markdownStr ) << std::endl;
	
	free(markdownStr);

	return 0;
}
