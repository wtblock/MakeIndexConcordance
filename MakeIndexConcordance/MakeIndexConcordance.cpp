/////////////////////////////////////////////////////////////////////////////
// Copyright © by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MakeIndexConcordance.h"
#include "CHelper.h"
#include <vector>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object
CWinApp theApp;

/////////////////////////////////////////////////////////////////////////////
// a console application that can crawl through the file
// and build a sorted list of filenames
int _tmain( int argc, TCHAR* argv[], TCHAR* envp[] )
{
	HMODULE hModule = ::GetModuleHandle( NULL );
	if ( hModule == NULL )
	{
		_tprintf( _T( "Fatal Error: GetModuleHandle failed\n" ) );
		return 1;
	}

	// initialize MFC and error on failure
	if ( !AfxWinInit( hModule, NULL, ::GetCommandLine(), 0 ) )
	{
		_tprintf( _T( "Fatal Error: MFC initialization failed\n " ) );
		return 2;
	}

	// do some common command line argument corrections
	vector<CString> arrArgs = CHelper::CorrectedCommandLine( argc, argv );
	const size_t nArgs = arrArgs.size();

	// this stream can be redirected from the command line to allow the 
	// output you are interested in to be captured into another file
	// (Ex. > out_file.csv)
	CStdioFile fOut( stdout );

	// this stream is not redirected; it only shows up on the console and
	// will not affect the output file that is being redirected to
	CStdioFile fErr( stderr );

	CString csMessage;

	// display the number of arguments if not 1 to help the user 
	// understand what went wrong if there is an error in the
	// command line syntax
	if ( nArgs != 1 )
	{
		fErr.WriteString( _T( ".\n" ) );
		csMessage.Format( _T( "The number of parameters are %d\n.\n" ), nArgs - 1 );
		fErr.WriteString( csMessage );

		// display the arguments
		for ( int i = 1; i < nArgs; i++ )
		{
			csMessage.Format( _T( "Parameter %d is %s\n.\n" ), i, arrArgs[ i ] );
			fErr.WriteString( csMessage );
		}
	}

	// two arguments expected
	if ( nArgs != 2 )
	{
		fErr.WriteString( _T( ".\n" ) );
		fErr.WriteString
		(
			_T( "MakeIndexConcordance, Copyright (c) 2022, " )
			_T( "by W. T. Block.\n" )
		);

		fErr.WriteString
		(
			_T( ".\n" )
			_T( "A Windows command line program to read an index\n" )
			_T( "  text file and output a concordance text\n" )
			_T( "  file that Microsoft Word can used to build\n" )
			_T( "  an index. \n" )
			_T( ".\n" )
			_T( "The input file is in this format:\n" )
			_T( "  last_name, first_name\n" )
			_T( "which will be the format displayed in the index.\n" )
			_T( ".\n" )
			_T( "The output format is as follows:\n" )
			_T( "  first_name last_name<tab>last_name, first_name\n" )
			_T( ".\n" )
			_T( "The output can be pasted into a blank Microsoft " )
			_T( "  Word document which can then be converted into \n" )
			_T( "  a table.\n" )
			_T( ".\n" )
		);

		fErr.WriteString
		(
			_T( ".\n" )
			_T( "Usage:\n" )
			_T( ".\n" )
			_T( ".  MakeIndexConcordance input_file\n" )
			_T( ".\n" )
			_T( "Where:\n" )
			_T( ".\n" )
			_T( ".  input_file is the pathname of the input file.\n" )
			_T( ".\n" )
		);

		return 3;
	}

	// display the executable path
	csMessage.Format( _T( "Executable pathname: %s\n" ), arrArgs[ 0 ] );
	fErr.WriteString( _T( ".\n" ) );
	fErr.WriteString( csMessage );
	fErr.WriteString( _T( ".\n" ) );

	// retrieve the pathname
	const CString csInput = arrArgs[ 1 ];

	// check for file existence
	bool bExists = false;
	if ( ::PathFileExists( csInput ) )
	{
		bExists = true;
	}
	
	// error out if the file does not exist
	if ( !bExists )
	{
		csMessage.Format( _T( "Invalid input file:\n\t%s\n" ), csInput );
		fErr.WriteString( _T( ".\n" ) );
		fErr.WriteString( csMessage );
		fErr.WriteString( _T( ".\n" ) );
		return 4;

	}
	else // display the name of the file input
	{
		csMessage.Format( _T( "Given input:\n\t%s\n" ), csInput );
		fErr.WriteString( _T( ".\n" ) );
		fErr.WriteString( csMessage );
	}

	// open the file and error out on failure
	CStdioFile file;
	if ( !file.Open( csInput, CFile::modeRead | CFile::typeText ) )
	{
		return 5;
	}
	
	// crawl through the input file parsing the names
	// and build a corresponding line of output for each
	// line of output
	do
	{
		CString csLine;
		if ( !file.ReadString( csLine ) )
		{
			break;
		}

		vector<CString> Output;
		const CString csDelim( _T( "," ));
		int nStart = 0;
		do
		{
			CString csToken = csLine.Tokenize( csDelim, nStart );
			if ( csToken.IsEmpty() )
			{
				break;
			}

			// store the token without leading and trailing
			// whitespace
			Output.push_back( csToken.Trim() );

		} while ( true );

		const size_t nTokens = Output.size();

		// ignore output less than two tokens
		if ( nTokens < 2 )
		{
			continue;
		}

		// build the first column of the concordance
		// which contains the text to be indexed
		CString csColumn1;

		// add the trailing tokens first (first and middle
		// names typically)
		for ( size_t nToken = 1; nToken < nTokens; nToken++ )
		{
			const CString csToken = Output[ nToken ];
			csColumn1 += csToken;
			csColumn1 += _T( " " );
		}

		// add the last name last
		csColumn1 += Output[ 0 ];

		// build the output string from column 1 and
		// the original input line separated by a tilde
		// character (~) and terminated with a line feed
		CString csOut;
		csOut.Format( _T( "%s~%s\n" ), csColumn1, csLine );

		// write the output line to the console (which
		// can be redirected to another text file)
		fOut.WriteString( csOut );
		
	} while ( true );

	// all is good
	return 0;

} // _tmain
