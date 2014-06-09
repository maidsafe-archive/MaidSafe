ECHO ON
START "Boot Strap" cmd /c "C:\Code\build_maidsafe\Debug\vault_key_helper -b > bootstrapinfo.in"
TIMEOUT 20
COPY bootstrapinfo.in bootstrapinfo.out
FOR /F "skip=3 tokens=2" %%G IN (C:\Code\bootstrapinfo.out) DO start2.bat %%G %1
DEL bootstrapinfo.out



