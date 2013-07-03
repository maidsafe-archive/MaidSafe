#include "maidsafe/common/log.h"
#include "maidsafe/common/test.h"

TEST(Dummy, HelloWorld) {
  LOG(kInfo) << "Hello World";
}

int main(int argc, char **argv) {
  maidsafe::log::Logging::Instance().Initialise(argc, argv);

  testing::InitGoogleTest(&argc, argv);
  int result(RUN_ALL_TESTS());
  int test_count = testing::UnitTest::GetInstance()->test_to_run_count();
  return (test_count == 0) ? -1 : result;
}
