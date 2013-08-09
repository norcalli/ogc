#ifndef ARGUMENTS_H_
#define ARGUMENTS_H_

class Arguments {
 public:
  class DefaultArg {
   public:
    DefaultArg() : value_(NULL) { }

    DefaultArg(const char* value) : value_(value) { }

    const char* operator()(const char* def) {
      if (value_ == NULL)
        return def;
      return value_;
    }

    operator const char*() {
      return value_;
    }
   private:
    const char* value_;
  };

  Arguments(int argc, char** argv) : argc_(argc), argv_(argv) { }

  DefaultArg operator[](int idx) {
    if (argc_ <= (1 + idx))
      return DefaultArg(NULL);
    return argv_[1 + idx];
  }
 private:
  char** argv_;
  int argc_;
};

#endif // ARGUMENTS_H_
