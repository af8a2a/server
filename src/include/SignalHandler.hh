
#pragma once
#include <csignal>
#include <functional>
#include <map>

std::map<int, std::function<void()>> handlers;//NOLINT
void SignalHandler(int sig) {
  handlers[sig]();
}

  static void Signal(int sig, const std::function<void()> &handler) {
    handlers[sig] = handler;
    ::signal(sig, SignalHandler);
  }