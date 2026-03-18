module;

#include <dlfcn.h>
#include <exception>
#include <type_traits>

#ifdef __USING_LOADER_H__
#    error "Cannot use loader.h with loader module"
#endif

export module loader;
export namespace loader {

typedef struct {
    char** syms;
    int nsyms;
} libinfo;

typedef libinfo* (*ctor_t)(void);
typedef void (*dtor_t)(libinfo*);

class loader_error : public std::exception {
  public:
    loader_error(const char* err) : _what(err) {}

    const char* what() const noexcept { return _what; }

  private:
    const char* _what;
};

class library {
  public:
    library(const char* path) : pth(path) {
        char* err;

        dlhdl = dlopen(path, RTLD_LAZY);

        if ((err = dlerror()) != nullptr) {
            throw loader_error(err);
        }

        ctor_t ctor = (ctor_t)dlsym(dlhdl, "ctor");
        li = ctor();
    }

    // disable copy - you don't want two owners of the same dlhandle
    library(const library&) = delete;
    library& operator=(const library&) = delete;

    // move is fine
    library(library&& other) noexcept
        : dlhdl(other.dlhdl), pth(other.pth), li(other.li) {
        other.dlhdl = nullptr;
        other.li = nullptr;
    }
    library& operator=(library&& other) noexcept {
        if (this != &other) {
            close();
            dlhdl = other.dlhdl;
            pth = other.pth;
            li = other.li;
            other.dlhdl = nullptr;
            other.li = nullptr;
        }
        return *this;
    }

    void close() {
        char* err;

        dtor_t dtor = (dtor_t)dlsym(dlhdl, "dtor");
        if ((err = dlerror()) != nullptr) {
            throw loader_error(err);
        }
        dtor(li);

        dlclose(dlhdl);
        if ((err = dlerror()) != nullptr) {
            throw loader_error(err);
        }
    }

    ~library() {
        if (dlhdl)
            close();
    }

    void* __gethandle() { return dlhdl; }

    libinfo* __getlibinfo() { return li; }

  private:
    void* dlhdl;
    const char* pth;
    libinfo* li;
};

template<typename R, typename... A>
struct libfn : std::false_type {};

template<typename R, typename... A>
struct libfn<R(A...)> : std::true_type {
    libfn(library& lib, const char* name) : _lib(&lib), _name(name) {
        void* sym = dlsym(_lib->__gethandle(), _name);
        char* err;
        if ((err = dlerror()) != nullptr) {
            throw loader_error(err);
        }
        _sym = sym;
    }
    libfn(library* lib, const char* name) : _lib(lib), _name(name) {
        void* sym = dlsym(_lib->__gethandle(), _name);
        char* err;
        if ((err = dlerror()) != nullptr) {
            throw loader_error(err);
        }
        _sym = sym;
    }

    R operator()(A... args) { return ((R (*)(A...))_sym)(args...); }

  private:
    library* _lib;
    const char* _name;
    void* _sym;
};

} // namespace loader
