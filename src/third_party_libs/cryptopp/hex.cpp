// hex.cpp - written and placed in the public domain by Wei Dai

#include "pch.h"

#ifndef CRYPTOPP_IMPORTS

#include "hex.h"
#include <atomic>
#include <memory>
#include <mutex>
#include <thread>

NAMESPACE_BEGIN(CryptoPP)

static const byte s_vecUpper[] = "0123456789ABCDEF";
static const byte s_vecLower[] = "0123456789abcdef";

void HexEncoder::IsolatedInitialize(const NameValuePairs &parameters)
{
  bool uppercase = parameters.GetValueWithDefault(Name::Uppercase(), true);
  m_filter->Initialize(CombinedNameValuePairs(
    parameters,
    MakeParameters(Name::EncodingLookupArray(), uppercase ? &s_vecUpper[0] : &s_vecLower[0], false)(Name::Log2Base(), 4, true)));
}

void HexDecoder::IsolatedInitialize(const NameValuePairs &parameters)
{
  BaseN_Decoder::IsolatedInitialize(CombinedNameValuePairs(
    parameters,
    MakeParameters(Name::DecodingLookupArray(), GetDefaultDecodingLookupArray(), false)(Name::Log2Base(), 4, true)));
}


#ifdef __GNUC__
const int *HexDecoder::GetDefaultDecodingLookupArray()
{
  static volatile bool s_initialized = false;
  static int s_array[256];

  if (!s_initialized)
  {
    InitializeDecodingLookupArray(s_array, s_vecUpper, 16, true);
    s_initialized = true;
  }
  return s_array;
}
#else
const int *HexDecoder::GetDefaultDecodingLookupArray()
{
  static std::once_flag s_initialized_flag;
  static std::atomic<bool> s_initialized(false);
  static int s_array[256];

  auto deleter([&](char* c) {
    delete c;
    s_initialized.store(true);
  });

  std::call_once(s_initialized_flag, [&] {
    // This unique_ptr guarantees that 's_initialized == true' on exiting the call_once lambda.
    const std::unique_ptr<char, decltype(deleter)> scoped_setter(new char, deleter);
    InitializeDecodingLookupArray(s_array, s_vecUpper, 16, true);
  });

  while (!s_initialized)
    std::this_thread::yield();

  return s_array;
}
#endif
NAMESPACE_END

#endif
