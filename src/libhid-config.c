#include <hid.h>

#include <getopt.h>

void print_library_usage(char const prog[])
{
  fprintf(stderr, "usage: %s [OPTION]\n\n", prog);
  fprintf(stderr, "prints information about the library, which can be used for automated\n");
  fprintf(stderr, "compilation. for instance:\n\n");
  fprintf(stderr, "  gcc -c `%s --cppflags` -o test.o test.c\n", prog);
  fprintf(stderr, "  gcc `%s --ldflags` -o test test.o\n\n", prog);
  fprintf(stderr, "valid OPTIONS are:\n\n");
  fprintf(stderr, "  --name, -n                library name\n");
  fprintf(stderr, "  --arch, -a                library compile architecture\n");
  fprintf(stderr, "  --version, -v             numeric library version\n");
  fprintf(stderr, "  --full-version, -V        full library version string\n");
  fprintf(stderr, "  --libtool-age, -A         library age in libtool parlance\n");
  fprintf(stderr, "  --compile-date, -d        library compilation date\n");
  fprintf(stderr, "  --bugreport-address, -b   email address for bug reports\n");
  fprintf(stderr, "  --prefix, -p              library installation prefix\n");
  fprintf(stderr, "  --cppflags, -c            CPPFLAGS necessary to compile using the library\n");
  fprintf(stderr, "  --ldflags, -l             LDFLAGS necessary to link against the library\n");
  fprintf(stderr, "  --libtool-libs, -L        .la file to link with libtool\n");
  fprintf(stderr, "  --help, -h                this help text\n");
  fputc('\n', stderr);
}

int main(int argc, char** const argv)
{
  static char opts[] = "navVAdbpclLh?";
  static struct option lopts[] = {
    {"name", 0, 0, 'n'},
    {"arch", 0, 0, 'a'},
    {"version", 0, 0, 'v'},
    {"full-version", 0, 0, 'V'},
    {"libtool-age", 0, 0, 'A'},
    {"compile-date", 0, 0, 'd'},
    {"bugreport-address", 0, 0, 'b'},
    {"prefix", 0, 0, 'p'},
    {"cppflags", 0, 0, 'c'},
    {"ldflags", 0, 0, 'l'},
    {"libtool-libs", 0, 0, 'L'},
    {"help", 0, 0, 'h'},
    {0, 0, 0, 0}
  };

  char opt;
  int idx;

  opt = getopt_long(argc, argv, opts, lopts, &idx);

  if (opt == -1) {
    hid_write_library_config(stdout);
    return 0;
  }

  switch (opt) {
    
    case 'n': hid_write_library_name(stdout); break;
    case 'a': hid_write_library_arch(stdout); break;
    case 'v': hid_write_library_version(stdout); break;
    case 'V': hid_write_library_full_version(stdout); break;
    case 'A': hid_write_library_libtool_age(stdout); break;
    case 'd': hid_write_library_compile_date(stdout); break;
    case 'b': hid_write_library_bugreport_address(stdout); break;
    case 'p': hid_write_library_prefix(stdout); break;
    case 'c': hid_write_library_cppflags(stdout); break;
    case 'l': hid_write_library_ldflags(stdout); break;
    case 'L': hid_write_library_libtool_libs(stdout); break;

    default: print_library_usage(argv[0]); break;
  }

  return 0;
}
