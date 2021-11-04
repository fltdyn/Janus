#ifndef ACRYPT_H_
#define ACRYPT_H_

#include "UteConfig.h"
#ifdef HAVE_OPENSSL

//
// DST Ute Library (Utilities Library)
//
// Defence Science and Technology (DST) Group
// Department of Defence, Australia.
// 506 Lorimer St
// Fishermans Bend, VIC
// AUSTRALIA, 3207
//
// Copyright 2005-2018 Commonwealth of Australia
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this
// software and associated documentation files (the "Software"), to deal in the Software
// without restriction, including without limitation the rights to use, copy, modify,
// merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be included in all copies
// or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
// OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//

//
// Title: aCrypt
// Class: aCrypt
//
// Description:
//   Encrypts/decrypts data using OpenSSL.
//
// Written by: Shane Hill  (Shane.Hill@dsto.defence.gov.au)
//
// First Date: 07/07/2014
//

// C/C++ Includes

// Local Includes
#include "aString.h"
#include "UteConfig.h"

// OpenSSL Includes.
#include <openssl/evp.h>
#include <openssl/rsa.h>

namespace dstoute {

  const size_t AES_KEYLEN_BITS = 256;
  const size_t AES_KEYLEN      = AES_KEYLEN_BITS / 8;
  const size_t ENCRYPT_BUFFER  = 1024; // Buffer to allow encryption/decryption to same buffer.

  class aCrypt {
   public:
    aCrypt();
    ~aCrypt();

    static RSA* generateRandomRsaKeys();
    static void writeRsaKeys( const aString &baseFileName);
    static void writeFile(  const aString &fileName, const unsigned char *fileData, size_t fileDataLength, bool doAppend = false);
    static void appendFile( const aString &fileName, const unsigned char *fileData, size_t fileDataLength)
      { writeFile( fileName, fileData, fileDataLength, true);}
    static size_t readFile( const aString &fileName, unsigned char* &fileData);

    static aString md5hash( const unsigned char *buffer, size_t bufferSize);
    static aString sha256hash( const unsigned char *buffer, size_t bufferSize);

    void setRsaKeys( RSA *rsaKeys);
    void setPrivateRsaKey( const aString &fileName);
    void setPublicRsaKey( const aString &fileName);

    void generateRandomAesKey();

    size_t rsaEncryptBuffer( const unsigned char *decData, size_t decDataLength, unsigned char* &encData);
    void   rsaEncryptFile( const aString &inFileName, const aString &outFileName = aString());
    size_t rsaDecryptBuffer( const unsigned char *encData, size_t encDataLength, unsigned char* &decData);
    void   rsaDecryptFile( const aString &inFileName, const aString &outFileName = aString());

    size_t aesEncrypt( const unsigned char *decData,  int decDataLength,  unsigned char* &encData);
    size_t aesDecrypt( const unsigned char *encData, int encDataLength, unsigned char* &decData);

   private:
    void initCipher();

    EVP_CIPHER_CTX *encryptCtx_;
    EVP_CIPHER_CTX *decryptCtx_;
    RSA            *rsaKeys_;
    unsigned char  *aesKey_;
  };

} // namespace dstoute

#else
#error "OpenSSL library not compiled into DSTO Ute library."

#endif // HAVE_OPENSSL

#endif // ACRYPT_H_
