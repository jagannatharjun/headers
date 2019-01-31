#ifndef GUPTA_BUFFER_HPP
#define GUPTA_BUFFER_HPP

#include <cstdint>
#include <cstring>
#include <gupta/file.hpp>
#include <gupta/podvector.hpp>

namespace gupta {

using byte = std::uint8_t;
using buffer = gupta::podvector<byte>;

/*
 * buffer_stream provides sequiential read and write to buffer through
 * convinient interface
 */
template <typename BufferType> class buffer_stream {
public:
  using size_type = std::size_t;
  using raw_buffer = std::pair<byte, std::size_t>;

  void reserve_bytes(size_type sz) { m_buffer.reserve(sz); }
  void rewind() { m_bufferPos = 0; }
  const BufferType &getbuffer() const { return m_buffer; }

  template <typename PodType>
  std::enable_if_t<std::is_pod<PodType>::value, buffer_stream &>
  operator<<(const PodType &pd) {
    size_type old_size = m_buffer.size();
    if (!isGivenMemAvailaible(sizeof(PodType)))
      m_buffer.resize(m_bufferPos + sizeof(PodType));
    std::memcpy(m_buffer.data() + m_bufferPos, &pd, sizeof(PodType));
    m_bufferPos += sizeof(PodType);
    return *this;
  }

  template <typename PodType>
  std::enable_if_t<std::is_pod<PodType>::value, buffer_stream &>
  operator>>(PodType &pd) {
    if (isGivenMemAvailaible(sizeof(PodType))) {
      std::memcpy(&pd, m_buffer.data() + m_bufferPos, sizeof pd);
      m_bufferPos += sizeof(pd);
    }
    return *this;
  }

  buffer_stream &operator<<(const raw_buffer &rb) {
    if (!isGivenMemAvailaible(rb.second)) {
      m_buffer.resize(m_bufferPos + rb.second);
    }
    std::memcpy(m_buffer.data() + m_bufferPos, rb.first, rb.second);
    m_bufferPos += rb.second;
    return *this;
  }

  buffer_stream &operator>>(raw_buffer &rb) {
    if (isGivenMemAvailaible(rb.second)) {
      std::memcpy(rb.first, m_buffer.data() + m_bufferPos, rb.second);
      m_bufferPos += rb.second;
    }
    return *this;
  }

private:
  BufferType m_buffer;
  size_type m_bufferPos = 0;

  bool isGivenMemAvailaible(size_t requiredBytes) const {
    return m_bufferPos + requiredBytes <= m_buffer.size();
  }
};

static void dump(const byte *b, size_t sz, const char *file_name) {
  gupta::file f(file_name, "wb");
  f.write(b, sz);
}

} // namespace gupta

#endif
