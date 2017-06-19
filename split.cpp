#include <windows.h>
#include <stdio.h>

#define MIN(a,b) (((a)<(b))?(a):(b))

HANDLE newFile( size_t cpt ) 
{
	HANDLE hOut ;
	cpt++ ;
	TCHAR szOutputFilename[MAX_PATH];
	sprintf( szOutputFilename, "%05d", cpt );
	hOut = ::CreateFile( szOutputFilename,
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL );
	if ( hOut == INVALID_HANDLE_VALUE )
	{
		fprintf( stderr, "Err create output file\n" );
		return INVALID_HANDLE_VALUE ;
	}
	printf("File %s...\n", szOutputFilename);
	return hOut ;
}


BOOL byFile( HANDLE hIn, int size ) 
{
	const int chunk_size = 1024 * size ;
	const int BUFF_SIZE = 65535;

	size_t cpt = 0 ;

	HANDLE hOut = newFile(cpt) ;

	size_t to_process = chunk_size ;

	char* buff = new char[BUFF_SIZE] ;
	DWORD toRead = BUFF_SIZE ;
	DWORD read ;
	while( ::ReadFile( hIn, buff, toRead, &read, NULL ) ) {

		DWORD written ;
		if ( ::WriteFile( hOut, buff, read, &written, NULL ) == FALSE )
		{
			fprintf( stderr, "Err write output file\n" );
			return FALSE ;
		}

		if ( read != toRead )
			break ; //eof

		to_process -= read ;

		if ( to_process == 0 )
		{
			::CloseHandle(hOut);
			hOut = newFile(++cpt);
			to_process = chunk_size ;
			toRead = BUFF_SIZE ;
		}

		//take min value to nxt read
		toRead = MIN( to_process, toRead );
	}

	delete []buff ;
	::CloseHandle(hOut);
	return TRUE ;
}

int main(int argc, char** argv)
{
	if ( argc != 3 )
	{
		printf("Program usage:\n\t split.exe input_filename size\nWhere size are in kBytes\n");
		return 1 ;
	}

	HANDLE hIn = ::CreateFile( argv[1],
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL );

	size_t chunk_size = atoi( argv[2] );

	if( hIn == INVALID_HANDLE_VALUE ) {
		fprintf( stderr, "Error input file\n");
		return 1 ;
	}

	byFile( hIn, chunk_size );

	::CloseHandle( hIn );

	return 0;
}