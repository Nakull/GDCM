/*=========================================================================

  Program: GDCM (Grassroots DICOM). A DICOM library
  Module:  $URL$

  Copyright (c) 2006-2009 Mathieu Malaterre
  All rights reserved.
  See Copyright.txt or http://gdcm.sourceforge.net/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __gdcmAES_h
#define __gdcmAES_h

#include "gdcmTypes.h"

namespace gdcm
{
/**
 * \brief Class for AES encryption
 *
http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf

MODES OF OPERATION

Electronic Code Book (ECB)
    ECB is the basic mode of all block ciphers. Each block is encrypted independently and so identical plain text will produce identical output when encrypted with the same key. Any encryption errors will only affect a single block however this is vulnerable to known plaintext attacks. 
Cipher Block Chaining (CBC)
    CBC mode uses the output of the last block encryption to affect the current block. An initialization vector of the same size as the cipher block size is used to handle the first block. The initialization vector should be chosen randomly and transmitted as the first block of the output. Errors in encryption affect the current block and the next block after which the cipher will correct itself. CBC is the most commonly used mode in software encryption. 

 * \todo: should I reject CryptCbc( DECRYPT ) when SetkeyDec was not called (SetkeyEnc)

 */
//-----------------------------------------------------------------------------
class AESInternals;
class GDCM_EXPORT AES
{
public :
  AES();
  ~AES();

  typedef enum {
/* #define AES_*/ ENCRYPT = 1,
/* #define AES_*/ DECRYPT = 0
  } AESModeType;


/**
 * \brief          AES key schedule (encryption)
 *
 * \param key      encryption key
 * \param keysize  must be 16, 24 or 32
 * \return         false on error (wrong keysize, key null)
 */
bool SetkeyEnc(const unsigned char *key, unsigned int keysize);

/**
 * \brief          AES key schedule (decryption)
 *
 * \param key      decryption key
 * \param keysize  must be 16, 24 or 32
 * \return         false on error (wrong keysize, key null)
 */
bool SetkeyDec(const unsigned char *key, unsigned int keysize );

/**
 * \brief          AES-ECB block encryption/decryption
 *
 * \param mode     ENCRYPT or DECRYPT
 * \param input    16-byte input block
 * \param output   16-byte output block
 * \return         false on error (invalid key)
 */
bool CryptEcb(
                    int mode,
                    const unsigned char input[16],
                    unsigned char output[16] ) const;

/**
 * \brief          AES-CBC buffer encryption/decryption
 *
 * \param mode     ENCRYPT or DECRYPT
 * \param length   length of the input data
 * \param iv       initialization vector (updated after use)
 * \param input    buffer holding the input data
 * \param output   buffer holding the output data
 * \return         false on error (invalid key, or length not multiple 16)
 */
bool CryptCbc(
                    int mode,
                    int length,
                    unsigned char iv[16],
                    const unsigned char *input,
                    unsigned char *output ) const;

/**
 * \brief          AES-CFB128 buffer encryption/decryption
 *
 * \param mode     ENCRYPT or DECRYPT
 * \param length   length of the input data
 * \param iv_off   offset in IV (updated after use)
 * \param iv       initialization vector (updated after use)
 * \param input    buffer holding the input data
 * \param output   buffer holding the output data
 * \return         false on error (invalid key)
 */
bool CryptCfb128(
                       int mode,
                       int length,
                       int& iv_off,
                       unsigned char iv[16],
                       const unsigned char *input,
                       unsigned char *output ) const;

  void operator=(const AES&);

protected:
/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed (or not compiled in)
 */
int SelfTest( int verbose = 0 ) const;
/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed 
 */
int SimpleTest(int verbose = 0) const;
private:
AESInternals *Internals;
private:
  AES(const AES&);  // Not implemented.
};
} // end namespace gdcm
//-----------------------------------------------------------------------------
#endif //__gdcmAES_h