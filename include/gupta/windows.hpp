#ifndef GUPTA_WINDOWS_HPP
#define GUPTA_WINDOWS_HPP

#pragma once

#include <Windows.h>
#include <gupta/format_io.hpp>
#include <string>

namespace gupta {

// Returns the last Win32 error, in string format. Returns an empty string if there is no error.
static std::string GetLastErrorAsString(DWORD errorMessageID = ::GetLastError()) {
  if (errorMessageID == 0)
    return std::string(); // No error message has been recorded

  LPSTR messageBuffer = nullptr;
  size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                                   FORMAT_MESSAGE_IGNORE_INSERTS,
                               NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                               (LPSTR)&messageBuffer, 0, NULL);

  std::string message(messageBuffer, size);

  // Free the buffer.
  LocalFree(messageBuffer);

  return message;
}

class WinLastError {
public:
  WinLastError(int l = 0) { lastError_ = l; }
  WinLastError &operator=(int l) {
    lastError_ = l;
    return *this;
  }
  auto lastError() { return lastError_; }
  auto msg() { return GetLastErrorAsString(lastError_); }

private:
  int lastError_ = 0;
};

class WinHandle {
  HANDLE value_;

public:
  WinHandle(std::nullptr_t = nullptr) : value_(nullptr) {}
  WinHandle(HANDLE value) : value_(value) {}

  WinHandle(const WinHandle &) = delete;
  WinHandle(WinHandle &&h) { std::swap(h.value_, value_); }

  WinHandle &operator=(const WinHandle &) = delete;
  WinHandle &operator=(WinHandle &&h) {
    std::swap(h.value_, value_);
    return *this;
  }

  explicit operator bool() const { return value_ != INVALID_HANDLE_VALUE; }
  operator HANDLE() const { return value_; }

  ~WinHandle() {
    if (value_ != INVALID_HANDLE_VALUE)
      CloseHandle(value_);
  }
};

static inline std::string to_string(WinLastError e) { return e.msg(); }

struct Semaphore {
  bool create(std::string Name) {
    Name_ = std::move(Name);
    return _create();
  }

  bool open(std::string Name) {
    Name_ = std::move(Name);
    return _open();
  }

  void lock() {
    // debug("locked %", Name_);
    if (WaitForSingleObject(hSemaphore_, INFINITE) == WAIT_FAILED)
      debug("failed wait");
    // debug("wait completed");
  }
  void unlock() {
   // debug("unlocked %", Name_);
    if (!ReleaseSemaphore(hSemaphore_, 1, NULL))
      debug("ReleaseSemaphore failed: %", GetLastErrorAsString());
    else {
      // debug("released semaphore %", Name_);
    }
  }

private:
  gupta::WinHandle hSemaphore_;
  std::string Name_;
  bool _create() {
    hSemaphore_ = CreateSemaphoreA(NULL, 1, 1, Name_.c_str());
    if (GetLastError() == ERROR_ALIAS_EXISTS) {
      debug("% semaphore already exists", Name_);
      return false;
    } else if (hSemaphore_ == INVALID_HANDLE_VALUE) {
      debug("CreateSemaphore(%) failed: %", Name_, gupta::GetLastErrorAsString());
      return false;
    }
    debug("created semaphore % with handle %", Name_, hSemaphore_);
    return true;
  }

  bool _open() {
    hSemaphore_ = OpenSemaphoreA(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, false, Name_.c_str());
    if (hSemaphore_ == INVALID_HANDLE_VALUE) {
      debug("failed to open handle - \"%\": %", Name_, gupta::GetLastErrorAsString());
      return false;
    }
    debug("opened semaphore % with handle %", Name_, hSemaphore_);
    return true;
  }
};

struct SharedMemory {
  std::string name() { return Name_; };
  void *data() { return BufPtr_; }

  void lock() { Semaphore_.lock(); }

  void unlock() { Semaphore_.unlock(); }

  bool create(std::string Name, int BufSize) {
    BufSize_ = BufSize;
    if (BufPtr_) {
      debug("attempt to create sharedmemory with already associated SharedMem");
      return 0;
    }
    Name_ = std::move(Name);
    hMapFile_ =
        CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 1024, Name_.c_str());
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
      debug("Shared Memory \"%\" Already exists", Name_);
      return 0;
    }
    if (hMapFile_ == INVALID_HANDLE_VALUE) {
      debug("Failed to open SharedMemory: %", gupta::GetLastErrorAsString());
      return 0;
    }
    debug("opened fileMap with name - %", Name_.c_str());
    BufPtr_ = MapViewOfFile(hMapFile_, FILE_MAP_ALL_ACCESS, 0, 0, BufSize_);
    if (!BufPtr_) {
      debug("failed to mapviewofFile");
      return 0;
    } else {
      debug("BufPtr = %", BufPtr_);
    }

    return Semaphore_.create(semaphore_name());
    ;
  }

  bool open(std::string Name, int BufSize) {
    BufSize_ = BufSize;
    if (BufPtr_) {
      debug("attempt to reassociat a shared memory");
      return 0;
    }
    Name_ = std::move(Name);
    hMapFile_ = OpenFileMappingA(FILE_MAP_ALL_ACCESS, // read/write access
                                 FALSE,               // do not inherit the name
                                 Name_.c_str());
    if (hMapFile_ == INVALID_HANDLE_VALUE) {
      debug("failed to open SharedMemory: %", Name_);
      return 0;
    }

    BufPtr_ = (LPTSTR)MapViewOfFile(hMapFile_,           // handle to map object
                                    FILE_MAP_ALL_ACCESS, // read/write permission
                                    0, 0, BufSize_);
    if (BufPtr_ == NULL) {
      debug("Could not map view of file: %", gupta::GetLastErrorAsString());
      return 0;
    }

    debug("succeffully opened: %", Name_);
    return Semaphore_.open(semaphore_name());
  }

  ~SharedMemory() {
    if (BufPtr_ && !UnmapViewOfFile(BufPtr_))
      debug("UnMapFileView Failed: %", gupta::GetLastErrorAsString());
    else {
      debug("unmapped successfully");
    }
  }

private:
  gupta::WinHandle hMapFile_;
  std::string Name_;
  void *BufPtr_ = nullptr;
  int BufSize_ = 1024;
  Semaphore Semaphore_;
  inline std::string semaphore_name() { return Name_ + "Semaphore"; }
};

} // namespace gupta

#endif
