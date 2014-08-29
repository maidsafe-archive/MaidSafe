// base64.cpp - written and placed in the public domain by Wei Dai

#include "pch.h"
#include "base64.h"
#include <atomic>
#include <memory>
#include <mutex>
#include <thread>

NAMESPACE_BEGIN(CryptoPP)

static const byte s_vec[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const byte s_padding = '=';

void Base64Encoder::IsolatedInitialize(const NameValuePairs &parameters)
{
  bool insertLineBreaks = parameters.GetValueWithDefault(Name::InsertLineBreaks(), true);
  int maxLineLength = parameters.GetIntValueWithDefault(Name::MaxLineLength(), 72);

  const char *lineBreak = insertLineBreaks ? "\n" : "";

  m_filter->Initialize(CombinedNameValuePairs(
    parameters,
    MakeParameters(Name::EncodingLookupArray(), &s_vec[0], false)
      (Name::PaddingByte(), s_padding)
      (Name::GroupSize(), insertLineBreaks ? maxLineLength : 0)
      (Name::Separator(), ConstByteArrayParameter(lineBreak))
      (Name::Terminator(), ConstByteArrayParameter(lineBreak))
      (Name::Log2Base(), 6, true)));
}
#ifdef __GNUC__
const int *Base64Decoder::GetDecodingLookupArray()
{
  static volatile bool s_initialized = false;
  static int s_array[256];

  if (!s_initialized)
  {
    InitializeDecodingLookupArray(s_array, s_vec, 64, false);
    s_initialized = true;
  }
  return s_array;
}

#else
const int *Base64Decoder::GetDecodingLookupArray()
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
    InitializeDecodingLookupArray(s_array, s_vec, 64, false);
  });

  while (!s_initialized)
    std::this_thread::yield();

  return s_array;
}
#endif

NAMESPACE_END
