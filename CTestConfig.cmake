set(CTEST_PROJECT_NAME "maidsafe")
set(CTEST_NIGHTLY_START_TIME "00:00:00 UTC")
set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "dash.maidsafe.net")
set(CTEST_DROP_LOCATION "/submit.php?project=MaidSafe")
set(CTEST_DROP_SITE_CDASH TRUE)
if(CMAKE_CL_64)
  set(BUILDNAME Win-x64-MSBuild)
endif()
set(CTEST_PROJECT_SUBPROJECTS
    Common
    Private
    Passport
    Rudp
    Routing
    Nfs
    Encrypt
    Drive
    Vault
    Lifestuff
    LifestuffUiQt
    )
