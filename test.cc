#include <iostream>
#include <filesystem>
int main() {
  std::filesystem::remove("./LogFiles/LogFile_20230326_162426.log");
}