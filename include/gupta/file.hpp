#ifndef GUPTA_FILE_HPP
#define GUPTA_FILE_HPP

#include <cstdio>
#include <type_traits>

namespace gupta {

class file {
public:
  explicit file(const char *file_name, const char *open_mode) : m_file_ptr{std::fopen(file_name, open_mode)} {}
  file(const file &) = delete;
  file(file &&other) { close_and_own(other.m_file_ptr); }
  file &operator=(const file &) = delete;
  file &operator=(file &&other) {
	  if (this != &other) {
		close_and_own(other.m_file_ptr);
	  }
	return *this;
  }
  ~file() { close(); }
  inline bool is_open() const { return m_file_ptr != nullptr; }
  
  inline size_t read(void *data, size_t size, size_t count)  {
  return std::fread(data,size,count,m_file_ptr);
  }

  template <typename PodType>
  std::enable_if_t<
      std::is_pod<PodType>::value && !std::is_pointer<PodType>::value, size_t>
  read(PodType &object) {
    return read(&object, sizeof(PodType), 1);
  }

  template <typename PodType>
  std::enable_if_t<
      std::is_pod<PodType>::value, size_t>
  read(PodType * object,size_t count) {
    return read(object, sizeof(PodType), count);
  }

  inline size_t write(const void *data, size_t size, size_t count) {
	  return std::fwrite(data,size,count,m_file_ptr);
  }

  template <typename PodType>
  std::enable_if_t<
      std::is_pod<PodType>::value && !std::is_pointer<PodType>::value, size_t>
  write(const PodType &object) {
    return write(&object, sizeof(PodType), 1);
  }

  template <typename PodType>
  std::enable_if_t<std::is_pod<PodType>::value, size_t> write(const PodType *object,
                                                             size_t count) {
    return write(object, sizeof(PodType), count);
  }
  
  bool eof() {
	return feof(m_file_ptr);
  }
  
  auto tellpos() { 
	return ftell(m_file_ptr);
  }
  
  template <typename String>
  size_t getline(String& str,char delim = '\n') {
	str.clear();
	char currentSymbl;
	while (this->read(&currentSymbl,1,1)) {
		if (currentSymbl == delim)
			break;
		str.push_back(currentSymbl);
	}
	return str.size();
  }
private:
  std::FILE *m_file_ptr;
  void close_and_own(std::FILE *&new_file) {
  close();
  if (new_file) {
    m_file_ptr = new_file;
    new_file = nullptr;
  }
}
  void close() {
  if (m_file_ptr)
    fclose(m_file_ptr);
}
};

} // namespace gupta

#endif