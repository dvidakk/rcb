project('RedditChat', 'c',
  version: '0.1.0',
  default_options: ['c_std=c17', 'warning_level=3']
)

cc = meson.get_compiler('c')
if cc.get_id() == 'msvc'
  add_project_arguments('/W4', language: 'c')
else
  add_project_arguments('-Wpedantic', language: 'c')
endif

# find python3
python3 = find_program('python3')

cjson_proj = subproject('cjson')
cjson_dep = cjson_proj.get_variable('libcjson_dep')

curl_proj = subproject('curl')
curl_dep = curl_proj.get_variable('curl_dep')

# Source files
matrix_src = files(
  'src/matrix/models/login_response.c',
  'src/matrix/models/messages.c',
  'src/matrix/matrix_client.c',
)

reddit_src = files(
  'src/reddit/base64.c',
  'src/reddit/reddit.c',
)

utils_src = files(
  'src/utils/http.c',
  'src/utils/ini_utils.c',
)

sources = [
  'src/main.c',
] + matrix_src + reddit_src + utils_src

# Include directories
inc_dirs = include_directories('include')

executable_name = 'RedditChat'
executable = executable(executable_name, sources,
  dependencies: [curl_dep, cjson_dep],
  include_directories: inc_dirs
)

# Platform-specific settings for Windows
if host_machine.system() == 'windows'
  cjson_dll = join_paths(meson.build_root(), 'subprojects', 'cJSON-1.7.18', 'cjson-1.dll')
  curl_dll = join_paths(meson.build_root(), 'subprojects', 'curl-8.10.0', 'lib', 'curl-4.dll')

  copy_dll_script = join_paths(meson.source_root(), 'scripts', 'copy_dll.py')

  meson.add_postconf_script('python3', copy_dll_script, cjson_dll, meson.current_build_dir())
  meson.add_postconf_script('python3', copy_dll_script, curl_dll, meson.current_build_dir())
endif