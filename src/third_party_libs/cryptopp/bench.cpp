// bench.cpp - written and placed in the public domain by Wei Dai

#define _CRT_SECURE_NO_DEPRECATE

#include "bench.h"
#include "validate.h"
#include "aes.h"
#include "blumshub.h"
#include "files.h"
#include "hex.h"
#include "modes.h"
#include "factory.h"
#include "cpu.h"

#include <time.h>
#include <math.h>
#include <iostream>
#include <iomanip>

USING_NAMESPACE(CryptoPP)
USING_NAMESPACE(std)

#ifdef CLOCKS_PER_SEC
const double CLOCK_TICKS_PER_SECOND = (double)CLOCKS_PER_SEC;
#elif defined(CLK_TCK)
const double CLOCK_TICKS_PER_SECOND = (double)CLK_TCK;
#else
const double CLOCK_TICKS_PER_SECOND = 1000000.0;
#endif

double logtotal = 0, g_allocatedTime, g_hertz;
unsigned int logcount = 0;

static const byte *const key=(byte *)"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";

void OutputResultBytes(const char *name, double length, double timeTaken)
{
	double mbs = length / timeTaken / (1024*1024);
	cout << "\n<TR><TH>" << name;
//	cout << "<TD>" << setprecision(3) << length / (1024*1024);
	cout << setiosflags(ios::fixed);
//	cout << "<TD>" << setprecision(3) << timeTaken;
	cout << "<TD>" << setprecision(0) << setiosflags(ios::fixed) << mbs;
	if (g_hertz)
		cout << "<TD>" << setprecision(1) << setiosflags(ios::fixed) << timeTaken * g_hertz / length;
	cout << resetiosflags(ios::fixed);
	logtotal += log(mbs);
	logcount++;
}

void OutputResultKeying(double iterations, double timeTaken)
{
	cout << "<TD>" << setprecision(3) << setiosflags(ios::fixed) << (1000*1000*timeTaken/iterations);
	if (g_hertz)
		cout << "<TD>" << setprecision(0) << setiosflags(ios::fixed) << timeTaken * g_hertz / iterations;
}

void OutputResultOperations(const char *name, const char *operation, bool pc, unsigned long iterations, double timeTaken)
{
	cout << "\n<TR><TH>" << name << " " << operation << (pc ? " with precomputation" : "");
//	cout << "<TD>" << iterations;
//	cout << setiosflags(ios::fixed);
//	cout << "<TD>" << setprecision(3) << timeTaken;
	cout << "<TD>" << setprecision(2) << setiosflags(ios::fixed) << (1000*timeTaken/iterations);
	if (g_hertz)
		cout << "<TD>" << setprecision(2) << setiosflags(ios::fixed) << timeTaken * g_hertz / iterations / 1000000;
	cout << resetiosflags(ios::fixed);

	logtotal += log(iterations/timeTaken);
	logcount++;
}

/*
void BenchMark(const char *name, BlockTransformation &cipher, double timeTotal)
{
	const int BUF_SIZE = RoundUpToMultipleOf(2048U, cipher.OptimalNumberOfParallelBlocks() * cipher.BlockSize());
	AlignedSecByteBlock buf(BUF_SIZE);
	const int nBlocks = BUF_SIZE / cipher.BlockSize();
	clock_t start = clock();

	unsigned long i=0, blocks=1;
	double timeTaken;
	do
	{
		blocks *= 2;
		for (; i<blocks; i++)
			cipher.ProcessAndXorMultipleBlocks(buf, NULL, buf, nBlocks);
		timeTaken = double(clock() - start) / CLOCK_TICKS_PER_SECOND;
	}
	while (timeTaken < 2.0/3*timeTotal);

	OutputResultBytes(name, double(blocks) * BUF_SIZE, timeTaken);
}
*/

void BenchMark(const char *name, StreamTransformation &cipher, double timeTotal)
{
	const int BUF_SIZE=RoundUpToMultipleOf(2048U, cipher.OptimalBlockSize());
	AlignedSecByteBlock buf(BUF_SIZE);
	GlobalRNG().GenerateBlock(buf, BUF_SIZE);
	clock_t start = clock();

	unsigned long i=0, blocks=1;
	double timeTaken;
	do
	{
		blocks *= 2;
		for (; i<blocks; i++)
			cipher.ProcessString(buf, BUF_SIZE);
		timeTaken = double(clock() - start) / CLOCK_TICKS_PER_SECOND;
	}
	while (timeTaken < 2.0/3*timeTotal);

	OutputResultBytes(name, double(blocks) * BUF_SIZE, timeTaken);
}

void BenchMark(const char *name, AuthenticatedSymmetricCipher &cipher, double timeTotal)
{
	if (cipher.NeedsPrespecifiedDataLengths())
		cipher.SpecifyDataLengths(0, cipher.MaxMessageLength(), 0);

	BenchMark(name, static_cast<StreamTransformation &>(cipher), timeTotal);
}

void BenchMark(const char *name, HashTransformation &ht, double timeTotal)
{
	const int BUF_SIZE=2048U;
	AlignedSecByteBlock buf(BUF_SIZE);
	GlobalRNG().GenerateBlock(buf, BUF_SIZE);
	clock_t start = clock();

	unsigned long i=0, blocks=1;
	double timeTaken;
	do
	{
		blocks *= 2;
		for (; i<blocks; i++)
			ht.Update(buf, BUF_SIZE);
		timeTaken = double(clock() - start) / CLOCK_TICKS_PER_SECOND;
	}
	while (timeTaken < 2.0/3*timeTotal);

	OutputResultBytes(name, double(blocks) * BUF_SIZE, timeTaken);
}

void BenchMark(const char *name, BufferedTransformation &bt, double timeTotal)
{
	const int BUF_SIZE=2048U;
	AlignedSecByteBlock buf(BUF_SIZE);
	GlobalRNG().GenerateBlock(buf, BUF_SIZE);
	clock_t start = clock();

	unsigned long i=0, blocks=1;
	double timeTaken;
	do
	{
		blocks *= 2;
		for (; i<blocks; i++)
			bt.Put(buf, BUF_SIZE);
		timeTaken = double(clock() - start) / CLOCK_TICKS_PER_SECOND;
	}
	while (timeTaken < 2.0/3*timeTotal);

	OutputResultBytes(name, double(blocks) * BUF_SIZE, timeTaken);
}

void BenchMarkKeying(SimpleKeyingInterface &c, size_t keyLength, const NameValuePairs &params)
{
	unsigned long iterations = 0;
	clock_t start = clock();
	double timeTaken;
	do
	{
		for (unsigned int i=0; i<1024; i++)
			c.SetKey(key, keyLength, params);
		timeTaken = double(clock() - start) / CLOCK_TICKS_PER_SECOND;
		iterations += 1024;
	}
	while (timeTaken < g_allocatedTime);

	OutputResultKeying(iterations, timeTaken);
}

//VC60 workaround: compiler bug triggered without the extra dummy parameters
// on VC60 also needs to be named differently from BenchMarkByName
template <class T_FactoryOutput, class T_Interface>
void BenchMarkByName2(const char *factoryName, size_t keyLength = 0, const char *displayName=NULL, const NameValuePairs &params = g_nullNameValuePairs, T_FactoryOutput *x=NULL, T_Interface *y=NULL)
{
	std::string name = factoryName;
	if (displayName)
		name = displayName;
	else if (keyLength)
		name += " (" + IntToString(keyLength * 8) + "-bit key)";

	std::auto_ptr<T_FactoryOutput> obj(ObjectFactoryRegistry<T_FactoryOutput>::Registry().CreateObject(factoryName));
	if (!keyLength)
		keyLength = obj->DefaultKeyLength();
	obj->SetKey(key, keyLength, CombinedNameValuePairs(params, MakeParameters(Name::IV(), ConstByteArrayParameter(key, obj->IVSize()), false)));
	BenchMark(name.c_str(), *static_cast<T_Interface *>(obj.get()), g_allocatedTime);
	BenchMarkKeying(*obj, keyLength, CombinedNameValuePairs(params, MakeParameters(Name::IV(), ConstByteArrayParameter(key, obj->IVSize()), false)));
}

//VC60 workaround: compiler bug triggered without the extra dummy parameters
template <class T_FactoryOutput>
void BenchMarkByName(const char *factoryName, size_t keyLength = 0, const char *displayName=NULL, const NameValuePairs &params = g_nullNameValuePairs, T_FactoryOutput *x=NULL)
{
	BenchMarkByName2<T_FactoryOutput, T_FactoryOutput>(factoryName, keyLength, displayName, params, x, x);
}

template <class T>
void BenchMarkByNameKeyLess(const char *factoryName, const char *displayName=NULL, const NameValuePairs &params = g_nullNameValuePairs, T *x=NULL)
{
	std::string name = factoryName;
	if (displayName)
		name = displayName;

	std::auto_ptr<T> obj(ObjectFactoryRegistry<T>::Registry().CreateObject(factoryName));
	BenchMark(name.c_str(), *obj, g_allocatedTime);
}

void BenchmarkAll(double t, double hertz)
{
#if 1
	logtotal = 0;
	logcount = 0;
	g_allocatedTime = t;
	g_hertz = hertz;

	const char *cpb, *cpk;
	if (g_hertz)
	{
		cpb = "<TH>Cycles Per Byte";
		cpk = "<TH>Cycles to<br>Setup Key and IV";
		cout << "CPU frequency of the test platform is " << g_hertz << " Hz.\n";
	}
	else
	{
		cpb = cpk = "";
		cout << "CPU frequency of the test platform was not provided.\n";
	}

	cout << "<TABLE border=1><COLGROUP><COL align=left><COL align=right><COL align=right><COL align=right><COL align=right>" << endl;
	cout << "<THEAD><TR><TH>Algorithm<TH>MiB/Second" << cpb << "<TH>Microseconds to<br>Setup Key and IV" << cpk << endl;

	cout << "\n<TBODY style=\"background: yellow\">";
#if CRYPTOPP_BOOL_AESNI_INTRINSICS_AVAILABLE
	if (HasCLMUL())
		BenchMarkByName2<AuthenticatedSymmetricCipher, AuthenticatedSymmetricCipher>("AES/GCM", 0, "AES/GCM");
	else
#endif
	{
		BenchMarkByName2<AuthenticatedSymmetricCipher, AuthenticatedSymmetricCipher>("AES/GCM", 0, "AES/GCM (2K tables)", MakeParameters(Name::TableSize(), 2048));
		BenchMarkByName2<AuthenticatedSymmetricCipher, AuthenticatedSymmetricCipher>("AES/GCM", 0, "AES/GCM (64K tables)", MakeParameters(Name::TableSize(), 64*1024));
	}
	BenchMarkByName2<AuthenticatedSymmetricCipher, AuthenticatedSymmetricCipher>("AES/CCM");
	BenchMarkByName2<AuthenticatedSymmetricCipher, AuthenticatedSymmetricCipher>("AES/EAX");

	cout << "\n<TBODY style=\"background: white\">";
#if CRYPTOPP_BOOL_AESNI_INTRINSICS_AVAILABLE
	if (HasCLMUL())
		BenchMarkByName2<AuthenticatedSymmetricCipher, MessageAuthenticationCode>("AES/GCM", 0, "GMAC(AES)");
	else
#endif
	{
		BenchMarkByName2<AuthenticatedSymmetricCipher, MessageAuthenticationCode>("AES/GCM", 0, "GMAC(AES) (2K tables)", MakeParameters(Name::TableSize(), 2048));
		BenchMarkByName2<AuthenticatedSymmetricCipher, MessageAuthenticationCode>("AES/GCM", 0, "GMAC(AES) (64K tables)", MakeParameters(Name::TableSize(), 64*1024));
	}
	BenchMarkByName<MessageAuthenticationCode>("VMAC(AES)-64");
	BenchMarkByName<MessageAuthenticationCode>("VMAC(AES)-128");
	BenchMarkByName<MessageAuthenticationCode>("HMAC(SHA-1)");
	BenchMarkByName<MessageAuthenticationCode>("Two-Track-MAC");
	BenchMarkByName<MessageAuthenticationCode>("CMAC(AES)");
	BenchMarkByName<MessageAuthenticationCode>("DMAC(AES)");

	cout << "\n<TBODY style=\"background: yellow\">";
	BenchMarkByNameKeyLess<HashTransformation>("CRC32");
	BenchMarkByNameKeyLess<HashTransformation>("Adler32");
	BenchMarkByNameKeyLess<HashTransformation>("MD5");
	BenchMarkByNameKeyLess<HashTransformation>("SHA-1");
	BenchMarkByNameKeyLess<HashTransformation>("SHA-256");
	BenchMarkByNameKeyLess<HashTransformation>("SHA-512");
	BenchMarkByNameKeyLess<HashTransformation>("Tiger");
	BenchMarkByNameKeyLess<HashTransformation>("Whirlpool");
	BenchMarkByNameKeyLess<HashTransformation>("RIPEMD-160");
	BenchMarkByNameKeyLess<HashTransformation>("RIPEMD-320");
	BenchMarkByNameKeyLess<HashTransformation>("RIPEMD-128");
	BenchMarkByNameKeyLess<HashTransformation>("RIPEMD-256");

	cout << "\n<TBODY style=\"background: white\">";
	BenchMarkByName<SymmetricCipher>("Panama-LE");
	BenchMarkByName<SymmetricCipher>("Panama-BE");
	BenchMarkByName<SymmetricCipher>("Salsa20");
	BenchMarkByName<SymmetricCipher>("Salsa20", 0, "Salsa20/12", MakeParameters(Name::Rounds(), 12));
	BenchMarkByName<SymmetricCipher>("Salsa20", 0, "Salsa20/8", MakeParameters(Name::Rounds(), 8));
	BenchMarkByName<SymmetricCipher>("Sosemanuk");
	BenchMarkByName<SymmetricCipher>("MARC4");
	BenchMarkByName<SymmetricCipher>("SEAL-3.0-LE");
	BenchMarkByName<SymmetricCipher>("WAKE-OFB-LE");

	cout << "\n<TBODY style=\"background: yellow\">";
	BenchMarkByName<SymmetricCipher>("AES/CTR", 16);
	BenchMarkByName<SymmetricCipher>("AES/CTR", 24);
	BenchMarkByName<SymmetricCipher>("AES/CTR", 32);
	BenchMarkByName<SymmetricCipher>("AES/CBC", 16);
	BenchMarkByName<SymmetricCipher>("AES/CBC", 24);
	BenchMarkByName<SymmetricCipher>("AES/CBC", 32);
	BenchMarkByName<SymmetricCipher>("AES/OFB", 16);
	BenchMarkByName<SymmetricCipher>("AES/CFB", 16);
	BenchMarkByName<SymmetricCipher>("AES/ECB", 16);
	BenchMarkByName<SymmetricCipher>("Camellia/CTR", 16);
	BenchMarkByName<SymmetricCipher>("Camellia/CTR", 32);
	BenchMarkByName<SymmetricCipher>("Twofish/CTR");
	BenchMarkByName<SymmetricCipher>("Serpent/CTR");
	BenchMarkByName<SymmetricCipher>("CAST-256/CTR");
	BenchMarkByName<SymmetricCipher>("RC6/CTR");
	BenchMarkByName<SymmetricCipher>("MARS/CTR");
	BenchMarkByName<SymmetricCipher>("SHACAL-2/CTR", 16);
	BenchMarkByName<SymmetricCipher>("SHACAL-2/CTR", 64);
	BenchMarkByName<SymmetricCipher>("DES/CTR");
	BenchMarkByName<SymmetricCipher>("DES-XEX3/CTR");
	BenchMarkByName<SymmetricCipher>("DES-EDE3/CTR");
	BenchMarkByName<SymmetricCipher>("IDEA/CTR");
	BenchMarkByName<SymmetricCipher>("RC5/CTR", 0, "RC5 (r=16)");
	BenchMarkByName<SymmetricCipher>("Blowfish/CTR");
	BenchMarkByName<SymmetricCipher>("TEA/CTR");
	BenchMarkByName<SymmetricCipher>("XTEA/CTR");
	BenchMarkByName<SymmetricCipher>("CAST-128/CTR");
	BenchMarkByName<SymmetricCipher>("SKIPJACK/CTR");
	BenchMarkByName<SymmetricCipher>("SEED/CTR", 0, "SEED/CTR (1/2 K table)");
	cout << "</TABLE>" << endl;

	BenchmarkAll2(t, hertz);

	cout << "Throughput Geometric Average: " << setiosflags(ios::fixed) << exp(logtotal/logcount) << endl;

	time_t endTime = time(NULL);
	cout << "\nTest ended at " << asctime(localtime(&endTime));
#endif
}
