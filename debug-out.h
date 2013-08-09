#ifndef DEBUG_OUT_H_
#define DEBUG_OUT_H_


void Logging(const char* fn, int line, const char* fmt, ...) {
  va_list va;
  va_start(va, fmt);
  // fprintf(stderr, "%s: %d: ", fn, line);
  vfprintf(stderr, fmt, va);
  va_end(va);
}

#ifdef DEBUG
#define dprintf(...)                            \
  Logging(__FUNCTION__, __LINE__, __VA_ARGS__)
#define dexec(x) x
#else
#define dprintf(...)
#define dexec(x)
#endif


#endif  // DEBUG_OUT_H_
