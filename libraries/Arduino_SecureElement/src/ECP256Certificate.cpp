/*
  This file is part of the Arduino_SecureElement library.

  Copyright (c) 2024 Arduino SA

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

/******************************************************************************
 * INCLUDE
 ******************************************************************************/

/* This is needed for memmem */
#define _GNU_SOURCE
#include <string.h>
#include <utility/SElementBase64.h>
#include "ECP256Certificate.h"

/******************************************************************************
 * DEFINE
 ******************************************************************************/

#define ASN1_INTEGER           0x02
#define ASN1_BIT_STRING        0x03
#define ASN1_NULL              0x05
#define ASN1_OBJECT_IDENTIFIER 0x06
#define ASN1_PRINTABLE_STRING  0x13
#define ASN1_SEQUENCE          0x30
#define ASN1_SET               0x31

/******************************************************************************
 * CTOR/DTOR
 ******************************************************************************/

ECP256Certificate::ECP256Certificate()
: _certBuffer(nullptr)
, _certBufferLen(0)
, _publicKey(nullptr)
{

}

ECP256Certificate::~ECP256Certificate() 
{
  if (_certBuffer) {
    free(_certBuffer);
    _certBuffer = nullptr;
  }
}

/******************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 ******************************************************************************/

int ECP256Certificate::begin()
{
  memset(_compressedCert.data, 0x00, sizeof(CompressedCertDataUType));
  return 1;
}

int ECP256Certificate::buildCSR()
{
  int csrInfoLen = CSRInfoLength();
  int subjectLen = issuerOrSubjectLength(_subjectData);

  _certBufferLen = getCSRSize();
  _certBuffer = (byte*)malloc(_certBufferLen);

  if (_certBuffer == nullptr) {
    return 0;
  }

  byte* out = _certBuffer;

  // header
  out += appendSequenceHeader(csrInfoLen, out);

  // version
  out += appendVersion(0x00, out);

  // subject
  out += appendSequenceHeader(subjectLen, out);
  out += appendIssuerOrSubject(_subjectData, out);

  // public key
  if (_publicKey == nullptr) {
    return 0;
  }
  out += appendPublicKey(_publicKey, out);
  
  // terminator
  *out++ = 0xa0;
  *out++ = 0x00;

  return 1;
}

int ECP256Certificate::signCSR(byte * signature)
{
  /* copy old certbuffer in a temp buffer */
  byte* tempBuffer = (byte*)malloc(_certBufferLen);

  if (tempBuffer == nullptr) {
    return 0;
  }

  memcpy(tempBuffer, _certBuffer, _certBufferLen);
  
  _certBufferLen = getCSRSignedSize(signature);
  _certBuffer = (byte*)realloc(_certBuffer, _certBufferLen);

  if (_certBuffer == nullptr) {
    return 0;
  }

  byte* out = _certBuffer;

  // header
  out += appendSequenceHeader(getCSRSize() + signatureLength(signature), out);

  // info
  memcpy(out, tempBuffer, getCSRSize());
  free(tempBuffer);
  out += getCSRSize();

  // signature
  out += appendSignature(signature, out);

  return 1;
}

String ECP256Certificate::getCSRPEM()
{
  return b64::pemEncode(_certBuffer, _certBufferLen, "-----BEGIN CERTIFICATE REQUEST-----\n", "\n-----END CERTIFICATE REQUEST-----\n");
}

int ECP256Certificate::buildCert()
{
  _certBufferLen = getCertSize();
  _certBuffer = (byte*)malloc(_certBufferLen);

  if (_certBuffer == nullptr) {
    return 0;
  }
  
  uint8_t* out = _certBuffer;

  int certInfoLen = certInfoLength();

  // header
  out += appendSequenceHeader(certInfoLen, out);

  // version
  *out++ = 0xA0;
  *out++ = 0x03;
  *out++ = 0x02;
  *out++ = 0x01;
  *out++ = 0x02;

  // serial number
  out += appendSerialNumber(_compressedCert.slot.two.values.serialNumber, ECP256_CERT_SERIAL_NUMBER_LENGTH, out);

  // signature type
  out += appendEcdsaWithSHA256(out);

  // issuer
  int issuerDataLen = issuerOrSubjectLength(_issuerData);
  out += appendSequenceHeader(issuerDataLen, out);
  out += appendIssuerOrSubject(_issuerData, out);

  // dates
  DateInfo dateData;
  getDateFromCompressedData(dateData);

  *out++ = ASN1_SEQUENCE;
  *out++ = 30 + ((dateData.issueYear > 2049) ? 2 : 0) + (((dateData.issueYear + dateData.expireYears) > 2049) ? 2 : 0);
  out += appendDate(dateData.issueYear, dateData.issueMonth, dateData.issueDay, dateData.issueHour, 0, 0, out);
  out += appendDate(dateData.issueYear + dateData.expireYears, dateData.issueMonth, dateData.issueDay, dateData.issueHour, 0, 0, out);

  // subject
  int subjectDataLen = issuerOrSubjectLength(_subjectData);
  out += appendSequenceHeader(subjectDataLen, out);
  out += appendIssuerOrSubject(_subjectData, out);

  // public key
  if (_publicKey == nullptr) {
    return 0;
  }
  out += appendPublicKey(_publicKey, out);

  int authorityKeyIdLen = authorityKeyIdLength(_compressedCert.slot.two.values.authorityKeyId, ECP256_CERT_AUTHORITY_KEY_ID_LENGTH);
  if (authorityKeyIdLen)
  {
    out += appendAuthorityKeyId(_compressedCert.slot.two.values.authorityKeyId, ECP256_CERT_AUTHORITY_KEY_ID_LENGTH, out);
  }
  else
  {
    // null sequence
    *out++ = 0xA3;
    *out++ = 0x02;
    *out++ = 0x30;
    *out++ = 0x00;
  }

  return 1;
}

int ECP256Certificate::signCert(const byte * signature)
{
  /* copy old certbuffer in a temp buffer */
  byte* tempBuffer = (byte*)malloc(_certBufferLen);

  if (tempBuffer == nullptr) {
    return 0;
  }

  memcpy(tempBuffer, _certBuffer, _certBufferLen);
  
  _certBufferLen = getCertSignedSize(signature);
  _certBuffer = (byte*)realloc(_certBuffer, _certBufferLen);

  if (_certBuffer == nullptr) {
    return 0;
  }
  
  byte* out = _certBuffer;

  // header
  out +=appendSequenceHeader(getCertSize() + signatureLength(signature), out);

  // info
  memcpy(out, tempBuffer, getCertSize());
  free(tempBuffer);
  out += getCertSize();

  // signature
  out += appendSignature(signature, out);

  return 1;
}

int ECP256Certificate::importCert(const byte certDER[], size_t derLen)
{
  _certBufferLen = derLen;
  _certBuffer = (byte*)malloc(_certBufferLen);

  if (_certBuffer == nullptr) {
    return 0;
  }

  memcpy(_certBuffer, certDER, _certBufferLen);

  /* Import Authority Key Identifier to compressed cert struct */
  if (!importCompressedAuthorityKeyIdentifier()) {
    return 0;
  }

  /* Import signature to compressed cert struct */
  if (!importCompressedSignature()) {
    return 0;
  }

  return 1;
}

int ECP256Certificate::signCert()
{
  return signCert(_compressedCert.slot.one.values.signature);
}

String ECP256Certificate::getCertPEM()
{
  return b64::pemEncode(_certBuffer, _certBufferLen, "-----BEGIN CERTIFICATE-----\n", "\n-----END CERTIFICATE-----\n");
}

void ECP256Certificate::getDateFromCompressedData(DateInfo& date) {
  date.issueYear = (_compressedCert.slot.one.values.dates[0] >> 3) + 2000;
  date.issueMonth = ((_compressedCert.slot.one.values.dates[0] & 0x07) << 1) | (_compressedCert.slot.one.values.dates[1] >> 7);
  date.issueDay = (_compressedCert.slot.one.values.dates[1] & 0x7c) >> 2;
  date.issueHour = ((_compressedCert.slot.one.values.dates[1] & 0x03) << 3) | (_compressedCert.slot.one.values.dates[2] >> 5);
  date.expireYears = (_compressedCert.slot.one.values.dates[2] & 0x1f);
}

void ECP256Certificate::setIssueYear(int issueYear) {
  _compressedCert.slot.one.values.dates[0] &= 0x07;
  _compressedCert.slot.one.values.dates[0] |= (issueYear - 2000) << 3;
}

void ECP256Certificate::setIssueMonth(int issueMonth) {
  _compressedCert.slot.one.values.dates[0] &= 0xf8;
  _compressedCert.slot.one.values.dates[0] |= issueMonth >> 1;

  _compressedCert.slot.one.values.dates[1] &= 0x7f;
  _compressedCert.slot.one.values.dates[1] |= issueMonth << 7;
}

void ECP256Certificate::setIssueDay(int issueDay) {
  _compressedCert.slot.one.values.dates[1] &= 0x83;
  _compressedCert.slot.one.values.dates[1] |= issueDay << 2;
}

void ECP256Certificate::setIssueHour(int issueHour) {
  _compressedCert.slot.one.values.dates[2] &= 0x1f;
  _compressedCert.slot.one.values.dates[2] |= issueHour << 5;

  _compressedCert.slot.one.values.dates[1] &= 0xfc;
  _compressedCert.slot.one.values.dates[1] |= issueHour >> 3;
}

void ECP256Certificate::setExpireYears(int expireYears) {
  _compressedCert.slot.one.values.dates[2] &= 0xe0;
  _compressedCert.slot.one.values.dates[2] |= expireYears;
}

int ECP256Certificate::setSerialNumber(const uint8_t serialNumber[], int serialNumberLen) {
  if (serialNumberLen == ECP256_CERT_SERIAL_NUMBER_LENGTH) {
    memcpy(_compressedCert.slot.two.values.serialNumber, serialNumber, ECP256_CERT_SERIAL_NUMBER_LENGTH);
    return 1;
  }
  return 0;
}

int ECP256Certificate::setAuthorityKeyId(const uint8_t authorityKeyId[], int authorityKeyIdLen) {
  if (authorityKeyIdLen == ECP256_CERT_AUTHORITY_KEY_ID_LENGTH) {
    memcpy(_compressedCert.slot.two.values.authorityKeyId, authorityKeyId, ECP256_CERT_AUTHORITY_KEY_ID_LENGTH);
    return 1;
  }
  return 0;
}

int ECP256Certificate::setPublicKey(const byte* publicKey, int publicKeyLen) {
  if (publicKeyLen == ECP256_CERT_PUBLIC_KEY_LENGTH) {
    _publicKey = publicKey;
    return 1;
  }
  return 0;
}

int ECP256Certificate::setSignature(const byte* signature, int signatureLen) {
  if (signatureLen == ECP256_CERT_SIGNATURE_LENGTH) {
    memcpy(_compressedCert.slot.one.values.signature, signature, ECP256_CERT_SIGNATURE_LENGTH);
    return 1;
  }
  return 0;
}

/******************************************************************************
 * PRIVATE MEMBER FUNCTIONS
 ******************************************************************************/

int ECP256Certificate::versionLength()
{
  return 3;
}

int ECP256Certificate::issuerOrSubjectLength(const CertInfo& issuerOrSubjectData)
{
  int length                       = 0;
  int countryNameLength            = issuerOrSubjectData.countryName.length();
  int stateProvinceNameLength      = issuerOrSubjectData.stateProvinceName.length();
  int localityNameLength           = issuerOrSubjectData.localityName.length();
  int organizationNameLength       = issuerOrSubjectData.organizationName.length();
  int organizationalUnitNameLength = issuerOrSubjectData.organizationalUnitName.length();
  int commonNameLength             = issuerOrSubjectData.commonName.length();

  if (countryNameLength) {
    length += (11 + countryNameLength);
  }

  if (stateProvinceNameLength) {
    length += (11 + stateProvinceNameLength);
  }

  if (localityNameLength) {
    length += (11 + localityNameLength);
  }

  if (organizationNameLength) {
    length += (11 + organizationNameLength);
  }

  if (organizationalUnitNameLength) {
    length += (11 + organizationalUnitNameLength);
  }

  if (commonNameLength) {
    length += (11 + commonNameLength);
  }

  return length;
}

int ECP256Certificate::sequenceHeaderLength(int length)
{
  if (length > 255) {
    return 4;
  } else if (length > 127) {
    return 3;
  } else {
    return 2;
  }
}

int ECP256Certificate::publicKeyLength()
{
  return (2 + 2 + 9 + 10 + 4 + 64);
}

int ECP256Certificate::signatureLength(const byte signature[])
{
  const byte* r = &signature[0];
  const byte* s = &signature[32];

  int rLength = 32;
  int sLength = 32;

  while (*r == 0x00 && rLength) {
    r++;
    rLength--;
  }

  if (*r & 0x80) {
    rLength++;
  }

  while (*s == 0x00 && sLength) {
    s++;
    sLength--;
  }

  if (*s & 0x80) {
    sLength++;
  }

  return (21 + rLength + sLength);
}

int ECP256Certificate::serialNumberLength(const byte serialNumber[], int length)
{
  while (*serialNumber == 0 && length) {
    serialNumber++;
    length--;
  }

  if (*serialNumber & 0x80) {
    length++;
  }

  return (2 + length);
}

int ECP256Certificate::authorityKeyIdLength(const byte authorityKeyId[], int length) {
  bool set = false;

  // check if the authority key identifier is non-zero
  for (int i = 0; i < length; i++) {
    if (authorityKeyId[i] != 0) {
      set = true;
      break;
    }
  }

  return (set ? (length + 17) : 0);
}

int ECP256Certificate::CSRInfoLength()
{
  int versionLen = versionLength();
  int subjectLen = issuerOrSubjectLength(_subjectData);
  int subjectHeaderLen = sequenceHeaderLength(subjectLen);
  int publicKeyLen = publicKeyLength();

  int csrInfoLen = versionLen + subjectHeaderLen + subjectLen + publicKeyLen + 2;

  return csrInfoLen;
}

int ECP256Certificate::getCSRSize()
{
  int csrInfoLen = CSRInfoLength();
  int csrInfoHeaderLen = sequenceHeaderLength(csrInfoLen);

  return (csrInfoLen + csrInfoHeaderLen);
}

int ECP256Certificate::getCSRSignedSize(byte * signature)
{
  int signatureLen = signatureLength(signature);
  int csrLen = getCSRSize() + signatureLen;
  return sequenceHeaderLength(csrLen) + csrLen;
}

int ECP256Certificate::certInfoLength()
{
  int datesSizeLen = 30;
  DateInfo dates;

  getDateFromCompressedData(dates);

  if (dates.issueYear > 2049) {
    // two more bytes for GeneralizedTime
    datesSizeLen += 2;
  }

  if ((dates.issueYear + dates.expireYears) > 2049) {
    // two more bytes for GeneralizedTime
    datesSizeLen += 2;
  }

  int serialNumberLen = serialNumberLength(_compressedCert.slot.two.values.serialNumber, ECP256_CERT_SERIAL_NUMBER_LENGTH);

  int issuerLen = issuerOrSubjectLength(_issuerData);

  int issuerHeaderLen = sequenceHeaderLength(issuerLen);

  int subjectLen = issuerOrSubjectLength(_subjectData);

  int subjectHeaderLen = sequenceHeaderLength(subjectLen);

  int publicKeyLen = publicKeyLength();
  
  int certInfoLen = 5 + serialNumberLen + 12 + issuerHeaderLen + issuerLen + (datesSizeLen + 2) +
                    subjectHeaderLen + subjectLen + publicKeyLen;

  int authorityKeyIdLen = authorityKeyIdLength(_compressedCert.slot.two.values.authorityKeyId, ECP256_CERT_AUTHORITY_KEY_ID_LENGTH);

  if (authorityKeyIdLen)
  {
    certInfoLen += authorityKeyIdLen;
  }
  else
  {
    certInfoLen += 4;
  }

  return certInfoLen;
}

int ECP256Certificate::getCertSize()
{
  int certInfoLen = certInfoLength();
  int certInfoHeaderLen = sequenceHeaderLength(certInfoLen);

  return (certInfoLen + certInfoHeaderLen);
}

int ECP256Certificate::getCertSignedSize(const byte * signature)
{
  int signatureLen = signatureLength(signature);
  int certLen = getCertSize() + signatureLen;
  return sequenceHeaderLength(certLen) + certLen;
}

int ECP256Certificate::appendSequenceHeader(int length, byte out[])
{
  *out++ = ASN1_SEQUENCE;
  if (length > 255) {
    *out++ = 0x82;
    *out++ = (length >> 8) & 0xff;
  } else if (length > 127) {
    *out++ = 0x81;
  }
  *out++ = (length) & 0xff;

  if (length > 255) {
    return 4;
  } else if (length > 127) {
    return 3;
  } else {
    return 2;
  }
}

int ECP256Certificate::appendVersion(int version, byte out[])
{
  out[0] = ASN1_INTEGER;
  out[1] = 0x01;
  out[2] = version;

  return versionLength();
}

int ECP256Certificate::appendName(const String& name, int type, byte out[])
{
  int nameLength = name.length();

  *out++ = ASN1_SET;
  *out++ = nameLength + 9;

  *out++ = ASN1_SEQUENCE;
  *out++ = nameLength + 7;

  *out++ = ASN1_OBJECT_IDENTIFIER;
  *out++ = 0x03;
  *out++ = 0x55;
  *out++ = 0x04;
  *out++ = type;

  *out++ = ASN1_PRINTABLE_STRING;
  *out++ = nameLength;
  memcpy(out, name.c_str(), nameLength);

  return (nameLength + 11);
}

int ECP256Certificate::appendIssuerOrSubject(const CertInfo& issuerOrSubjectData, byte out[])
{
  if (issuerOrSubjectData.countryName.length() > 0) {
    out += appendName(issuerOrSubjectData.countryName, 0x06, out);
  }

  if (issuerOrSubjectData.stateProvinceName.length() > 0) {
    out += appendName(issuerOrSubjectData.stateProvinceName, 0x08, out);
  }

  if (issuerOrSubjectData.localityName.length() > 0) {
    out += appendName(issuerOrSubjectData.localityName, 0x07, out);
  }

  if (issuerOrSubjectData.organizationName.length() > 0) {
    out += appendName(issuerOrSubjectData.organizationName, 0x0a, out);
  }

  if (issuerOrSubjectData.organizationalUnitName.length() > 0) {
    out += appendName(issuerOrSubjectData.organizationalUnitName, 0x0b, out);
  }

  if (issuerOrSubjectData.commonName.length() > 0) {
    out += appendName(issuerOrSubjectData.commonName, 0x03, out);
  }

  return issuerOrSubjectLength(issuerOrSubjectData);
}

int  ECP256Certificate::appendPublicKey(const byte publicKey[], byte out[])
{
  int subjectPublicKeyDataLength = 2 + 9 + 10 + 4 + 64;

  // subject public key
  *out++ = ASN1_SEQUENCE;
  *out++ = (subjectPublicKeyDataLength) & 0xff;

  *out++ = ASN1_SEQUENCE;
  *out++ = 0x13;

  // EC public key
  *out++ = ASN1_OBJECT_IDENTIFIER;
  *out++ = 0x07;
  *out++ = 0x2a;
  *out++ = 0x86;
  *out++ = 0x48;
  *out++ = 0xce;
  *out++ = 0x3d;
  *out++ = 0x02;
  *out++ = 0x01;

  // PRIME 256 v1
  *out++ = ASN1_OBJECT_IDENTIFIER;
  *out++ = 0x08;
  *out++ = 0x2a;
  *out++ = 0x86;
  *out++ = 0x48;
  *out++ = 0xce;
  *out++ = 0x3d;
  *out++ = 0x03;
  *out++ = 0x01;
  *out++ = 0x07;

  *out++ = 0x03;
  *out++ = 0x42;
  *out++ = 0x00;
  *out++ = 0x04;

  memcpy(out, publicKey, 64);

  return publicKeyLength();
}

int ECP256Certificate::appendSignature(const byte signature[], byte out[])
{
  // signature algorithm
  *out++ = ASN1_SEQUENCE;
  *out++ = 0x0a;
  *out++ = ASN1_OBJECT_IDENTIFIER;
  *out++ = 0x08;

  // ECDSA with SHA256
  *out++ = 0x2a;
  *out++ = 0x86;
  *out++ = 0x48;
  *out++ = 0xce;
  *out++ = 0x3d;
  *out++ = 0x04;
  *out++ = 0x03;
  *out++ = 0x02;

  const byte* r = &signature[0];
  const byte* s = &signature[32];

  int rLength = 32;
  int sLength = 32;

  while (*r == 0 && rLength) {
    r++;
    rLength--;
  }

  while (*s == 0 && sLength) {
    s++;
    sLength--;
  }

  if (*r & 0x80) {
    rLength++;
  }

  if (*s & 0x80) {
    sLength++;
  }

  *out++ = ASN1_BIT_STRING;
  *out++ = (rLength + sLength + 7);
  *out++ = 0;

  *out++ = ASN1_SEQUENCE;
  *out++ = (rLength + sLength + 4);

  *out++ = ASN1_INTEGER;
  *out++ = rLength;
  if ((*r & 0x80) && rLength) {
    *out++ = 0;
    rLength--;
  }
  memcpy(out, r, rLength);
  out += rLength;

  *out++ = ASN1_INTEGER;
  *out++ = sLength;
  if ((*s & 0x80) && sLength) {
    *out++ = 0;
    sLength--;
  }
  memcpy(out, s, sLength);
  out += rLength;

  return signatureLength(signature);
}

int ECP256Certificate::appendSerialNumber(const byte serialNumber[], int length, byte out[])
{
  while (*serialNumber == 0 && length) {
    serialNumber++;
    length--;
  }

  if (*serialNumber & 0x80) {
    length++;  
  }

  *out++ = ASN1_INTEGER;
  *out++ = length;

  if (*serialNumber & 0x80) {
    *out++ = 0x00;
    length--;
  }

  memcpy(out, serialNumber, length);
  
  if (*serialNumber & 0x80) {
    length++;
  }

  return (2 + length);
}

int ECP256Certificate::appendDate(int year, int month, int day, int hour, int minute, int second, byte out[])
{
  bool useGeneralizedTime = (year > 2049);

  if (useGeneralizedTime) {
    *out++ = 0x18;
    *out++ = 0x0f;
    *out++ = '0' + (year / 1000);
    *out++ = '0' + ((year % 1000) / 100);
    *out++ = '0' + ((year % 100) / 10);
    *out++ = '0' + (year % 10);
  } else {
    year -= 2000;

    *out++ = 0x17;
    *out++ = 0x0d;
    *out++ = '0' + (year / 10);
    *out++ = '0' + (year % 10);
  }
  *out++ = '0' + (month / 10);
  *out++ = '0' + (month % 10);
  *out++ = '0' + (day / 10);
  *out++ = '0' + (day % 10);
  *out++ = '0' + (hour / 10);
  *out++ = '0' + (hour % 10);
  *out++ = '0' + (minute / 10);
  *out++ = '0' + (minute % 10);
  *out++ = '0' + (second / 10);
  *out++ = '0' + (second % 10);
  *out++ = 0x5a; // UTC

  return (useGeneralizedTime ? 17 : 15);
}

int ECP256Certificate::appendEcdsaWithSHA256(byte out[])
{
  *out++ = ASN1_SEQUENCE;
  *out++ = 0x0A;
  *out++ = ASN1_OBJECT_IDENTIFIER;
  *out++ = 0x08;
  *out++ = 0x2A;
  *out++ = 0x86;
  *out++ = 0x48;
  *out++ = 0xCE;
  *out++ = 0x3D;
  *out++ = 0x04;
  *out++ = 0x03;
  *out++ = 0x02;

  return 12;
}

int ECP256Certificate::appendAuthorityKeyId(const byte authorityKeyId[], int length, byte out[]) {
  // [3]
  *out++ = 0xa3;
  *out++ = 0x23;

  // sequence
  *out++ = ASN1_SEQUENCE;
  *out++ = 0x21;

  // sequence
  *out++ = ASN1_SEQUENCE;
  *out++ = 0x1f;

  // 2.5.29.35 authorityKeyIdentifier(X.509 extension)
  *out++ = 0x06;
  *out++ = 0x03;
  *out++ = 0x55;
  *out++ = 0x1d;
  *out++ = 0x23;

  // octet string
  *out++ = 0x04;
  *out++ = 0x18;

  // sequence
  *out++ = ASN1_SEQUENCE;
  *out++ = 0x16;

  *out++ = 0x80;
  *out++ = 0x14;

  memcpy(out, authorityKeyId, length);

  return length + 17;
}

int ECP256Certificate::importCompressedAuthorityKeyIdentifier() {
  static const byte objectId[] = {0x06, 0x03, 0x55, 0x1D, 0x23};
  byte * result = nullptr;
  void * ptr = memmem(_certBuffer, _certBufferLen, objectId, sizeof(objectId));
  if (ptr != nullptr) {
    result = (byte*)ptr;
    result += 11;
    memcpy(_compressedCert.slot.two.values.authorityKeyId, result, ECP256_CERT_AUTHORITY_KEY_ID_LENGTH);
    return 1;
  }
  return 0;
}

int ECP256Certificate::importCompressedSignature() {
  byte * result = nullptr;
  byte paddingBytes = 0;
  byte rLen = 0;
  byte sLen = 0;

  /* Search AuthorityKeyIdentifier */
  static const byte KeyId[] = {0x06, 0x03, 0x55, 0x1D, 0x23};
  void * ptr = memmem(_certBuffer, _certBufferLen, KeyId, sizeof(KeyId));
  if(ptr == nullptr) {
    return 0;
  }
  result = (byte*)ptr;

  /* Search Algorithm identifier */
  static const byte AlgId[] = {0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x02};
  ptr = memmem(result, _certBufferLen - (_certBuffer - result), AlgId, sizeof(AlgId));
  if(ptr == nullptr) {
    return 0;
  }
  result = (byte*)ptr;

  /* Skip algorithm identifier */
  result += sizeof(AlgId);

  /* Search signature sequence */
  if (result[0] == 0x03) {
    /* Move to  the first element of R sequence skipping 0x03 0x49 0x00 0x30 0xXX*/
    result += 5;
    /* Check if value is padded */
    if (result[0] == 0x02 && result[1] == 0x21 && result[2] == 0x00) {
      paddingBytes = 1;
    }
    rLen = result[1] - paddingBytes;
    /* Skip padding and ASN INTEGER sequence 0x02 0xXX */
    result += (2 + paddingBytes);
    /* Check data length */
    if (rLen != ECP256_CERT_SIGNATURE_R_LENGTH) {
      return 0;
    }
    /* Copy data to compressed slot */
    memcpy(_compressedCert.slot.one.values.signature, result, rLen);
    /* reset padding before importing S sequence */
    paddingBytes = 0;
    /* Move to the first element of S sequence skipping R values */
    result += rLen;
    /* Check if value is padded */
    if (result[0] == 0x02 && result[1] == 0x21 && result[2] == 0x00) {
      paddingBytes = 1;
    }
    sLen = result[1] - paddingBytes;
    /* Skip padding and ASN INTEGER sequence 0x02 0xXX */
    result += (2 + paddingBytes);
    /* Check data length */
    if (sLen != ECP256_CERT_SIGNATURE_S_LENGTH) {
      return 0;
    }
    /* Copy data to compressed slot */
    memcpy(&_compressedCert.slot.one.values.signature[rLen], result, sLen);
    return 1;
  }
  return 0;
}
