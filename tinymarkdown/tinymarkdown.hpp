//
//  tinymarkdown.hpp
//  tinymarkdown
//
//	Copyright 2017 by Uli Kusterer.
//
//	This software is provided 'as-is', without any express or implied
//	warranty. In no event will the authors be held liable for any damages
//	arising from the use of this software.
//
//	Permission is granted to anyone to use this software for any purpose,
//	including commercial applications, and to alter it and redistribute it
//	freely, subject to the following restrictions:
//
//	1. The origin of this software must not be misrepresented; you must not
//	claim that you wrote the original software. If you use this software
//	in a product, an acknowledgment in the product documentation would be
//	appreciated but is not required.
//
//	2. Altered source versions must be plainly marked as such, and must not be
//	misrepresented as being the original software.
//
//	3. This notice may not be removed or altered from any source
//	distribution.
//

#ifndef tinymarkdown_hpp
#define tinymarkdown_hpp

#include <string>

namespace tinymarkdown
{
	
	const std::string lineBreak = "<br />\n";
	
	class parser
	{
	public:
		std::string parse( std::string inMarkdown )
		{
			std::string	htmlResult;
			std::string	currentLine;
			std::string	previousLine;
			off_t		markdownLength = inMarkdown.length();
			char		previousChar = '\0';
			int			underlineHeaderLevel = 0;
			off_t		italicStartOffset = -1;
			off_t		imageStartOffset = -1;

			for( off_t x = 0; x < markdownLength; )
			{
				char currCh = inMarkdown[x];
				
				if( currCh == '\r' )	// Mac-style CR line ending, or maybe Windows-style CRLF.
				{
					if( underlineHeaderLevel > 0 )
					{
						char openingTag[20] = {};
						snprintf( openingTag, sizeof(openingTag) -1, "<h%d>", underlineHeaderLevel );
						currentLine = openingTag;
						
						size_t lineBreakOffset = previousLine.length() -lineBreak.length();
						if( previousLine.rfind( lineBreak ) == lineBreakOffset )
							previousLine.resize(lineBreakOffset);
						currentLine.append( previousLine );
						
						char closingTag[20] = {};
						snprintf( closingTag, sizeof(openingTag) -1, "</h%d>\n", underlineHeaderLevel );
						currentLine.append( closingTag );

						// Don't write out the previousLine, we incorporated it into our currentLine and would get it twice.
					}
					else
					{
						htmlResult.append( previousLine );
						currentLine.append( lineBreak );
					}
					previousLine = currentLine;
					currentLine = "";
					previousChar = '\r';	// So next LF can be handled specially if this was a CRLF.
					++x;
					underlineHeaderLevel = 0;
					italicStartOffset = -1;
					continue;
				}
				
				if( currCh == '\n' )
				{
					if( previousChar == '\r' )	// Windows-style CRLF sequence
					{
						// We already generated a BR tag for this line break in the CR code.
						previousChar = '\n';
					}
					else	// Unix-style LF.
					{
						if( underlineHeaderLevel > 0 )
						{
							char openingTag[20] = {};
							snprintf( openingTag, sizeof(openingTag) -1, "<h%d>", underlineHeaderLevel );
							currentLine = openingTag;
							
							size_t lineBreakOffset = previousLine.length() -lineBreak.length();
							if( previousLine.rfind( lineBreak ) == lineBreakOffset )
								previousLine.resize(lineBreakOffset);
							currentLine.append( previousLine );
							
							char closingTag[20] = {};
							snprintf( closingTag, sizeof(openingTag) -1, "</h%d>\n", underlineHeaderLevel );
							currentLine.append( closingTag );
							
							// Don't write out the previousLine, we incorporated it into our currentLine and would get it twice.
						}
						else
						{
							htmlResult.append( previousLine );
							currentLine.append( lineBreak );
						}
						previousLine = currentLine;
						currentLine = "";
						underlineHeaderLevel = 0;
					}
					italicStartOffset = -1;
					++x;
					continue;
				}
				
				if( currCh == '=' )	// Underlines previous line's text? Possibly start of headline!
				{
					currentLine.append("=");
					if( underlineHeaderLevel == 0 )
						underlineHeaderLevel = 1;
					++x;
				}
				else if( currCh == '-' )	// Underlines previous line's text? Possibly start of headline!
				{
					currentLine.append("-");
					if( underlineHeaderLevel == 0 )
						underlineHeaderLevel = 2;
					++x;
				}
				else if( currCh == '*' )
				{
					if( italicStartOffset > -1 )	// End of italic sequence.
					{
						currentLine.replace(italicStartOffset, 1, "<i>");
						currentLine.append("</i>");
						italicStartOffset = -1;
					}
					else
					{
						italicStartOffset = currentLine.length();
						currentLine.append("*");
					}
					underlineHeaderLevel = -1;

					++x;
				}
				else if( currCh == '<' )
				{
					currentLine.append( "&lt;" );
					underlineHeaderLevel = -1;
					++x;
				}
				else if( currCh == '>' )
				{
					currentLine.append( "&gt;" );
					underlineHeaderLevel = -1;
					++x;
				}
				else if( currCh == '!' )
				{
					imageStartOffset = currentLine.length();
					currentLine.append( "!" );
					underlineHeaderLevel = -1;
					++x;
				}
				else if( currCh == ')' && imageStartOffset > -1 )
				{
					size_t imageInfoLength = currentLine.length() -imageStartOffset;
					if( currentLine[imageStartOffset +1] == '[' )	// Valid alt-text! Image!
					{
						std::string imageInfo = currentLine.substr( imageStartOffset +2, imageInfoLength -2 );	// We know it started with "![", no need to extract that.
						size_t	separatorOffset = imageInfo.find("](");
						if( separatorOffset == std::string::npos )	// Not an image link.
							currentLine.append( ")" );
						else
						{
							std::string imageTag( "<img src=\"" );
							imageTag.append( imageInfo.substr( separatorOffset +2, imageInfo.length() -separatorOffset -2 ) );
							if( separatorOffset > 0 )
							{
								imageTag.append( "\" alt=\"" );
								imageTag.append( imageInfo.substr( 0, separatorOffset ) );
							}
							imageTag.append( "\" />" );
							currentLine.replace( imageStartOffset, imageInfoLength, imageTag );
						}
					}
					else	// Not an image, leave untouched, and append the ")" we just saw.
					{
						currentLine.append( ")" );
					}
					underlineHeaderLevel = -1;
					++x;
				}
				else
				{
					currentLine.append( 1, currCh );
					underlineHeaderLevel = -1;
					++x;
				}

				/* Previous iteration saw an asterisk and thought it might start italics
					but there is whitespace after it? It's just an asterisk. */
				if( italicStartOffset > -1 && italicStartOffset == (x -1)
				   && (currCh == ' ' || currCh == '\t' || currCh == '\r' || currCh == '\n') )
				{
					italicStartOffset = -1;
				}
				
				previousChar = currCh;
			}
			
			htmlResult.append( previousLine );
			htmlResult.append( currentLine );

			return htmlResult;
		}
	};
}

#endif /* tinymarkdown_hpp */
