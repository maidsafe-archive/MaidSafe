file(TO_NATIVE_PATH "${ProcessLocation}" ProcessLocation)
string(REPLACE "\\" "\\\\" ProcessLocation "${ProcessLocation}")
configure_file(${InputFile} ${OutputFile} @ONLY)
