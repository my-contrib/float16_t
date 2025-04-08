project ('nntrainer-softfloat16', ['cpp'],
         default_options: ['cpp_std=c++20'],
         meson_version: '>=1.1')

host_cxx = meson.get_compiler('cpp', native: false)
build_cxx = meson.get_compiler('cpp', native: true)

host_cxx_has_constexpr_bit_cast = host_cxx.compiles(files('meson'/'check_for_bit_cast.cc'))
# Check if shift right of signed int is arithmetic shift (required since c++20)
host_cxx_shift_right_of_signed_is_asl = host_cxx.compiles(files('meson'/'check_for_asl.cc'))

if meson.is_cross_build()
  build_cxx_has_constexpr_bit_cast = build_cxx.compiles(files('meson'/'check_for_bit_cast.cc'))
  build_cxx_shift_right_of_signed_is_asl = build_cxx.compiles(files('meson'/'check_for_asl.cc'))
else
  build_cxx_has_constexpr_bit_cast = host_cxx_has_constexpr_bit_cast
  build_cxx_shift_right_of_signed_is_asl = host_cxx_shift_right_of_signed_is_asl
endif

assert(host_cxx_has_constexpr_bit_cast)
assert(host_cxx_shift_right_of_signed_is_asl)


_catch2_amal_dep = declare_dependency(
  include_directories: include_directories('tests'),
  sources: files('tests'/'catch_amalgamated.cpp'),
)

_interface_cpp_args = get_option('enable-debug-mode')? ['-DFLOAT16_T_DEBUG'] : []
float16_t_dep = declare_dependency(include_directories: include_directories('.'),
                                   compile_args: _interface_cpp_args)

_tst_exe = executable('host_tst', ['tests'/'test.cc'],
  build_by_default: false,
  dependencies: [float16_t_dep, _catch2_amal_dep]
)


test('test sanity', _tst_exe)

_fp_literals_exe = executable('fp_literals', ['fp16_storage_t.cc'],
                              dependencies: float16_t_dep)
