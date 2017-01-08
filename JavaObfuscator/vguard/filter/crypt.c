
#include "crypt.h"

/*

#define KEY_SIZE 32



static UCHAR S[256], iii, jjj, ttt;
static 	char  frameKey[ KEY_SIZE + sizeof(long)];



// Generate a random password key for encryption/decryption
void gen_rand(char* randBuf, int numChar) 
{
	srand(time(NULL));
    for (int i=0; i < numChar; ++i)
    {
		randBuf[i] = rand() % ('~' - '!') + '!';
    }

    return;
}

void init_rand(const char *key, int keylen)
{
    int keypos = 0, x;
    UCHAR K[256];
    for ( x=0; x<256; ++x ) {
        S[x] = x;
        K[x] = key[keypos++];
        if ( keypos >= keylen ) keypos=0;
    }
	
    for ( jjj=x=0; x<256; ++x ) {
        jjj += S[x] + K[x];
        ttt = S[x];
        S[x] = S[jjj];
        S[jjj] = ttt;
    }
	
    iii = jjj = 0;
}



inline UCHAR next_rand()
{
    ++iii;
    jjj += S[iii];
    ttt = S[iii];
    S[iii] = S[jjj];
    S[jjj] = ttt;
    ttt = S[iii] + S[jjj];
    return S[ttt];
}


// for both encryption and decryption
void crypt_buffer(char *p, int len, int offset)
{
	int i=0;
	char c;

	init_rand(frameKey,KEY_SIZE+sizeof(long));  
	for(i=0; i<len; i++)  
	{
		c=next_rand();
		*p= *p^c;
		p++;
	}
}

*/


// for both encryption and decryption
void crypt_buffer_demo( char* p, int len, int offset)
{
	int i=0;
	char c = 1;

	for(i=0; i<len; i++)  
	{
		*p ^= c;
		p++;
	}
}



void DecryptBuffer(char* p, int len, int offset)
{
  crypt_buffer_demo( p, len, offset);
}


void EncryptBuffer(char* p, int len, int offset)
{
  crypt_buffer_demo( p, len, offset);
}



